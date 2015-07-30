#ifndef SEGMENTATIONALGORITHMS_H
#define SEGMENTATIONALGORITHMS_H

#include "RPFileReader.h"
#include "RTCore.h"

/// class:		InPlaneSegmentation
/// brief:		Analyze in-plane Doppler images of steerable needle to measure tip pose
///
///	author:		Troy Adebar
///	date:		  July 2, 2015

#include <armadillo>

namespace Nf
{
  class InPlaneSegmentation
  {
  public: //Methods
    InPlaneSegmentation(void);
    ~InPlaneSegmentation(void);
    void addManualScanFrame(RPData);
    void resetManualScan();
    Matrix44d processManualScan();
    void processFrame(RPData);    
    void fitPolynomialandLineToDoppler();
    void findExtents();
    void measureTipFrame();

  private:
    std::vector <RPData> m_scanFrames;
    Matrix44d measurement;
    void separateColor(IplImage* combined, IplImage* doppler, IplImage* mask);
    void largestBlob(IplImage* dopplerAll, cv::Mat &dopplerLargest);
    arma::vec m_tipPt;
    Matrix44d m_tipFrame;
    arma::vec m_c1lin, m_c2lin, m_c1poly, m_c2poly;
    arma::mat m_dopplerPoints;
    int m_NdopplerPoints;

  };
}

#endif