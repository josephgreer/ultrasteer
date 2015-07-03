#ifndef SEGMENTATIONALGORITHMS_H
#define SEGMENTATIONALGORITHMS_H

#include "RPFileReader.h"
#include "RTCore.h"

/// class:		InPlaneSegmentation
/// brief:		Analyze in-plane Doppler images of steerable needle to measure tip pose
///
///	author:		Troy Adebar
///	date:		  July 2, 2015

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

  private:
    std::vector <RPData> m_scanFrames;
    Matrix44d measurement;
  };
}

#endif