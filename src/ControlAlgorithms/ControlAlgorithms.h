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
#include <cmath>

namespace Nf {

  class STrigger;
  
  class ControlAlgorithms 
  {

  public:
    ControlAlgorithms();
    ~ControlAlgorithms();
    void startStopManualScanning(bool start);
    bool startStopTaskSpaceControl();
    bool startStopJointSpaceControl();
    bool inTaskSpaceControl();
    bool inJointSpaceControl();
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
    void setCalibration(Matrix44d, Matrix44d, s32);
    void updateTransducerPose();
    void setJointSpaceControlVelocities(f32 v_rot, f32 v_ins);
    void initializeEstimator();

    void getOverlayValues(bool &show_p, Vec3d &p_img, Vec3d &pz_img, Vec3d &py_img, Vec3d &p, Matrix33d &R,
                          bool &show_t, Vec3d &t_img, Vec3d &t,
                          bool &show_S, Vec3d &Sxyz);
    
    void getVisualizerValues(Vec3d &t, Matrix44d &x, Matrix44d &z, Matrix44d &Tref2robot,
                                              Matrix44d &Ttrans2robot, s32 &transducerType, Cubed &frameBoundaries, Matrix44d &Tem2robot);


  private:
    void GetPoseEstimate(Matrix44d &x);
    bool CheckCompletion();

  private:
    Vec3d m_t;
    Matrix44d m_x;
    Matrix44d m_z;
    Matrix44d m_usCalibrationMatrix;
    Matrix44d m_Tref2robot;
    Matrix44d m_Tem2robot;
    Cubed m_frameBoundaries;
    s32 m_transducerType;

    //Matrix44d m_measurement;
    Matrix44d m_Ttrans2robot;
    bool m_inManualScanning;
    bool m_inTaskSpaceControl;
    bool m_inJointSpaceControl;
    InPlaneSegmentation m_segmentation; 
    UnscentedKalmanFilter m_UKF;
    NeedleSteeringRobot* m_robot;
    double m_lastInsMM;
    double m_lastRollDeg;
    RPData m_data;    
    STrigger* m_insTrigger;
    STrigger* m_rotTrigger;
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

  class STrigger
  {
  public:
    STrigger();
    ~STrigger();
    float update(float input);
    void setThresholds(float t);

  protected:
    float m_value;
    float m_threshold;
    int m_count;
  };
}

