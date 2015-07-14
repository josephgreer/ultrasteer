#include "SegmentationAlgorithms.h"


namespace Nf {

  InPlaneSegmentation::InPlaneSegmentation()
  {
  }

  InPlaneSegmentation::~InPlaneSegmentation()
  {
  }

  void InPlaneSegmentation::addManualScanFrame(RPData data)
  {
    m_scanFrames.push_back(data.Clone());
  }

  void InPlaneSegmentation::resetManualScan()
  {
    RPData frame;

    for( int i = m_scanFrames.size(); i>0 ; i-- )
    {
      frame = m_scanFrames[i-1];
      frame.Release();
      m_scanFrames.pop_back();
    }
  }

  Matrix44d InPlaneSegmentation::processManualScan()
  {

    if( !m_dopplerPoints.empty() )
      m_dopplerPoints.clear();

    if( m_scanFrames.size() < 1 )
      return Matrix44d::I();

    for( int i = 0; i < m_scanFrames.size(); i++ )
    {
      RPData frame = m_scanFrames[i];
      processFrame(frame);
    }

    return fitPolynomialToDoppler();

  }

  // Separate combined BGR doppler + B-mode image into grayscale Doppler only and mask
  void InPlaneSegmentation::separateColor(IplImage* combined, IplImage* gray, IplImage* binary)
  {
    using namespace cv;

    CvSize s = cvSize(combined->width, combined->height);
    int d = combined->depth;
    IplImage* R = cvCreateImage(s, d, 1);
    IplImage* G = cvCreateImage(s, d, 1);
    IplImage* B = cvCreateImage(s, d, 1);
    IplImage* X = cvCreateImage(s, d, 1);
    cvSplit(combined, B, G, R, X);

    // convert the color doppler image to grayscale
    IplImage* rawGray = cvCreateImage(s, d, 1);
    cvCvtColor(combined,rawGray,CV_RGBA2GRAY);

    // mask out the color by comparing channels
    IplImage* diff = cvCreateImage(s, d, 1);
    IplImage* mask_all = cvCreateImage(s, d, 1);
    IplImage* gray_all = cvCreateImage(s, d, 1);
    cvAbsDiff(R, G, diff);
    cvThreshold(diff, mask_all, 1, 255, THRESH_BINARY);
    cvZero(gray_all);
    cvCopy(rawGray, gray_all, mask_all);

    // remove doppler noise below a low value
    cvThreshold(gray_all,gray,8.0,255,THRESH_TOZERO);
    cvZero(gray);    
    cvCopy(gray_all, gray, binary);

    // set a binary mask for all remaining doppler
    cvThreshold(gray, binary, 1, 255, THRESH_BINARY);

    // clean up
    cvReleaseImage(&R);
    cvReleaseImage(&G);
    cvReleaseImage(&B);
    cvReleaseImage(&X);
    cvReleaseImage(&rawGray);
    cvReleaseImage(&mask_all);
    cvReleaseImage(&gray_all);
    cvReleaseImage(&diff);
  }

  // Extract largest connected region
  void InPlaneSegmentation::largestBlob(IplImage* dopplerAllBin, cv::Mat &dopplerLargestBin)
  {
    using namespace cv;

    // dilate
    Mat tempMask(dopplerAllBin);
    int erosion_size = 3;
    Mat element = getStructuringElement(MORPH_ELLIPSE,
      Size( 2*erosion_size + 1, 2*erosion_size+1 ),
      Point( erosion_size, erosion_size ) );
    dilate(tempMask,tempMask,element);

    // find the largest blob
    int largest_area=0;
    int largest_contour_index=0;
    Rect bounding_rect;
    vector<vector<Point>> contours;
    vector<cv::Vec4i> hierarchy;

    Mat temp_image;
    tempMask.copyTo(temp_image);
    findContours(temp_image, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    Mat b(contours[0]);
    for( int i = 0; i< contours.size(); i++ ) // iterate through each contour. 
    {
      double a=contourArea(Mat(contours[i]),false);  //  Find the area of contour
      if(a>largest_area){
        largest_area=a;
        largest_contour_index=i;                //Store the index of largest contour
        bounding_rect=boundingRect(Mat(contours[i])); // Find the bounding rectangle for biggest contour
      }
      tempMask.setTo(Scalar(0));
      drawContours( tempMask, contours, largest_contour_index, Scalar(255), CV_FILLED, 8, hierarchy ); // Draw the largest contour using previously stored index.
    }

    // erode the image
    erode(tempMask,tempMask,element);
    dopplerLargestBin = tempMask.clone();
  }

  void InPlaneSegmentation::processFrame(RPData data)
  {
    using namespace cv;

    // create image memory
    CvSize s = cvSize(data.color->width, data.color->height);
    int d = data.color->depth;
    IplImage* gray = cvCreateImage(s, d, 1);
    IplImage* binary = cvCreateImage(s, d, 1);

    // separate color channels 
    separateColor(data.color, gray, binary);

    // dilate, erode, and isolate largest blob
    Mat gray_mat;
    largestBlob(binary, gray_mat);
    
    // define values to convert 2D image points to 3D using GPS data
    Matrix44d cal(14.8449, 0.9477, -0.0018, 0.0, 15.0061, 0.0016, 1.00, 0.0, 0.1638, 0.0166, 0.0052, 0.0, 0.0, 0.0, 0.0, 1.0);
    Matrix44d tPose = Matrix44d::FromCvMat(data.gps.pose);
    Matrix33d pose = tPose.GetOrientation();
    Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, data.gps.pos);
    Vec2d scale(data.mpp/1000.0, data.mpp/1000.0);

    BEGIN_TIMING(DopplerPointSaving,3);
    // iterate through the pixels, and save the coordinates of non-zero pixels
    for (int i = 0; i < gray_mat.cols; i++ ) {
      for (int j = 0; j < gray_mat.rows; j++) {
        if (gray_mat.at<uchar>(j, i) > 0) {   
          //NTrace("x= %d, y=%d\n", i, j);
          m_dopplerPoints.push_back(rpImageCoordToWorldCoord3(Vec2d(i,j), posePos, cal, data.origin, scale));
        }else{
          int i = 1;
        }
      } 
    }
    END_TIMING(DopplerPointSaving,3)

    // save images for debugging
    cvSaveImage("F:/bmode.jpg", data.b8);
    cvSaveImage("F:/doppler.jpg", data.color);
    cvSaveImage("F:/binary.jpg",binary);
    cvSaveImage("F:/gray.jpg",gray);
    imwrite("F:/graySingle.jpg",gray_mat);

    // Clean up
    cvReleaseImage(&binary);
    cvReleaseImage(&gray);
  }

  Matrix44d InPlaneSegmentation::fitPolynomialToDoppler()
  {
    using namespace arma;
    if( m_dopplerPoints.size() < 1 )
      return Matrix44d::I();

    // define the independent and dependent coordinates for curve fitting
    int i_i=2;          //z is independent
    int i_d1=0, i_d2=1; // x and y and dependent

    // format the problem as Av = b
    mat A;
    vec b1(m_dopplerPoints.size());
    vec b2(m_dopplerPoints.size());

    mat row = ones<mat>(1,4);
    vec pt(3);

    for( int i = 0; i < m_dopplerPoints.size(); i++ ){
      // extract the current point
      pt(0) = m_dopplerPoints[i].x;
      pt(1) = m_dopplerPoints[i].y;
      pt(2) = m_dopplerPoints[i].z;

      // save the powers of the independent coordinate into a matrix row
      row(0,0) = pt(i_i)*pt(i_i)*pt(i_i);
      row(0,1) = pt(i_i)*pt(i_i);
      row(0,2) = pt(i_i);
      A.insert_rows(i,row);

      // save the dependent coordinates into vectors
      b1(i) = pt(i_d1);
      b2(i) = pt(i_d2);
    }

    // use the pseudo inverse of A to find the polynomial fits for dependent coordinates
    vec v1 = pinv(A)*b1;
    vec v2 = pinv(A)*b2;

    NTrace("a_x= { %f, %f, %f, %f}\n", v1(0), v1(1), v1(2), v1(3));
    NTrace("a_y= {  %f, %f, %f, %f}\n", v2(0), v2(1), v2(2), v2(3));
    return Matrix44d::I();

  }
};

