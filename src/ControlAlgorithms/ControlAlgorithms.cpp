#include "ControlAlgorithms.h"
#include "math.h"
#include <time.h>

namespace Nf {

#define TIP_LENGTH 15.3*0
#define TIP_ANGLE 0

  static Vec3d tipToWrist(const Matrix33d &R, const Vec3d &tip)
  {
    return tip-(R.Col(2)*cos((f64)TIP_ANGLE)+R.Col(1)*sin((f64)TIP_ANGLE))*TIP_LENGTH;
  }

  static Vec3d wristToTip(const Matrix33d &R, const Vec3d &wrist)
  {
    return wrist+(R.Col(2)*cos((f64)TIP_ANGLE)+R.Col(1)*sin((f64)TIP_ANGLE))*TIP_LENGTH;
  }

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
    , m_th(0.0)
    , m_l(0.0)
    , m_maxArticulationAngle(0.0)
    , m_calibratingPlaneOffest(false)
  {

#ifdef RECORDING_MEASUREMENT_NOISE
    m_scan = 0;
    m_step = 0;
#endif
    m_insTrigger = new STrigger();
    m_rotTrigger = new STrigger();
    m_insTrigger->setThresholds(0.01);
    m_rotTrigger->setThresholds(0.01);
	  QuickandDirty=false;
    myfile.open("timeUS.txt");
    ArticulationAngle=0;
    NPoint=0;

    m_planeCalibrator = std::tr1::shared_ptr < PlaneCalibrator > (new PlaneCalibrator());
  }

  ControlAlgorithms::~ControlAlgorithms()
  {
    delete m_insTrigger;
    delete m_rotTrigger;
	  CloseHandle(hThreadArray);
    myfile.close();

  }

  void ControlAlgorithms::StartControlThread()
  {
    // Create the thread to begin execution on its own.

    hThreadArray = CreateThread( 
      NULL,                   // default security attributes
      0,                      // use default stack size  
      ControlThread,         // thread function name
      this,                  // argument to thread function 
      0,                      // use default creation flags 
      &dwThreadIdArray);   // returns the thread identifier 
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
    if(m_EST.isInitialized()){ //don't change the state if EST is not initialized
      m_inTaskSpaceControl = !m_inTaskSpaceControl;
    }

    if(m_inTaskSpaceControl){ // if we've just started teleoperation
      m_maxArticulationAngle = m_robot->getArticulationAngle();
      //m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
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

    if(m_EST.isInitialized()){ //don't change the state if UKF is not initialized
      if( m_recordingData ){ // if we have been recording, save the data
        m_recordingData = false;
        char strbuff[100];
        m_xest_record.save("C:/Troy/Data/xest.m", arma_ascii);
        m_xact_record.save("C:/Troy/Data/xact.m", arma_ascii);
        m_z_record.save("C:/Troy/Data/z.m", arma_ascii);
        m_t_record.save("C:/Troy/Data/t.m", arma_ascii);
        m_u_record.save("C:/Troy/Data/u.m", arma_ascii);
        m_K_record.save("C:/Troy/Data/K.m", arma_ascii);
        m_P_record.save("C:/Troy/Data/P.m", arma_ascii);
      }else{ 
        m_xest_record.reset();
        m_xact_record.reset();
        m_z_record.reset();
        m_t_record.reset();
        m_u_record.reset();
        m_K_record.reset();
        m_P_record.reset();
        m_recordingData = !m_recordingData;
      }
      return m_recordingData;
    }
  }

  // initialize the Unscented Kalman Filter based on joint values
  void ControlAlgorithms::initializeEstimator()
  {
    m_EST.setEstimator(m_l,NEEDLE_DEAD_LENGTH, m_robot->getRollAngle());
    m_x = m_EST.getCurrentEstimate();
  }

  // toggle the joint-space control state
  bool ControlAlgorithms::startStopJointSpaceControl()
  {
    if(m_EST.isInitialized()){ //don't change the state if UKF is not initialized
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
        //m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
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
    if(!m_calibratingPlaneOffest) {

      // show distance point, if the user confirm, add point
      m_EST.addPOINT(p);
      
      /*Matrix33d R = m_x.GetOrientation();
      // Track of the Wrist
      Vec3d ux = R.Col(0);
      Vec3d uz = R.Col(2);
      Vec3d v,W;
      double a=-45*(PI/180);
      if( m_robot->getArticulationAngle() < 5)
        a=0;
      //Matrix33d Rx=Matrix33d(cos(a)+(ux.x)*(ux.x)*(1-cos(a)), (ux.y)*(ux.x)*(1-cos(a))-(ux.z)*sin(a), (ux.z)*(ux.x)*(1-cos(a))+(ux.y)*sin(a), (ux.y)*(ux.x)*(1-cos(a))+(ux.z)*sin(a), cos(a)+(ux.y)*(ux.y)*(1-cos(a)), (ux.z)*(ux.y)*(1-cos(a))-(ux.x)*sin(a), (ux.z)*(ux.x)*(1-cos(a))-(ux.y)*sin(a), (ux.z)*(ux.y)*(1-cos(a))+(ux.x)*sin(a), cos(a)+(ux.z)*(ux.z)*(1-cos(a)));

      // check transpose?
      v=Rx*uz;
      //W=p-v*15.3;  //TIP CHANGE 123
      W = tipToWrist(R, p);/*
      m_z = Matrix44d::FromOrientationAndTranslation(R,W);
      Vec3d u;
      GetIncrementalInputVector(u);
      m_UKF.fullUpdateUKF(u, m_z);
      Matrix66d P, K;
      m_UKF.getCurrentStateEstimate(m_x);
      P  = m_UKF.getCurrentCovariance();
      K = m_UKF.getCurrentGain();
      */
      //recordDataPoint(m_x, m_Tneedletip2robot, m_z, m_t, u, K, P);

#ifdef RECORDING_MEASUREMENT_NOISE
      arma::mat z = m_z.ToArmaMatrix4x4();
      char strbuff[100];
      sprintf(strbuff, "C:/Troy/Data/step_%d_measurement_%d.m",m_step,m_scan);
      z.save(strbuff, arma::arma_ascii);
      m_scan++;
      if( m_scan == 5 ){
        m_scan = 0;
        m_step++;
        m_insertionMMatLastManualScan = m_robot->getInsMM2(); 
        if( m_inTaskSpaceControl ){        
          m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
        }   
      }
#else
      NPoint++;
      if (NPoint < 3)
      {
          
      }
      else
      {
          m_insertionMMatLastManualScan = m_l;
          m_EST.saveDataOpt();
          m_EST.WaitAndCorrect();
          NPoint=0;
      }
      
      if( m_inTaskSpaceControl ){        
        //m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
      }   
#endif
    } else {
      m_planeCalibrator->AddPoint(p);
    }
  }

  void ControlAlgorithms::DoPlaneCalibration()
  {
    m_planeCalibrator->DoCalibration();

    Plane pp = m_planeCalibrator->GetSolution();
    Vec3d normal = pp.GetNormalVector();

    Matrix33d R = m_EST.getCurrentEstimate().GetOrientation();
    Vec3d estimatedNormal = R.Col(0);
    f64 angle = abs(acos(estimatedNormal.dot(normal)/(estimatedNormal.magnitude()*normal.magnitude())));

    angle = fmod(angle+2*PI, PI);
    angle = MIN(angle, PI-angle);  // angle \in [0, pi/2)

    if(normal.cross(estimatedNormal).dot(R.Col(2)) > 0)
      angle = -angle;

    m_robot->RotateIncremental(angle*180.0/PI);
    Sleep(2000);
    // Now we need to not have the estimator "know" that this happened
    m_th = m_robot->getRollAngle();
    m_lastRollDeg = m_th;
  }

  std::vector < Vec3d > ControlAlgorithms::GetPlanarCalibrationPoints()
  {
    std::vector < Vec3d > res;
    arma::mat points = m_planeCalibrator->GetPoints();
    for(s32 i=0; i<points.n_rows; i++) {
      res.push_back(Vec3d(points(i, 0), points(i,1), points(i,2)));
    }
    return res;
  }


  void ControlAlgorithms::GetPlaneCalibration(Vec3d &corner1, Vec3d &axis1, Vec3d &axis2, Vec3d &corner2, Vec3d &axis3, Vec3d &axis4)
  {
    arma::mat points = m_planeCalibrator->GetPoints();
    if(points.n_rows > 3) {
      m_planeCalibrator->DoCalibration();
      Plane pp = m_planeCalibrator->GetSolution();

      arma::mat centerPoint = arma::mean(points, 0);
      Vec3d center = Vec3d(centerPoint(0,0), centerPoint(0,1), centerPoint(0,2));

      Matrix33d R = m_EST.getCurrentEstimate().GetOrientation();
      axis1 = pp.ProjectPointOntoPlane(center+R.Col(1)*100.0)-center;
      axis2 = pp.ProjectPointOntoPlane(center+R.Col(2)*100.0)-center;
      corner1 = center-axis1-axis2;

      axis3 = R.Col(1)*100.0;
      axis4 = R.Col(2)*100.0;
      corner2 = center-axis3-axis4;
      axis1 = axis1*2;
      axis2 = axis2*2;
      axis3 = axis3*2;
      axis4 = axis4*2;
    }
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

    SYSTEMTIME time;
	  GetSystemTime(&time);
	  LONG time_ms = (time.wSecond * 1000) + time.wMilliseconds;
	  myfile<<time_ms<<endl;

    if(m_data.gps.valid)
    {
      m_data.Release();
    }
    m_data = data.Clone();
    updateTransducerPose();

    if (!QuickandDirty)
    {	
      //GetPoseEstimate(m_x);
      if( m_robot && m_robot->isRollInitialized() && m_robot->isInsertionInitialized() ){
        m_th = m_robot->getRollAngle();
        m_l = m_robot->getInsMM();
            
      }
      
    }
    

    

    if( inManualScanning() ) // if manually scanning
    { 
      m_segmentation.addManualScanFrame(data);
    }

     // update the estimate

    if( m_inTaskSpaceControl ) 
    {
    	bool condition=false; 
      //ControlCorrection()
     //if( CheckCompletion() )
     //{  // if we've reached the target
    //	 				m_robot->SetInsertionVelocity(0.0);
    //	 				m_robot->SetRotationVelocity(0.0);
    //	 				QuickandDirty=false;
    //          condition=true;
     //}
     if( insertionSinceLastManualScan() >= MAX_OPEN_LOOP_INSERTION )
     { // if we need a new scan
    
    	    QuickandDirty=false;
    	    m_robot->SetInsertionVelocity(0.0);
          m_robot->SetRotationVelocity(0.0);
        condition=true;
     }
      if ((!QuickandDirty)&&(!condition))
    		 QuickandDirty=true;
    }
  }

  /*void ControlAlgorithms::GetPoseEstimate(Matrix44d &x)
  {
    Vec3d u;
    GetIncrementalInputVector(u);

    if( abs(u.x) > 1e-6 || abs(u.z) > 1e-6){
      m_UKF.processUpdateUKF(u);
      m_UKF.getCurrentStateEstimate(x);
      Matrix66d P = m_UKF.getCurrentCovariance();
      recordDataPoint(x, m_Tneedletip2robot, Matrix44d::Zero(), m_t, u, Matrix66d::Zero(), P);
    }
  }*/

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

  void ControlAlgorithms::recordDataPoint(Matrix44d x_est, Matrix44d x_act, Matrix44d z, Vec3d t, Vec3d u, Matrix66d K, Matrix66d P)
  {
    m_xest_record.insert_cols(m_xest_record.n_cols,x_est.ToArmaMatrix4x4());
    m_xact_record.insert_cols(m_xact_record.n_cols,x_act.ToArmaMatrix4x4());
    m_z_record.insert_cols(m_z_record.n_cols,z.ToArmaMatrix4x4());
    m_K_record.insert_cols(m_K_record.n_cols,K.ToArmaMatrix6x6());
    m_P_record.insert_cols(m_P_record.n_cols,P.ToArmaMatrix6x6());

    arma::vec3 t_; 
    t_ << t.x << t.y << t.z;
    arma::vec3 u_; 
    u_ << u.x << u.y << u.z;

    m_t_record.insert_cols(m_t_record.n_cols,t_);
    m_u_record.insert_cols(m_u_record.n_cols,u_);
  }

  bool ControlAlgorithms::CheckCompletion(void)
  {
    Vec3d W = m_x.GetPosition();
    Matrix33d R = m_x.GetOrientation();

    // Track of the Wrist
    Vec3d ux = R.Col(0);
    Vec3d uz = R.Col(2);
    
    Vec3d v,p;
    double a=0;
    if( ArticulationAngle > 5)
      a=-45*(PI/180);    

    //Matrix33d Rx=Matrix33d(cos(a)+(ux.x)*(ux.x)*(1-cos(a)), (ux.y)*(ux.x)*(1-cos(a))-(ux.z)*sin(a), (ux.z)*(ux.x)*(1-cos(a))+(ux.y)*sin(a), (ux.y)*(ux.x)*(1-cos(a))+(ux.z)*sin(a), cos(a)+(ux.y)*(ux.y)*(1-cos(a)), (ux.z)*(ux.y)*(1-cos(a))-(ux.x)*sin(a), (ux.z)*(ux.x)*(1-cos(a))-(ux.y)*sin(a), (ux.z)*(ux.y)*(1-cos(a))+(ux.x)*sin(a), cos(a)+(ux.z)*(ux.z)*(1-cos(a)));
    // check transpose?
    //v=Rx*uz;
    
    //p=W+uz*15.3; TIP CHANGE 123
    p = wristToTip(R, W);

    //Vec3d tipOffset = R.Transpose()*(m_t-p);
    if( R.Col(2).dot(m_t-p) < 0 )
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
    if( m_robot && m_robot->isRollInitialized() && m_robot->isInsertionInitialized() && m_robot->isArticulationInitialized())
    {
      double d_l = m_l - m_lastInsMM;
      double d_th = (m_th - m_lastRollDeg)*PI/180.0;   

      ArticulationAngle=m_robot->getArticulationAngle();
      if( m_l > INTRODUCER_LENGTH )
      {
        if(  ArticulationAngle < 5)
          u = Vec3d(d_th,RHO_MAX,d_l);        
        else
          u = Vec3d(d_th,RHO,d_l);
      }
      else
          u = Vec3d(d_th,1000,d_l);
      
      m_lastInsMM = m_l;
      m_lastRollDeg = m_th;
    }
    else
      u = Vec3d(0.0,RHO,0.0);
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

    // Track of the Wrist
    Matrix33d R = m_z.GetOrientation();
    Vec3d p = m_z.GetPosition();

    Vec3d ux = R.Col(0);
    Vec3d uz = R.Col(2);
 
    Vec3d v,W;
    double a=-45*(PI/180);
    if( m_robot->getArticulationAngle() < 5)
         a=0;
    Matrix33d Rx=Matrix33d(cos(a)+(ux.x)*(ux.x)*(1-cos(a)), (ux.y)*(ux.x)*(1-cos(a))-(ux.z)*sin(a), (ux.z)*(ux.x)*(1-cos(a))+(ux.y)*sin(a), (ux.y)*(ux.x)*(1-cos(a))+(ux.z)*sin(a), cos(a)+(ux.y)*(ux.y)*(1-cos(a)), (ux.z)*(ux.y)*(1-cos(a))-(ux.x)*sin(a), (ux.z)*(ux.x)*(1-cos(a))-(ux.y)*sin(a), (ux.z)*(ux.y)*(1-cos(a))+(ux.x)*sin(a), cos(a)+(ux.z)*(ux.z)*(1-cos(a)));
    // check transpose?
    //v=Rx*uz;
    //W=p-v*15.3; //TIP CHANGE 123
    W = tipToWrist(R,p);
    m_z = Matrix44d::FromOrientationAndTranslation(R,W);

    //m_UKF.fullUpdateUKF(u, m_z);
    //m_UKF.getCurrentStateEstimate(m_x);
  }

  void ControlAlgorithms::setRobot(NeedleSteeringRobot* robot)
  {
    m_robot = robot;
    m_l = m_robot->getInsMM();
    m_th = m_robot->getRollAngle();
    m_lastInsMM = m_l;
    m_lastRollDeg = m_th;
  }

  void ControlAlgorithms::RotateInline()
  {
    Vec3d p = m_x.GetPosition();
    Matrix33d R = m_x.GetOrientation();

    // Get relative error in the current tip frame
    Vec3d e = R.Inverse()*(m_t-p);

    f32 d_th = atan2(-e.x,e.y);
    m_robot->RotateIncremental(d_th*180.0/PI);
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

      Vec2d ePlane(e.x, e.y);
      bool targetInLine = ePlane.magnitude() < 2.5;

      f32 d_th = atan2(-e.x,e.y);

      // Check if needle tip is past target
      if( e.z < 0 )
        int i = 1;// Do end of steering tasks ??????
   
      //bool releaseArticulation = false;//(ABS(d_th) > 10*PI/180.0) && !targetInLine;
     // targetInLine = false;

      bool releaseArticulation = (ABS(d_th) > 10*PI/180.0) && !targetInLine;
      

      // try release articulation before turning 
      if(releaseArticulation) {
        m_robot->SetInsertionVelocity(0);
        m_robot->SetArticulationAngle(0);
        
       Sleep(3000);
      }

      // Correct needle rotation NEW
      m_robot->RotateIncremental(d_th*180.0/PI);

      if(releaseArticulation) {
        Sleep(3000);
        m_robot->SetArticulationAngle(m_maxArticulationAngle);
        Sleep(3000);
        m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
     } else if(targetInLine && ABS(m_robot->getArticulationAngle()) > 5) {
        m_robot->SetInsertionVelocity(0);
        m_robot->SetArticulationAngle(0);
        NTrace("Thinks target is inline\n");
        Sleep(3000);
        m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
        //NTrace("Articulation %f\n", m_robot->getArticulationAngle());
      } else if(!targetInLine) {
        m_robot->SetArticulationAngle(m_maxArticulationAngle);
      }

    }else{
      NTrace("No target defined; skipping correction\n");
    }
  }


  void ControlAlgorithms::getOverlayValues(Matrix44d &x, Vec3d &p_img, Vec3d &pz_img, Vec3d &py_img,
                                           Matrix44d &z,
                                           Vec3d &Sxyz,
                                           Vec3d &t_img, Vec3d &t,
                                           double &mmToNextScan, bool &targetDepthReached, double& alpha_e)
  {
    // target  
    t_img = RobotPtToImagePt(m_t);
    t = m_t;
      
    //Sxyz = m_UKF.getCurrentXYZVariance();
    // measurement
    z = m_z;
    // tip frame in image coordinates
    Vec3d W = m_x.GetPosition();
    Matrix33d R = m_x.GetOrientation();

    // Track of the Wrist
    Vec3d ux = R.Col(0);
    Vec3d uz = R.Col(2);
    
    Vec3d v,p;
    double a=0;
    if( ArticulationAngle > 5)
        a=-45*(PI/180);

    Matrix33d Rx=Matrix33d(cos(a)+(ux.x)*(ux.x)*(1-cos(a)), (ux.y)*(ux.x)*(1-cos(a))-(ux.z)*sin(a), (ux.z)*(ux.x)*(1-cos(a))+(ux.y)*sin(a), (ux.y)*(ux.x)*(1-cos(a))+(ux.z)*sin(a), cos(a)+(ux.y)*(ux.y)*(1-cos(a)), (ux.z)*(ux.y)*(1-cos(a))-(ux.x)*sin(a), (ux.z)*(ux.x)*(1-cos(a))-(ux.y)*sin(a), (ux.z)*(ux.y)*(1-cos(a))+(ux.x)*sin(a), cos(a)+(ux.z)*(ux.z)*(1-cos(a)));
    // check transpose?
    //v=Rx*uz;
    
    //p=W+v*15.3; TIP CHANGE 123
    p = wristToTip(R,W);
    x =  Matrix44d::FromOrientationAndTranslation(R,p);
    p_img = RobotPtToImagePt(p);
    Vec3d pz_world = p + R*Vec3d(0.0,0.0,0.1);
    Vec3d py_world = p + R*Vec3d(0.0,5.0,0.0);
    pz_img = RobotPtToImagePt(pz_world);
    py_img = RobotPtToImagePt(py_world);
    mmToNextScan = MAX(MAX_OPEN_LOOP_INSERTION-insertionSinceLastManualScan(),0.0);
    targetDepthReached = CheckCompletion();

    Nf::Vec3d e = (m_t - p);
    Nf::Vec3d e_v = e/e.magnitude();
    Nf::Vec3d ze=R.Col(2);
    alpha_e=(acos(ze.dot(e_v)))*(180/PI);

#ifdef GPS3_SAVING
    Sxyz = m_Tneedletip2robot.GetPosition(); 
#endif
  }

  void ControlAlgorithms::getVisualizerValues(Vec3d &t, Matrix44d &x, Matrix44d &z, Matrix44d &Tref2robot,
    Matrix44d &Ttrans2robot, s32 &transducerType, Cubed &frameBoundaries, Matrix44d &Tem2robot, Matrix44d &Tneedletip2robot)
  {
    t = m_t;
    z = m_z;
    Matrix33d R = m_x.GetOrientation();
    Vec3d wrist = m_x.GetPosition();
    x = Matrix44d::FromOrientationAndTranslation(R, wristToTip(R, wrist));
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
      double currentInsMM = m_l;
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

  void StylusCalibration::SetOffset(arma::mat c)
  {
    m_stylusCalibrationComplete = true;
    m_vcal = c;
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

// cosi la trovi
DWORD WINAPI ControlThread (LPVOID lpParam)
{

  if(!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
   {
      return -1;
   }

  if(!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
   {
      return -1;
   } 


	Nf::ControlAlgorithms* C=(Nf::ControlAlgorithms*) lpParam;
	// timer
  int count=0;
  int STATE=2;
  double prev_ins=0;
  
  double prevalpha_e=1000;
  int countder;
  bool Sliding=false;

  // salvo il tempo
  std::ofstream myfile;
  myfile.open("timeTHREAD.txt");

  s32 dt = 25;
	
  while(1)
	{

    u32 t0 = timeGetTime();
   
    SYSTEMTIME time;
		GetSystemTime(&time);
		LONG time_ms = (time.wSecond * 1000) + time.wMilliseconds;
		myfile << time_ms << endl;

    
    
			  
		if (C->QuickandDirty)
		{
      count++;
      C->m_th = C->m_robot->getRollAngle();
      C->m_l = C->m_robot->getInsMM();
      C->m_EST.updateInput(C->m_l, C->m_th);
      C->m_x =  C->m_EST.getCurrentEstimate() ;

			if (C->m_t.magnitude() > 1e-3) // if the target is defined
			{
			  // Get current tip frame estimate
			  Nf::Vec3d W = C->m_x.GetPosition();
			  Nf::Matrix33d R = C->m_x.GetOrientation();
        Nf::Vec3d uz = R.Col(2);  
        //Nf::Vec3d p=W+uz*15.3; TIP CHANGE 123
        Nf::Vec3d p = wristToTip(R,W);
			  // Get relative error in the current tip frame
			  Nf::Vec3d e = R.Inverse()*(C->m_t - p);
        
        Nf::Vec3d e_0 = (C->m_t - p);
        Nf::Vec3d e_v = e_0/e_0.magnitude();
        Nf::Vec3d ze=R.Col(2);
        double alpha_e=(acos(ze.dot(e_v)))*(180/PI);
        alpha_e = abs(alpha_e);
			  // verificare convenzioni
			  f32 d_th = atan2(-e.x, e.y);

        if ( R.Col(2).dot(C->m_t-p) < 0)
        {
          C->m_robot->SetInsertionVelocity(0.0);
          C->m_robot->SetRotationVelocity(0.0);
          C->QuickandDirty=false;
        }
        else
        {
          switch(STATE)
          {
          case 0:
                 C->m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
                 C->m_robot->SetRotationVelocity(0);
               
                 if ((alpha_e-prevalpha_e)>0)
                 {
                    countder++;
                 }
                 else
                    countder=0;
               
                 if (countder > 500)
			            {
				             C->m_robot->SetInsertionVelocity(0);
				             C->m_robot->SetArticulationAngle(0);
                     Sleep(2000);
                     C->m_robot->InsertIncremental(-10);
                     STATE=1; 
			            }
                  prevalpha_e = alpha_e;
            break;
           case 1:
                 C->m_robot->SetRotationVelocity(0);
                 Sleep(4000);
                 STATE=2;
             break;
           case 2:
                C->m_robot->SetInsertionVelocity(INS_AUTO_SPEED*0.5);         
                if (d_th<0)
                  C->m_robot->SetRotationVelocity(-350);
                else
                  C->m_robot->SetRotationVelocity(350);
             break;
            case 3:
                C->m_robot->SetInsertionVelocity(INS_AUTO_SPEED*0);
                if (d_th<0)
                  d_th=d_th+2*PI;

                C->m_robot->RotateIncremental(d_th*(180/PI));
                STATE=4;
                break;
            case 4:
                C->m_robot->SetInsertionVelocity(INS_AUTO_SPEED*0);
                if (abs(d_th)<0.1)
                  STATE = 5;
             break;
            case 5:
                C->m_robot->InsertIncremental(-3.0);
                prev_ins = C->m_l;
                STATE = 6;
                break;
            case 6:
                if (abs(prev_ins - 3.0 - C->m_l)< 0.01)
                  STATE = 7;
            break;
            case 7:
                C->m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
                // Vibration
                if (alpha_e<1.0)
                {
                    STATE = 8;
                }
                if (abs(d_th)>2)
                   STATE = 3;
            break;
            case 8:
                C->m_robot->SetInsertionVelocity(INS_AUTO_SPEED);
                C->m_robot->SetRotationVelocity(350);
                if (alpha_e>2)
                {
                    STATE = 3;
                }
            break;

          }
        }
      }
    }

    

    u32 tNow = timeGetTime();
    u32 sleepTime = (u32)MAX((__int64)dt-(__int64)(tNow-t0), 0);
    Sleep(sleepTime);
  }
  myfile.close();
}

