#include "SegmentationAlgorithms.h"

using namespace cv;

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
    if( m_scanFrames.size() < 1 )
      return Matrix44d::I();

    for( int i = 0; i < m_scanFrames.size(); i++ )
    {
      RPData frame = m_scanFrames[i];
      processFrame(frame);
    }
  }

  void InPlaneSegmentation::processFrame(RPData data)
  {
     cvSaveImage("F:/bmode.jpg", data.b8);
     cvSaveImage("F:/doppler.jpg", data.color);

    CvSize s = cvSize(data.color->width,data.color->height);
     int d = data.color->depth;
     IplImage* R = cvCreateImage(s, d, 1);
     IplImage* G = cvCreateImage(s, d, 1);
     IplImage* B = cvCreateImage(s, d, 1);
     IplImage* X = cvCreateImage(s, d, 1);
     cvSplit(data.color, R, G, B, X);
    
     cvSaveImage("F:/R.jpg", R);
     cvSaveImage("F:/G.jpg", G);
     cvSaveImage("F:/B.jpg", B);
     cvSaveImage("F:/X.jpg", X);
     
     // convert the color doppler image to a grayscale
     IplImage* rawGray = cvCreateImage(s, d, 1);
     cvCvtColor(data.color,rawGray,CV_RGBA2GRAY);
     
     // mask the color by comparing channels
     IplImage* mask = cvCreateImage(s, d, 1);
     IplImage* gray = cvCreateImage(s, d, 1);
     cvZero(gray);
     cvAbsDiff(R, G, mask);
     cvCopy(rawGray,gray,mask);

     cvSaveImage("F:/grayed.jpg",gray);
     
     //// iterate through the pixels, and save the coordinates of non-zero pixels
     //for (int i = 0; i < gray->width; i++ ) {
     //  for (int j = 0; j < gray->height; j++) {
     //    if (img.at<uchar>(j, i) == 255) {   
     //      cout << i << ", " << j << endl;     // Do your operations
     //    }
     //  }
     //}
   

     }

  
};