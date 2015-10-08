#include "ControlAlgorithms.h"
#include "math.h"
#include <time.h>



namespace Nf {

  ControlAlgorithms::ControlAlgorithms():    
      m_inTaskSpaceControl(false)
    , m_inJointSpaceControl(false)
    , m_inManualScanning(false)
    , m_recordingData(false)
    , m_t(0.0, 0.0, 0.0)
    , m_x(Matrix44d::Zero())
    , m_z(Matrix44d::Zero())
    , m_usCalibrationMatrix(Matrix44d::Zero())
    , m_Tref2robot(Matrix44d::Zero())
    , m_Tem2robot(Matrix44d::Zero())
    , m_Tneedletip2robot(Matrix44d::Zero())
    , m_transducerType(0)
    , m_robot(NULL)
    , m_insertionMMatLastManualScan(0.0)
  {

#ifdef RECORDING_MEASUREMENT_NOISE
    m_scan = 0;
    m_step = 0;
#endif
    m_insTrigger = new STrigger();
    m_rotTrigger = new STrigger();
    m_insTrigger->setThresholds(0.01);
    m_rotTrigger->setThresholds(0.01);
  }

  ControlAlgorithms::~ControlAlgorithms()
  {
    delete m_insTrigger;
    delete m_rotTrigger;
  }

  // return true if the robot is in following; false otherwise
  bool ControlAlgorithms::inTaskSpaceControl()
  {
    return m_inTaskSpaceControl;
  }

  // return true if the robot is in following; false otherwise
  bool ControlAlgorithms::inJointSpaceControl()
  {
    return m_inJointSpaceControl;
  }

  // return true if a manual scan is underway; false otherwise
  bool ControlAlgorithms::inManualScanning()
  {
    return m_inManualScanning;
  }

  // toggle the task-space control state
  bool ControlAlgorithms::startStopTaskSpaceControl()
  {
    if(m_UKF.isInitialized()){ //don't change the state if UKF is not initialized
      m_inTaskSpaceControl = !m_inTaskSpaceControl;
    }

    if(m_inTaskSpaceControl){ // if we've just started teleoperation
      m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
    }else{
      m_robot->SetInsertionVelocity(0.0);
      m_robot->SetRotationVelocity(0.0);
    }

    return m_inTaskSpaceControl;
  }

  // toggle recording data
  bool ControlAlgorithms::startStopRecordingData()
  {
    using namespace arma;

    if(m_UKF.isInitialized()){ //don't change the state if UKF is not initialized
      if( m_recordingData ){ // if we have been recording, save the data
        m_recordingData = false;
        char strbuff[100];
        m_xest_record.save("C:/Troy/Data/xest.m", arma_ascii);
        m_xact_record.save("C:/Troy/Data/xact.m", arma_ascii);
        m_z_record.save("C:/Troy/Data/z.m", arma_ascii);
        m_t_record.save("C:/Troy/Data/t.m", arma_ascii);
        m_u_record.save("C:/Troy/Data/u.m", arma_ascii);
      }else{ 
        m_xest_record.reset();
        m_xact_record.reset();
        m_z_record.reset();
        m_t_record.reset();
        m_u_record.reset();
        m_recordingData = !m_recordingData;
      }
      return m_recordingData;
    }
  }

  // initialize the Unscented Kalman Filter based on joint values
  void ControlAlgorithms::initializeEstimator()
  {
    m_UKF.initialize(m_robot->getInsMM()+NEEDLE_DEAD_LENGTH, m_robot->getRollAngle());
    m_x = m_UKF.getCurrentEstimate();
  }

  // toggle the joint-space control state
  bool ControlAlgorithms::startStopJointSpaceControl()
  {
    if(m_UKF.isInitialized()){ //don't change the state if UKF is not initialized
      m_inJointSpaceControl = !m_inJointSpaceControl;
    }

    if(!m_inJointSpaceControl){
      m_robot->SetInsertionVelocity(0.0);
      m_robot->SetRotationVelocity(0.0);
    }

    return m_inJointSpaceControl;
  }

  // set the robot velocity based on input from 3D mouse
  void ControlAlgorithms::setJointSpaceControlVelocities(f32 v_rot, f32 v_ins)
  {
    if( m_inJointSpaceControl ){
      NTrace("v_ins = %.5f, v_rtot =%.5f\n",v_ins,v_rot);
      m_robot->SetInsertionVelocity(m_insTrigger->update(v_ins)*INS_SPEED);
      m_robot->SetRotationVelocity(m_rotTrigger->update(v_rot)*ROT_SPEED);
    }
  }

  void ControlAlgorithms::startStopManualScanning(bool start)
  {
    if( start ){
        resetManualScan();
        m_inManualScanning = true;
    }else{
      m_inManualScanning = false;
      processManualScan();
      m_insertionMMatLastManualScan = m_robot->getInsMM(); 
      if( m_inTaskSpaceControl ){        
        m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
      }
    }
  }  

  void ControlAlgorithms::setCalibration(Matrix44d Tref2robot, Matrix44d usCal, s32 transducerType)
  {
    m_Tref2robot = Tref2robot;
    m_usCalibrationMatrix = usCal;
    m_segmentation.setCalibration(Tref2robot,usCal);
    m_transducerType =  transducerType;
  }

  void ControlAlgorithms::SetTarget(Vec2d t_im)
  {
    m_t = ImagePtToRobotPt(t_im);
  }

  void ControlAlgorithms::ManualNeedleTipSelection(Vec2d p_im)
  {
    Vec3d p = ImagePtToRobotPt(p_im);
    Matrix33d R = m_x.GetOrientation();
    m_z = Matrix44d::FromOrientationAndTranslation(R,p);
    Vec3d u;
    GetIncrementalInputVector(u);
    m_UKF.fullUpdateUKF(u, m_z);
    m_UKF.getCurrentStateEstimate(m_x);
    recordDataPoint(m_x, m_Tneedletip2robot, m_z, m_t, u);

#ifdef RECORDING_MEASUREMENT_NOISE
    arma::mat z = m_z.ToArmaMatrix4x4();
    char strbuff[100];
    sprintf(strbuff, "C:/Troy/Data/step_%d_measurement_%d.m",m_step,m_scan);
    z.save(strbuff, arma::arma_ascii);
    m_scan++;
    if( m_scan == 5 ){
      m_scan = 0;
      m_step++;
      m_insertionMMatLastManualScan = m_robot->getInsMM(); 
      if( m_inTaskSpaceControl ){        
        m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
      }   
    }
#else
    m_insertionMMatLastManualScan = m_robot->getInsMM(); 
    if( m_inTaskSpaceControl ){        
      m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
    }   
#endif
  }

  Vec3d ControlAlgorithms::ImagePtToRobotPt(Vec2d p_im)
  {
    if(m_data.gps.valid && !m_usCalibrationMatrix.isZero() && !m_Tref2robot.isZero())
    { 
      Matrix44d Ttrans2em = Matrix44d::FromCvMat(m_data.gps.pose);
      Ttrans2em.SetPosition(m_data.gps.pos);
      Matrix44d Tref2em = Matrix44d::FromCvMat(m_data.gps2.pose);
      Tref2em.SetPosition(m_data.gps2.pos);
      Vec2d scale(m_data.mpp.x/1000.0, m_data.mpp.y/1000.0);
//#ifdef GPS3_SAVING
//      Vec3d ImPtInGPSFrame = rpImageCoordToWorldCoord3(p_im, Ttrans2em, m_usCalibrationMatrix, m_data.origin, scale);
//      Matrix44d Tneedletip2em = Matrix44d::FromCvMat(m_data.gps3.pose);
//      Vec3d tipOffset(NEEDLE_GPS_OFFSET,0.0,0.0);
//      Vec3d GPS2Pt = m_data.gps3.pos + Tneedletip2em.GetOrientation()*tipOffset;
//      int i = 1;
//#endif
      return rpImageCoordToRobotCoord3(p_im, Ttrans2em, Tref2em, m_Tref2robot, m_usCalibrationMatrix, m_data.origin, scale);
    }else{
      return Vec3d(0.0, 0.0, 0.0);
    }
  }

  Vec3d ControlAlgorithms::RobotPtToImagePt(Vec3d p_robot)
  {
    if(m_data.gps.valid && !m_usCalibrationMatrix.isZero() && !m_Tref2robot.isZero())
    { 
      Matrix44d Ttrans2em = Matrix44d::FromCvMat(m_data.gps.pose);
      Ttrans2em.SetPosition(m_data.gps.pos);
      Matrix44d Tref2em = Matrix44d::FromCvMat(m_data.gps2.pose);
      Tref2em.SetPosition(m_data.gps2.pos);
      Vec2d scale(m_data.mpp.x/1000.0, m_data.mpp.y/1000.0);
      return rpRobotCoord3ToImageCoord(p_robot, Ttrans2em, Tref2em, m_Tref2robot.Inverse(), m_usCalibrationMatrix, m_data.origin, scale);
    }else{
      return Vec3d(0.0, 0.0, 0.0);
    }
  }

  void ControlAlgorithms::controlHeartBeat(RPData data)
  {
    if(m_data.gps.valid)
    {
      m_data.Release();
    }
    m_data = data.Clone();
    updateTransducerPose();

    if( inManualScanning() ) // if manually scanning
    {
      m_segmentation.addManualScanFrame(data);
    }

    GetPoseEstimate(m_x); // update the estimate 

    if( m_inTaskSpaceControl ) 
    {
      ControlCorrection();

      if( CheckCompletion() ){  // if we've reached the target
        m_robot->SetInsertionVelocity(0.0);
        m_robot->SetRotationVelocity(0.0);
      }

      if( insertionSinceLastManualScan() >= MAX_OPEN_LOOP_INSERTION ){ // if we need a new scan
        m_robot->SetInsertionVelocity(0.0);
        m_robot->SetRotationVelocity(0.0);
      }
    }
  }

  void ControlAlgorithms::GetPoseEstimate(Matrix44d &x)
  {
    Vec3d u;
    GetIncrementalInputVector(u);

    if( abs(u.x) > 1e-6 || abs(u.z) > 1e-6){
      m_UKF.processUpdateUKF(u);
      m_UKF.getCurrentStateEstimate(x);
      recordDataPoint(x, m_Tneedletip2robot, Matrix44d::Zero(), m_t, u);
    }
  }

  void ControlAlgorithms::updateTransducerPose()
  {
    if(m_data.gps.valid)
    {
      Matrix44d Ttrans2em = Matrix44d::FromCvMat(m_data.gps.pose);
      Ttrans2em.SetPosition(m_data.gps.pos);
      Matrix44d Tref2em = Matrix44d::FromCvMat(m_data.gps2.pose);
      Tref2em.SetPosition(m_data.gps2.pos);

      m_Ttrans2robot = m_Tref2robot*Tref2em.Inverse()*Ttrans2em;
      m_Tem2robot = m_Ttrans2robot*Ttrans2em.Inverse();

#ifdef GPS3_SAVING
      Matrix44d Tneedletip2em = Matrix44d::FromCvMat(m_data.gps3.pose);
      Vec3d tipOffset(NEEDLE_GPS_OFFSET,0.0,0.0);
      //Vec3d tipOffset(0.0,0.0,0.0);
      Tneedletip2em.SetPosition(m_data.gps3.pos + Tneedletip2em.GetOrientation()*tipOffset);
      m_Tneedletip2robot = m_Tem2robot*Tneedletip2em;
#endif

      m_frameBoundaries = m_data.GetFrameBoundariesInRobotFrame(m_usCalibrationMatrix, m_Tref2robot);
    }
  }

  void ControlAlgorithms::recordDataPoint(Matrix44d x_est, Matrix44d x_act, Matrix44d z, Vec3d t, Vec3d u)
  {
    m_xest_record.insert_cols(m_xest_record.n_cols,x_est.ToArmaMatrix4x4());
    m_xact_record.insert_cols(m_xact_record.n_cols,x_act.ToArmaMatrix4x4());
    m_z_record.insert_cols(m_z_record.n_cols,z.ToArmaMatrix4x4());

    arma::vec3 t_; 
    t_ << t.x << t.y << t.z;
    arma::vec3 u_; 
    u_ << u.x << u.y << u.z;
    
    m_t_record.insert_cols(m_t_record.n_cols,t_);
    m_u_record.insert_cols(m_u_record.n_cols,u_);
  }

  bool ControlAlgorithms::CheckCompletion(void)
  {
    Vec3d p = m_x.GetPosition();
    if( p.z > m_t.z )
    {
      return true;
    }else{
      return false;
    }
  }

  void ControlAlgorithms::GetTarget(Vec3d &t)
  {
    t = m_t;
  } 

  void ControlAlgorithms::GetIncrementalInputVector(Vec3d &u)
  {
    if( m_robot && m_robot->isRollInitialized() && m_robot->isInsertionInitialized() ){
      double currentInsMM = m_robot->getInsMM();
      double l = currentInsMM - m_lastInsMM;
      
      double currentRollDeg = m_robot->getRollAngle();
      double d_th = (currentRollDeg - m_lastRollDeg)*PI/180.0;   

      if( currentInsMM > INTRODUCER_LENGTH ){
        u = Vec3d(d_th,RHO,l);
      }else{
        u = Vec3d(d_th,1000,l);
      }

      m_lastInsMM = currentInsMM;
      m_lastRollDeg = currentRollDeg;
    }else{
      u = Vec3d(0.0,RHO,0.0);
    }
  }

  bool ControlAlgorithms::isTargetDefined()
  {
    return !m_t.isZero();
  } 

  void ControlAlgorithms::resetManualScan()
  {
    m_segmentation.resetManualScan();
  }

  void ControlAlgorithms::processManualScan()
  {
    Vec3d u;
    GetIncrementalInputVector(u);
    m_z = m_segmentation.processManualScan();
    m_UKF.fullUpdateUKF(u, m_z);
    m_UKF.getCurrentStateEstimate(m_x);
  }

  void ControlAlgorithms::setRobot(NeedleSteeringRobot* robot)
  {
    m_robot = robot;
    m_lastInsMM = m_robot->getInsMM();
    m_lastRollDeg = m_robot->getRollAngle();
  }


  void ControlAlgorithms::ControlCorrection()
  {
    if(m_t.magnitude() > 1e-3) // if the target is defined
    {
      // Get current tip frame estimate
      Vec3d p = m_x.GetPosition();
      Matrix33d R = m_x.GetOrientation();

      // Get relative error in the current tip frame
      Vec3d e = R.Inverse()*(m_t-p);
      f32 d_th = atan2(-e.x,e.y);

      // Check if needle tip is past target
      if( e.z < 0 )
        int i = 1;// Do end of steering tasks

      // Correct needle rotation
      m_robot->RotateIncremental(d_th*180.0/PI);

    }else{
      NTrace("No target defined; skipping correction\n");
    }
  }

  void ControlAlgorithms::getOverlayValues(Matrix44d &x, Vec3d &p_img, Vec3d &pz_img, Vec3d &py_img,
                                           Matrix44d &z,
                                           Vec3d &Sxyz,
                                           Vec3d &t_img, Vec3d &t,
                                           double &mmToNextScan, bool &targetDepthReached)
  {
    // target  
    t_img = RobotPtToImagePt(m_t);
    t = m_t;
    // estimate and variance
    x = m_x;
    Sxyz = m_UKF.getCurrentXYZVariance();
    // measurement
    z = m_z;
    // tip frame in image coordinates
    Vec3d p = m_x.GetPosition();
    Matrix33d R = m_x.GetOrientation();
    p_img = RobotPtToImagePt(p);
    Vec3d pz_world = p + R*Vec3d(0.0,0.0,10.0);
    Vec3d py_world = p + R*Vec3d(0.0,5.0,0.0);
    pz_img = RobotPtToImagePt(pz_world);
    py_img = RobotPtToImagePt(py_world);
    mmToNextScan = max(MAX_OPEN_LOOP_INSERTION-insertionSinceLastManualScan(),0.0);
    targetDepthReached = CheckCompletion();

#ifdef GPS3_SAVING
    Sxyz = m_Tneedletip2robot.GetPosition(); 
#endif
  }

  void ControlAlgorithms::getVisualizerValues(Vec3d &t, Matrix44d &x, Matrix44d &z, Matrix44d &Tref2robot,
    Matrix44d &Ttrans2robot, s32 &transducerType, Cubed &frameBoundaries, Matrix44d &Tem2robot, Matrix44d &Tneedletip2robot)
  {
    t = m_t;
    x = m_x;
    z = m_z;
    Tref2robot = m_Tref2robot;
    Ttrans2robot = m_Ttrans2robot;
    frameBoundaries = m_frameBoundaries;
    Tem2robot = m_Tem2robot;
    Tneedletip2robot = m_Tneedletip2robot;
    transducerType = m_transducerType;
  }

  double ControlAlgorithms::insertionSinceLastManualScan()
  {
    if( m_robot && m_robot->isInsertionInitialized() ){
      double currentInsMM = m_robot->getInsMM();
      return currentInsMM - m_insertionMMatLastManualScan;
    }else{
      return 0.0;
    }
  }

  /// ----------------------------------------------------
  /// Stylus Calibration Class
  /// ----------------------------------------------------

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

  f32 StylusCalibration::solveCalibration(void)
  {
    using namespace arma;
    arma::mat x = arma::solve(m_A,m_b);
    m_vcal = x.rows(0,2);
    m_c = x.rows(3,5);
    m_stylusCalibrationComplete = true;

    arma::mat error = m_A*x - m_b;
    m_e = 0.0;
    int Npts = m_A.n_rows/3;
    for( int i = 0; i < Npts; i+=3 ){
      m_e += norm(error.rows(i,i+2),2)/double(Npts);
    }    
    return m_e;
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
  
  /// ----------------------------------------------------
  /// STrigger Class
  /// ----------------------------------

  STrigger::STrigger()
  {
    m_value = 0.0;
    m_threshold = 0.0;
    m_count = 0;
  }

  STrigger::~STrigger()
  {
  }
  
  float STrigger::update(float input)
  {
    float correctedInput;
    if( fabs(input) > m_threshold){
      correctedInput = 1.0;
      if( input < 0 )
        correctedInput = -1.0;
    }
    else{
      correctedInput = 0.0;
    }

    if( correctedInput == m_value )
      m_count = 0;
    else 
      m_count++;

    if( m_count > 3 ){
      m_value = correctedInput;
      m_count = 0;
    }

    return m_value;
  }
    
  void STrigger::setThresholds(float t)
  {
    m_threshold= t;
  }
};