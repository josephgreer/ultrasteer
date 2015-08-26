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
#include <QtGui/QFileDialog>

namespace Nf {
  
  class ControlAlgorithms 
  {

  public:
    ControlAlgorithms();
    ~ControlAlgorithms();
    void startStopManualScanning(bool start);
    bool startStopTeleoperation();
    bool inFollowing();
    bool inManualScanning();
    
    void controlHeartBeat(RPData);
    void onUpdateOverlay();

    void SetTarget(Vec2d t_im);
    void GetTarget(Vec3d &t);
    bool isTargetDefined();
    void resetManualScan();
    Matrix44d processManualScan();
    void setRobot(NeedleSteeringRobot* robot);
    void GetIncrementalInputVector(Vec3d &u);
    void ControlCorrection();
    Vec3d ImagePtToRobotPt(Vec2d p_im);
    Vec3d RobotPtToImagePt(Vec3d p_robot);
    bool isCalibrationSet();
    void setCalibration(Matrix44d, Matrix44d);

    void getOverlayValues(bool &show_p, Vec3d &p_img, Vec3d &pz_img, Vec3d &py_img, Vec3d &p, Matrix33d &R,
                          bool &show_t, Vec3d &t_img, Vec3d &t,
                          bool &show_S, Vec3d &Sxyz);

  private:
    void GetPoseEstimate(Matrix44d &x);
    bool CheckCompletion();

  private:
    Vec3d m_t;
    Matrix44d m_x;
    Matrix44d m_measurement;
    Matrix44d m_usCalibrationMatrix;
    Matrix44d m_Tref2robot;
    bool m_targetDefined;
    bool m_estimateDefined;
    bool m_inManualScanning;
    bool m_inFollowing;
    InPlaneSegmentation m_segmentation; 
    UnscentedKalmanFilter m_UKF;
    NeedleSteeringRobot* m_robot;
    
    double m_lastInsMM;
    double m_lastRollDeg;

    RPData m_data;

    bool m_calibrationSet;

  };

  class StylusCalibration
  {
  public:
    StylusCalibration();
    ~StylusCalibration();
    void addMeasurement(arma::mat, arma::mat);
    void clearMeasurements(void);
    void solveCalibration(void);
    arma::mat getCalibrationVector(void);
    arma::mat getCenter(void);
    bool isComplete(void);

  protected:
    arma::mat m_A;
    arma::mat m_b;
    arma::mat m_vcal;
    arma::mat m_c;
    bool m_stylusCalibrationComplete;
  };
}

