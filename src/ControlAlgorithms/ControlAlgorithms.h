/// class:		ControlAlgorithms
/// brief:		Encapsulates the needle steering control and estimation algorithms.
///
///	author:		Troy Adebar
///	date:		  April 27, 2015

#pragma once

#include <time.h>
#include "SegmentCore.h"

namespace Nf {

  class ControlAlgorithms 
  {

  public:
    ControlAlgorithms();
    ~ControlAlgorithms();
    void SetTarget(Vec3d target);
    void GetTarget(Vec3d &target);
    bool isTargetDefined();

  private:
    Vec3d m_target; 
    bool m_targetDefined;

  };

};

