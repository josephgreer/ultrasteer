#define I_IND   2    // z is independent
#define I_DEP1  0    // x is dependent
#define I_DEP2  1    // y is dependent
#define DIR     -1.0 // insertion is along the -ve z direction

#include "SegmentationAlgorithms.h"


namespace Nf {

  InPlaneSegmentation::InPlaneSegmentation()
  {
    m_tipPt = arma::zeros<arma::vec>(3);
    m_tipFrame = Matrix44d::I();
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
    if( !m_dopplerPoints.empty() )
      m_dopplerPoints.clear();

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

    // Reset and preallocate points matrix
    m_dopplerPoints.reset();
    m_dopplerPoints = arma::zeros<arma::mat>(100000,3);
    m_NdopplerPoints = 0;

    if( m_scanFrames.size() < 1 )
      return Matrix44d::I();

    for( int i = 0; i < m_scanFrames.size(); i++ )
    {
      RPData frame = m_scanFrames[i];
      processFrame(frame);
    }

    fitPolynomialandLineToDoppler();
    findExtents();
    return measureTipFrame();
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

  // Extract 3D coordinates of all Doppler points
  void InPlaneSegmentation::processFrame(RPData data)
  {
    using namespace cv;
    using namespace arma;

    // create image memory
    CvSize s = cvSize(data.color->width, data.color->height);
    int d = data.color->depth;
    IplImage* gray = cvCreateImage(s, d, 1);
    IplImage* binary = cvCreateImage(s, d, 1);

    // separate color channels 
    separateColor(data.color, gray, binary);

    // dilate, erode, and isolate largest blob
    cv::Mat gray_mat;
    largestBlob(binary, gray_mat);

    // define values to convert 2D image points to 3D using GPS data
    Matrix44d cal(14.8449, 0.9477, -0.0018, 0.0, 15.0061, 0.0016, 1.00, 0.0, 0.1638, 0.0166, 0.0052, 0.0, 0.0, 0.0, 0.0, 1.0);
    Matrix44d tPose = Matrix44d::FromCvMat(data.gps.pose);
    Matrix33d pose = tPose.GetOrientation();
    Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, data.gps.pos);
    Vec2d scale(data.mpp/1000.0, data.mpp/1000.0);

    //BEGIN_TIMING(DopplerPointSaving,10);
    // iterate through the pixels, and save the coordinates of non-zero pixels
    for (int i = 0; i < gray_mat.cols; i++ ) {
      for (int j = 0; j < gray_mat.rows; j++) {
        if (gray_mat.at<uchar>(j, i) > 0) {   
          //NTrace("x= %d, y=%d\n", i, j);
          Vec3d pt = rpImageCoordToWorldCoord3(Vec2d(i,j), posePos, cal, data.origin, scale);
          m_dopplerPoints(m_NdopplerPoints,0) = pt.x;
          m_dopplerPoints(m_NdopplerPoints,1) = pt.y;
          m_dopplerPoints(m_NdopplerPoints,2) = pt.z;
          m_NdopplerPoints++;
        }
      } 
    }
    //END_TIMING(DopplerPointSaving,10)

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

  // Fit polynomial and line to 3D Doppler coordinates
  void InPlaneSegmentation::fitPolynomialandLineToDoppler()
  {
    //BEGIN_TIMING(CURVEFITTING,1);

    using namespace arma;
    if( m_dopplerPoints.size() < 1 )
      return;

    // remove extra empty memory from the Doppler points matrix
    m_dopplerPoints.resize(m_NdopplerPoints,3);

    // to format the problem as Av = b, create the A matrices
    mat A_lin = join_horiz(m_dopplerPoints.col(I_IND),ones<mat>(m_NdopplerPoints,1));
    mat A_pol = ones<mat>(m_NdopplerPoints,1);
    for( int i = 0; i<3; i++ )
      A_pol.insert_cols(0,m_dopplerPoints.col(I_IND)%A_pol.col(0));

    // extract the dependent coordinate columns
    vec b1 = m_dopplerPoints.col(I_DEP1);
    vec b2 = m_dopplerPoints.col(I_DEP2);

    // use the pseudo inverse of A to find the polynomial fits for dependent coordinates
    m_c1poly = pinv(A_pol)*b1;
    m_c2poly = pinv(A_pol)*b2;
    NTrace("a_poly_x= { %f, %f, %f, %f}\n", m_c1poly(0), m_c1poly(1), m_c1poly(2), m_c1poly(3));
    NTrace("a_poly_y= {  %f, %f, %f, %f}\n", m_c2poly(0), m_c2poly(1), m_c2poly(2), m_c2poly(3));

    // use the pseudo inverse of A to find the linear fits for dependent coordinates
    m_c1lin = pinv(A_lin)*b1;
    m_c2lin = pinv(A_lin)*b2;
    NTrace("a_lin_x= { %f, %f }\n", m_c1lin(0), m_c1lin(1));
    NTrace("a_lin_y= {  %f, %f }\n", m_c2lin(0), m_c2lin(1));

    //END_TIMING(CURVEFITTING,1)
  }

  // Find the location of the tip of the best-fit polynomial
  void InPlaneSegmentation::findExtents()
  {
    using namespace arma;

    // Rearrange the best-fit line into p = o + s*v parameterization
    mat o = zeros<mat>(1,3);
    vec v = ones<vec>(3);
    o(0,I_DEP1) = m_c1lin(1);
    o(0,I_DEP2) = m_c2lin(1);
    v(I_DEP1) = m_c1lin(0);
    v(I_DEP2) = m_c2lin(0);
    mat O = repmat(o,m_NdopplerPoints,1);

    // Find the projection of each point onto the best-fit line
    mat proj = (m_dopplerPoints-O)*v;

    // Find the points that are farthest along the best-fit line
    uword min_row, max_row, col;
    double maxVal = proj.max(max_row, col);
    double minVal = proj.min(min_row, col);

    // Extract the minimum and maximum extent coordinates
    mat pt_a = m_dopplerPoints.row(max_row);
    mat pt_b = m_dopplerPoints.row(min_row);

    // Pick the tip point
    if( DIR*pt_a(0,I_IND) > DIR*pt_b(0,I_IND) )
      m_tipPt = trans(pt_a);
    else
      m_tipPt = trans(pt_b);
  }

  // Find the orientation of the needle tip frame based on the polynomial
  Matrix44d InPlaneSegmentation::measureTipFrame()
  {
    using namespace arma;
    
    // Find the first derivative at the tip, this is the z-axis of the tip frame
    vec z_axis(3);
    z_axis(I_DEP1) = m_c1poly(2) + 2*m_c1poly(1)*m_tipPt(I_IND) + 3*m_c1poly(0)*pow(m_tipPt(I_IND),2);
    z_axis(I_DEP2) = m_c2poly(2) + 2*m_c2poly(1)*m_tipPt(I_IND) + 3*m_c2poly(0)*pow(m_tipPt(I_IND),2); 
    z_axis(I_IND) = 1;
    z_axis = normalise(z_axis);

    // Find the second derivative at the tip, this is the y-axis and z-axis combined
    vec y_axis(3);
    y_axis(I_DEP1) = 2*m_c1poly(1) + 6*m_c1poly(0)*m_tipPt(I_IND);
    y_axis(I_DEP2) = 2*m_c2poly(1) + 6*m_c2poly(0)*m_tipPt(I_IND);
    y_axis(I_IND) = 0;
    y_axis = normalise(y_axis);
    
    // Remove z-axis components from y_axis
    y_axis = y_axis - dot(y_axis,z_axis)*z_axis;
    y_axis = normalise(y_axis);

    // Flip the axes if insertion is along negative indep. axis
    if (DIR < 0){ 
      z_axis = -1*z_axis; 
      y_axis = -1*y_axis; 
    }

    // Create rotation matrix representation of the frame
    vec x_axis = cross(y_axis, z_axis);
    mat R;
    R.insert_cols(0,z_axis);
    R.insert_cols(0,y_axis);
    R.insert_cols(0,x_axis);

    // Create 4x4 transformation matrix
    mat T = R;
    T.insert_cols(3,m_tipPt);
    mat h_row;
    h_row << 0 << 0 << 0 << 1;
    T.insert_rows(3,h_row);

    return Matrix44d::FromArmaMatrix4x4(T);

  }
  }