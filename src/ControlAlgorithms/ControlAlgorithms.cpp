#include "ControlAlgorithms.h"
#include "math.h"
#include <time.h>

#define   RHO     60.0    // radius of curvature for needle in mm

namespace Nf {

  ControlAlgorithms::ControlAlgorithms()
  {
    m_targetDefined = false;
    m_inFollowing = false;
    m_inManualScanning = false;
    m_estimateDefined = false;

    m_cal = Matrix44d(14.8449, 0.9477, -0.0018, 0.0, 15.0061, 0.0016, 1.00, 0.0, 0.1638, 0.0166, 0.0052, 0.0, 0.0, 0.0, 0.0, 1.0);
  }

  ControlAlgorithms::~ControlAlgorithms()
  {
  }

  // return true if the robot is in following; false otherwise
  bool ControlAlgorithms::inFollowing()
  {
    return m_inFollowing;
  }

  // return true if a manual scan is underway; false otherwise
  bool ControlAlgorithms::inManualScanning()
  {
    return m_inManualScanning;
  }

  // toggle the teleoperation state, and return inFollowing()
  bool ControlAlgorithms::startStopTeleoperation()
  {
    m_inFollowing = !m_inFollowing;
    return inFollowing();
  }

  // keep going HERE
  void ControlAlgorithms::startStopManualScanning(bool start)
  {
    if( start ){
        resetManualScan();
        m_inManualScanning = true;
    }else{
      m_inManualScanning = false;
      m_x = processManualScan();
      m_estimateDefined = true;
    }
  }
  
  void ControlAlgorithms::SetTarget(Vec2d t_im)
  {
    m_t = ImagePtToWorldPt(t_im);
    m_targetDefined = true;
  }

  Vec3d ControlAlgorithms::ImagePtToWorldPt(Vec2d p_im)
  {
    if(m_data.gps.valid)
    { 
      Matrix44d tPose = Matrix44d::FromCvMat(m_data.gps.pose);
      Matrix33d pose = tPose.GetOrientation();
      Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, m_data.gps.pos);
      Vec2d scale(m_data.mpp.x/1000.0, m_data.mpp.y/1000.0);
      return rpImageCoordToWorldCoord3(p_im, posePos, m_cal, m_data.origin, scale);
    }else{
      return Vec3d(999.9,999.9,999.9);
    }
  }

  Vec3d ControlAlgorithms::WorldPtToImagePt(Vec3d p_world)
  {
    if(m_data.gps.valid)
    { 
      Matrix44d tPose = Matrix44d::FromCvMat(m_data.gps.pose);
      Matrix33d pose = tPose.GetOrientation();
      Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, m_data.gps.pos);
      Vec2d scale(m_data.mpp.x/1000.0, m_data.mpp.y/1000.0);
      return rpWorldCoord3ToImageCoord(p_world, posePos, m_cal, m_data.origin, scale);
    }else{
      return Vec3d(999.9,999.9,999.9);
    }
  }

  void ControlAlgorithms::controlHeartBeat(RPData data)
  {
    if(m_data.gps.valid)
    {
      m_data.Release();
    }
    
    m_data = data.Clone();

    if( m_inManualScanning ) // if manually scanning
      m_segmentation.addManualScanFrame(data);
      
    if( true) // if in following
      int i = 1; // do controlly stuff
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

  void ControlAlgorithms::getOverlayValues(bool &show_p, Vec3d &p_img, Vec3d &pz_img, Vec3d &py_img, Vec3d &p, Matrix33d &R,
                                           bool &show_t, Vec3d &t_img, Vec3d &t,
                                           bool &show_S, Vec3d &Sxyz)
  {
    // default to not showing anything
    show_p = false;
    show_t = false;
    show_S = false;

    if( m_targetDefined ){ // if we have defined the target
      t_img = WorldPtToImagePt(m_t);

      if( fabs(t_img.z) < 10.0 ){ // if the target is within 5 mm of the image plane
        show_t = true;
        t = m_t;
      }
    }

    if( m_estimateDefined ){ // if we have defined the estimate
      Matrix44d T;
      T = m_x;
      //GetPoseEstimate(T);
      p = T.GetPosition();
      R = T.GetOrientation();
      p_img = WorldPtToImagePt(p);

      if( fabs(p_img.z) < 10.0 ){ // if the tip estimate is within 5 mm of the image plane
        show_S = true;
        Sxyz = m_UKF.getCurrentXYZVariance();

        show_p = true;
        Matrix33d R = T.GetOrientation();
        Vec3d pz_world = p + R*Vec3d(0.0,0.0,10.0);
        Vec3d py_world = p + R*Vec3d(0.0,5.0,0.0);
        pz_img = WorldPtToImagePt(pz_world);
        py_img = WorldPtToImagePt(py_world);
      }
    }
  }

  StylusCalibration::StylusCalibration()
  {
    m_stylusCalibrationComplete = false;
  }

  StylusCalibration::~StylusCalibration()
  {
  }

  void StylusCalibration::addMeasurement(arma::mat R_i, arma::mat p_i)
  {
    using namespace arma;
    m_A.insert_rows(m_A.n_rows,join_rows(R_i,-eye(3,3)));
    m_b.insert_rows(m_b.n_rows,-p_i);
  }

  void StylusCalibration::clearMeasurements(void)
  {
    using namespace arma;
    m_A.reset();
    m_b.reset();
    m_stylusCalibrationComplete = false;
  }

  void StylusCalibration::solveCalibration(void)
  {
    arma::mat x = arma::solve(m_A,m_b);
    m_vcal = x.rows(0,2);
    m_c = x.rows(3,5);
    m_stylusCalibrationComplete = true;
  }

  arma::mat StylusCalibration::getCalibrationVector(void)
  {
    return m_vcal;
  }

  arma::mat StylusCalibration::getCenter(void)
  {
    return m_c;
  }

  bool StylusCalibration::isComplete(void)
  {
    return m_stylusCalibrationComplete;
  }
};