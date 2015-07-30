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
#include "UnscentedKalmanFilter.h"
#include "NeedleSteeringRobot.h"

namespace Nf {

  class ControlAlgorithms 
  {

  public:
    ControlAlgorithms();
    ~ControlAlgorithms();
    void SetTarget(Vec3d t);
    void GetTarget(Vec3d &t);
    void GetPoseEstimate(Matrix44d x);
    bool isTargetDefined();
    void addManualScanFrame(RPData);
    void resetManualScan();
    void processManualScan();
    void setRobot(NeedleSteeringRobot* robot);
    void GetIncrementalInputVector(Vec3d &u);

  private:
    Vec3d m_t;
    Matrix44d m_measurement;
    bool m_targetDefined;
    InPlaneSegmentation m_segmentation; 
    UnscentedKalmanFilter m_UKF;
    NeedleSteeringRobot* m_robot;

    double m_lastInsMM;
    double m_lastRollDeg;

  };
};

