/// class:		ControlAlgorithms
/// brief:		Encapsulates the needle steering control and estimation algorithms.
///
///	author:		Troy Adebar
///	date:		  April 27, 2015

#pragma once

#include <time.h>
#include "RTCore.h"
#include "RPFileReader.h"
#include "SegmentationAlgorithms.h"

namespace Nf {

  class ControlAlgorithms 
  {

  public:
    ControlAlgorithms();
    ~ControlAlgorithms();
    void SetTarget(Vec3d target);
    void GetTarget(Vec3d &target);
    bool isTargetDefined();
    void addManualScanFrame(RPData);
    void resetManualScan();
    Matrix44d processManualScan();

  private:
    Vec3d m_target;
    Matrix44d m_measurement;
    bool m_targetDefined;
    InPlaneSegmentation m_segmentation; 

  };

};
