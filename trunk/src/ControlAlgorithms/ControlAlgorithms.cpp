#include "ControlAlgorithms.h"
#include "math.h"
#include <time.h>

#define   RHO   60.0  // radius of curvature for needle in mm

namespace Nf {

  ControlAlgorithms::ControlAlgorithms()
  {
    m_targetDefined = false;
  }

  ControlAlgorithms::~ControlAlgorithms()
  {
  }

  void ControlAlgorithms::SetTarget(Vec3d t)
  {
    m_t = t;
    m_targetDefined = true;
  }

  void ControlAlgorithms::GetTarget(Vec3d &t)
  {
    t = m_t;
  } 

  void ControlAlgorithms::GetPoseEstimate(Matrix44d &x)
  {
    Vec3d u;
    GetIncrementalInputVector(u);
    //m_UKF.processUpdateUKF(u);

    m_UKF.getCurrentStateEstimate(x);
  }

  void ControlAlgorithms::GetIncrementalInputVector(Vec3d &u)
  {
    double currentInsMM = m_robot->getInsMM();
    double l = currentInsMM - m_lastInsMM;

    double currentRollDeg = m_robot->getRollAngle();
    double d_th = currentRollDeg - m_lastRollDeg;

    u = Vec3d(d_th,RHO,l);

    m_lastInsMM = currentInsMM;
    m_lastRollDeg = currentRollDeg;
  }

  bool ControlAlgorithms::isTargetDefined()
  {
    return m_targetDefined;
  } 

  void ControlAlgorithms::addManualScanFrame(RPData data)
  {
    m_segmentation.addManualScanFrame(data);
  }

  void ControlAlgorithms::resetManualScan()
  {
    m_segmentation.resetManualScan();
  }

  Matrix44d ControlAlgorithms::processManualScan()
  {
    Vec3d u;
    GetIncrementalInputVector(u);
    Matrix44d T = m_segmentation.processManualScan();
    m_UKF.fullUpdateUKF(u, T);
    return T;
  }
  
  void ControlAlgorithms::setRobot(NeedleSteeringRobot* robot)
  {
    m_robot = robot;
  }
  
  void ControlAlgorithms::ControlCorrection()
  {
    if(m_targetDefined)
    {
    // Get current tip frame estimate
    Matrix44d T = m_UKF.getCurrentEstimate();
    Vec3d p = T.GetPosition();
    Matrix33d R = T.GetOrientation();

    // Get relative error in the current tip frame
    Vec3d e = R.Inverse()*(m_t-p);
    f32 d_th = atan2(-e.x,e.y);
    
    // Check if needle tip is past target
    if( e.z < 0 )
      int i = 1;// Do end of steering tasks

    // Correct needle rotation
    m_robot->RotateIncremental(d_th);

    }else{
      NTrace("No target defined; skipping correction\n");
    }
  }
};