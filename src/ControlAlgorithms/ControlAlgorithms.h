/// class:		ControlAlgorithms
/// brief:		Encapsulates the needle steering control and estimation algorithms.
///
///	author:		Troy Adebar
///	date:		  April 27, 2015

#pragma once

//#define RECORDING_MEASUREMENT_NOISE

#include <time.h>
#include "RTCore.h"
#include "RPFileReader.h"
#include "SegmentationAlgorithms.h"
#include "UnscentedKalmanFilter.h"
#include "NeedleSteeringRobot.h"
#include <QtGui/QFileDialog>
#include <cmath>
#include <boost/chrono/chrono.hpp>
#include <boost/thread/thread.hpp>

#include <iostream>
#include <fstream>
#include <windows.h>


#define   RHO                  33.0    // radius of curvature for 12-mm needle in liver (mm)
#define   RHO_MAX                 75.0    // radius of curvature for 12-mm needle in liver (mm)
#define   INS_AUTO_SPEED          0.5     // insertions speed during task-space teleoperation  (mm/s)
#define   INS_SPEED               20.0    // insertion speed during joint-space teleoperation (mm/s) 
#define   ROT_SPEED               200.0   // rotation speed during joint-space teleoperation (RPM)
#define   NEEDLE_DEAD_LENGTH      146.0   // offset of needle tip at zero insertion due to extra needle length 
#define   MAX_OPEN_LOOP_INSERTION 15.0    // maximum open-loop insertion distance before a new scan is needed (mm)
#define   PI                      3.14159265359
#define   NEEDLE_GPS_OFFSET       0.0    // x-axis distance from GPS transducer to needle "tip" point (mm)
#define   INTRODUCER_LENGTH       5.0    // insertion length where the tip is inside the introducer (mm)

DWORD WINAPI ControlThread( LPVOID lpParam );


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
    bool startStopRecordingData();
    bool inTaskSpaceControl();
    bool inJointSpaceControl();
    bool inManualScanning();
    void controlHeartBeat(RPData);
    void onUpdateOverlay();
    void SetTarget(Vec2d t_im);
    void ManualNeedleTipSelection(Vec2d p_im);
    void GetTarget(Vec3d &t);
    bool isTargetDefined();
    void resetManualScan();
    void processManualScan();
    void setRobot(NeedleSteeringRobot* robot);
    void GetIncrementalInputVector(Vec3d &u);
    void ControlCorrection();
    Vec3d ImagePtToRobotPt(Vec2d p_im);
    Vec3d RobotPtToImagePt(Vec3d p_robot);
    void setCalibration(Matrix44d, Matrix44d, s32);
    void updateTransducerPose();
    void setJointSpaceControlVelocities(f32 v_rot, f32 v_ins);
    void initializeEstimator();
    void RotateInline();

    void getOverlayValues(Matrix44d &x, Vec3d &p_img, Vec3d &pz_img, Vec3d &py_img, Matrix44d &z, Vec3d &Sxyz, Vec3d &t_img, Vec3d &t, double &mmToNextScan, bool &targetDepthReached,double& alpha_e);
    
    void getVisualizerValues(Vec3d &t, Matrix44d &x, Matrix44d &z, Matrix44d &Tref2robot,
                                              Matrix44d &Ttrans2robot, s32 &transducerType, Cubed &frameBoundaries, Matrix44d &Tem2robot, Matrix44d &Tneedletip2robot);

  private:
    
    bool CheckCompletion();
    double insertionSinceLastManualScan();
    void recordDataPoint(Matrix44d x_est, Matrix44d x_act, Matrix44d z, Vec3d t, Vec3d u, Matrix66d K, Matrix66d P); 

  private:
    
    Matrix44d m_z;
    Matrix44d m_usCalibrationMatrix;
    Matrix44d m_Tref2robot;
    Matrix44d m_Tem2robot;
    Matrix44d m_Tneedletip2robot;
    Cubed m_frameBoundaries;
    s32 m_transducerType;
    Matrix44d m_Ttrans2robot;
    bool m_inManualScanning;
    bool m_inTaskSpaceControl;
    bool m_inJointSpaceControl;
    bool m_recordingData;
    double m_insertionMMatLastManualScan;
    double ArticulationAngle;
    InPlaneSegmentation m_segmentation; 
    UnscentedKalmanFilter m_UKF;
    
    double m_lastInsMM;
    double m_lastRollDeg;
    
    RPData m_data;    
    STrigger* m_insTrigger;
    STrigger* m_rotTrigger;
    arma::mat m_xest_record;
    arma::mat m_xact_record;
    arma::mat m_z_record;
    arma::mat m_t_record;
    arma::mat m_u_record;
    arma::mat m_K_record;
    arma::mat m_P_record;
    
	
	DWORD   dwThreadIdArray;
	HANDLE  hThreadArray;
  std::ofstream myfile;
  public:
    bool QuickandDirty;
    Vec3d m_t;
    Matrix44d m_x;
   NeedleSteeringRobot* m_robot;
   f64 m_maxArticulationAngle;
   void GetPoseEstimate(Matrix44d &x);
   double m_l;
   double m_th;
#ifdef RECORDING_MEASUREMENT_NOISE
    int m_scan;
    int m_step;
#endif
  };

  class StylusCalibration
  {
  public:
    StylusCalibration();
    ~StylusCalibration();
    void addMeasurement(arma::mat, arma::mat);
    void clearMeasurements(void);
    f32 solveCalibration(void);
    arma::mat getCalibrationVector(void);
    arma::mat getCenter(void);
    bool isComplete(void);
    void SetOffset(arma::mat c);

  protected:
    arma::mat m_A;
    arma::mat m_b;
    arma::mat m_vcal;
    arma::mat m_c;
    f32 m_e;
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

