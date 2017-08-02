#include "Teleoperation2DWidget.h"

#define   TSCAN       8.0     // total manual scan time in ms 

namespace Nf
{
  Teleoperation2DWidget::Teleoperation2DWidget(QWidget *parent, NeedleSteeringRobot* robot, ControlAlgorithms* control, Mouse3DInput* mouse)
    : Nf::ParameterCollection("Teleoperation 2D")
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
    , m_Tref2robot(Matrix44d::Zero())
  {
    m_robot = robot;
    m_control = control;
    m_mouse = mouse;

    m_imageViewer = std::tr1::shared_ptr<ImageViewer2DTeleoperationWidget>(new ImageViewer2DTeleoperationWidget(parent, control, this));
    m_teleoperationVisualizer = std::tr1::shared_ptr<TeleoperationVisualizationWidget>(new TeleoperationVisualizationWidget(parent, control));

    m_scanButton = new QPushButton("Scan Needle", parent);
    QFont font = m_scanButton->font();
    font.setPointSize(16);
    m_scanButton->setFont(font);
    m_scanButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_scanButton->setFixedWidth(300);
    connect(m_scanButton, SIGNAL(clicked()), this, SLOT(onStartManualNeedleScan()));

    m_taskControlButton = new QPushButton("Start Task-Space Teleoperation", parent);
    font = m_taskControlButton->font();
    font.setPointSize(16);
    m_taskControlButton->setFont(font);
    m_taskControlButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_taskControlButton->setFixedWidth(300);
    connect(m_taskControlButton, SIGNAL(clicked()), this, SLOT(onStartStopTaskSpaceControl()));

    m_jointControlButton = new QPushButton("Start Joint-Space Teleoperation", parent);
    font = m_jointControlButton->font();
    font.setPointSize(16);
    m_jointControlButton->setFont(font);
    m_jointControlButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_jointControlButton->setFixedWidth(300);
    connect(m_jointControlButton, SIGNAL(clicked()), this, SLOT(onStartStopJointSpaceControl()));

    m_robotStatusWidget = new RobotStatusWidget(parent, m_robot);

    m_leftSubLayout = new QHBoxLayout(parent);
    m_leftSubLayout->addWidget((QWidget *)(m_imageViewer.get()));
        
    m_buttonLayout = new QVBoxLayout(parent);
    m_buttonLayout->addWidget(m_scanButton);
    m_buttonLayout->addWidget(m_taskControlButton);
    m_buttonLayout->addWidget(m_jointControlButton);
    m_buttonLayout->setMargin(0);
    m_buttonLayout->setContentsMargins(QMargins(0,0,0,0));
    m_buttonLayout->setSpacing(0);
    
    m_rightSubLayout = new QGridLayout(parent);
    m_rightSubLayout->addWidget((QWidget *)(m_teleoperationVisualizer.get()), 0, 0, 1, 2);
    m_rightSubLayout->addLayout(m_buttonLayout,1,0);
    m_rightSubLayout->addWidget(m_robotStatusWidget);

    m_layout = new QGridLayout(parent);
    m_layout->addLayout(m_leftSubLayout,0,0);    
    m_layout->addLayout(m_rightSubLayout,0,1);
    this->setLayout(m_layout);

    m_preScanTimer = std::tr1::shared_ptr<QTimer>((QTimer *)NULL);

    QObject::connect(m_mouse, SIGNAL(Move3d(std::vector<float>&)), this, SLOT(OnMove(std::vector<float>&)));

    // add framework params
    ADD_ENUM_PARAMETER(m_transducerType, "Transducer", CALLBACK_POINTER(onSetTransducerType, Teleoperation2DWidget), this, QtEnums::Transducer::CONVEX, "Transducer");
    ADD_BOOL_PARAMETER(m_initializeEstimator, "Initialize Estimator", CALLBACK_POINTER(onInitializeEstimator, Teleoperation2DWidget), this, false);
    ADD_BOOL_PARAMETER(m_recordingData, "Record Data", CALLBACK_POINTER(onRecordingData, Teleoperation2DWidget), this, false);
    ADD_ACTION_PARAMETER(m_rotateInline, "Rotate Inline", CALLBACK_POINTER(onRotateInline, Teleoperation2DWidget), this, false);
    ADD_CHILD_COLLECTION(m_imageViewer.get());
    ADD_CHILD_COLLECTION(m_teleoperationVisualizer.get());
  }

  Teleoperation2DWidget::~Teleoperation2DWidget()
  {
  }

  void Teleoperation2DWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void Teleoperation2DWidget::onRotateInline()
  {
    this->m_control->RotateInline();
  }

  void Teleoperation2DWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
    m_imageViewer->UpdateSize(QSize(sz.width()/2-10, sz.height()));
  }

  std::vector < QVTKWidget * > Teleoperation2DWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    res.push_back(m_imageViewer.get());
    return res;
  }

  void Teleoperation2DWidget::onStartStopTaskSpaceControl()
  {
    if( checkCalibrations() ) 
    {
      if( m_control->startStopTaskSpaceControl() ){ // starting task-space control
        m_taskControlButton->setText("Stop Task-Space Teleoperation");
        m_jointControlButton->setEnabled(false);
      }else{ // stopping task-space control
        m_control->setJointSpaceControlVelocities(0.0, 0.0);
        m_taskControlButton->setText("Start Task-Space Teleoperation");
        m_jointControlButton->setEnabled(true);
      }
    }
  }

  void Teleoperation2DWidget::onStartStopJointSpaceControl()
  {
    if( checkCalibrations() )
    {
      if( m_control->startStopJointSpaceControl() ){ // starting joint-space control
        m_jointControlButton->setText("Stop Joint-Space Teleoperation");
        m_taskControlButton->setEnabled(false);
      }else{ // stopping joint-space control
        m_control->setJointSpaceControlVelocities(0.0, 0.0);
        m_jointControlButton->setText("Start Joint-Space Teleoperation");
        m_taskControlButton->setEnabled(true);
      }
    }
  }

  void Teleoperation2DWidget::onStartManualNeedleScan()
  {
    if (checkCalibrations())
    {
      if(!m_preScanTimer) {
        m_preScanTimer = std::tr1::shared_ptr<QTimer>(new QTimer());
        connect(m_preScanTimer.get(), SIGNAL(timeout()), this, SLOT(onManualTimeout()));
        m_preScanTimer->setInterval(2000); }
      m_preScanTimer->start();

      // print prepare for scan message
      char str [100];
      int n = sprintf(str, "Prepare for needle scan");
      m_imageViewer->SetInCountdownToManualScan(true);
      m_imageViewer->SetInstructionText(str);
    }
  }

  void Teleoperation2DWidget::onManualTimeout()
  {
    // stop the preparation timer and begin the manual scan
    m_preScanTimer->stop();
    m_imageViewer->SetInCountdownToManualScan(false);
    m_scanTimer.start();
    displayScanTimeRemaining();
    m_control->startStopManualScanning(true);
  }

  void Teleoperation2DWidget::displayScanTimeRemaining()
  {
    qint64 nsecs = m_scanTimer.nsecsElapsed();
    double secs = double(nsecs/1e9);
    char str [100];
    int n = sprintf(str, "%.2f seconds remaining...",TSCAN-secs);
    m_imageViewer->SetInstructionText(str);

    if( secs > TSCAN ){
      m_control->startStopManualScanning( false );
      m_imageViewer->SetInstructionText(" ");
    }
  }

  void Teleoperation2DWidget::OnMove(std::vector<float>& motionData)
  {
    if (motionData.size() < 6) return;

    if ( m_control->inJointSpaceControl() ){ // if we are using the 3D mouse to control the robot
      float dz = -motionData[1];
      float rz = -motionData[4];
      m_control->setJointSpaceControlVelocities(rz, dz);
    }
  }

  bool Teleoperation2DWidget::checkCalibrations()
  {
    QString port;
    QMessageBox msgBox;

    if( !(m_robot->isRollInitialized()) || !(m_robot->isInsertionInitialized()) ){
      // we haven't initialized the robot yet...
      msgBox.setWindowTitle("DANGER: SLEEPING ROBOT");
      msgBox.setText("Initialize the insertion and rotation DOF with the Robot Hardware Widget.");
      QPushButton *continueButton = msgBox.addButton("Got it!", QMessageBox::ActionRole);
      msgBox.exec();
      return false;
    }

    if( !m_initializeEstimator->GetValue() ){
      // we haven't initialized the estimator yet...
      msgBox.setWindowTitle("WARNING: THIS KALMAN FILTER STINKS!");
      msgBox.setText("Initialize the estimator before teleoperation.");
      QPushButton *continueButton = msgBox.addButton("Got it!", QMessageBox::ActionRole);
      msgBox.exec();
      return false;
    }

    if( m_Tref2robot.isZero() ){
      m_Tref2robot = loadRobotEmCalibration();
    }

    Matrix44d transducerCalibration;
    switch ( m_transducerType->GetValue() ){
    case QtEnums::Transducer::LINEAR :
      transducerCalibration = Matrix44d(14.8449, 0.9477, -0.0018, 0.0, 
        15.0061, 0.0016, 1.00, 0.0, 
        0.1638, 0.0166, 0.0052, 0.0, 
        0.0, 0.0, 0.0, 1.0);
      break;
    case QtEnums::Transducer::CONVEX :
      transducerCalibration = Matrix44d(-29.7558, 0.9433, -0.0034, 0.0, 
        -0.087, 0.0033, 1.00, 0.0, 
        -0.7053, 0.0132, -0.0087, 0.0, 
        0.0, 0.0, 0.0, 1.0);
      break;
    }
    m_control->setCalibration( m_Tref2robot, transducerCalibration, m_transducerType->GetValue() );
    return true;    
  }

  Matrix44d Teleoperation2DWidget::loadRobotEmCalibration()
  {
    // Save rotation and translation
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Robot Calibration File"), QString("F:/ultrasteer/EMCal"), "*.m");
    if(fileName.length() <= 0) {
      return Matrix44d::I();
    }
    std::string fname = fileName.toStdString();
    arma::mat Tref2robot;
    Tref2robot.load(fname);
    return Matrix44d::FromArmaMatrix4x4(Tref2robot);
  }

  void Teleoperation2DWidget::onSetTransducerType()
  {
    checkCalibrations();
  }

  void Teleoperation2DWidget::updateTeleoperationVisualization()
  {
    m_teleoperationVisualizer->onUpdateVisualization();
    m_imageViewer->onUpdateOverlay();
  }

  void Teleoperation2DWidget::onInitializeEstimator()
  {   
    if(m_initializeEstimator->GetValue()){
      QMessageBox msgBox;
      msgBox.setWindowTitle("CAUTION");
      msgBox.setText("Estimator initialization assumes the needle tip is inside the introducer and aligned with the robot's Z-axis.\n\nContinue with initialization?");
      QPushButton *continueButton = msgBox.addButton("Do it!", QMessageBox::ActionRole);
      QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);
      msgBox.exec();
      if (msgBox.clickedButton() == continueButton){
        m_control->initializeEstimator();
      }else{
        m_initializeEstimator->SetValue(false);
      }
    }
  }

  void Teleoperation2DWidget::onRecordingData()
  {
    m_recordingData->SetValue(m_control->startStopRecordingData());
  }


  Teleoperation2DFileWidget::Teleoperation2DFileWidget(QWidget *parent, NeedleSteeringRobot* robot, ControlAlgorithms* control, Mouse3DInput* mouse)
    : Teleoperation2DWidget(parent, robot, control, mouse) 
    , m_rpReaders(NULL)
  {

    //ADD_OPEN_FILE_PARAMETER(m_rpFile, "RP Filename", CALLBACK_POINTER(onUpdateFile, Teleoperation2DFileWidget), this, "F:/TargetScans/TargetDataSet.b8", "Any File (*.*)");
    //ADD_OPEN_FILE_PARAMETER(m_rpFile, "RP Filename", CALLBACK_POINTER(onUpdateFile, Teleoperation2DFileWidget), this, "F:/NeedleScan/8_25_15/TroyData/scan.b8", "Any File (*.*)");
    ADD_OPEN_FILE_PARAMETER(m_rpFile, "RP Filename", CALLBACK_POINTER(onUpdateFile, Teleoperation2DFileWidget), this, "D:/NeedleScan/8_25_15/TroyData/scan.b8", "Any File (*.*)");
    ADD_INT_PARAMETER(m_frame, "Frame Index", CALLBACK_POINTER(onUpdateFrame, Teleoperation2DFileWidget), this, 1, 1, 100, 1);

    onUpdateFile();
  }

  void Teleoperation2DFileWidget::onUpdateFile()
  {
    std::string fname = m_rpFile->GetValue();
    fname = fname.substr(0,fname.find_last_of("."));

    if(m_rpReaders) {
      delete m_rpReaders;
      m_rpReaders = NULL;
    }

    m_rpReaders = new RPFileReaderCollection();

    char temp[200];
    sprintf(temp,"%s.b8",fname.c_str());
    RPFileReader *tempReader = new RPFileReader(temp);
    s32 nframes = tempReader->GetHeader().frames;
    m_rpReaders->AddReader(RPF_BPOST8, (RPReader*)tempReader);
    sprintf(temp,"%s.b32",fname.c_str());
    tempReader = new RPFileReader(temp);
    m_rpReaders->AddReader(RPF_BPOST32, (RPReader *)tempReader);
    sprintf(temp,"%s.gps1",fname.c_str());
    m_rpReaders->AddGPSReader((RPGPSReaderBasic *)new RPGPSReader(temp));
    sprintf(temp,"%s.gps2",fname.c_str());
    m_rpReaders->AddGPSReader2((RPGPSReaderBasic *)new RPGPSReader(temp)); 

    if(nframes <= 0) {
      delete m_rpReaders;
      m_rpReaders = NULL;
      return;
    }

    m_frame->SetMax(nframes);

    onUpdateFrame();
  }

  void Teleoperation2DFileWidget::onUpdateFrame()
  {
    if(m_data.gps.valid)
    {
      m_data.Release();
    }

    m_data = m_rpReaders->GetRPData(m_frame->GetValue());
    m_imageViewer->SetImage(&m_data, RPF_BPOST32);

    m_control->controlHeartBeat(m_data);
    updateTeleoperationVisualization();

    if( m_control->inManualScanning() )
      displayScanTimeRemaining();
  }

  Teleoperation2DFileWidget::~Teleoperation2DFileWidget()
  {

  }

  Teleoperation2DStreamingWidget::Teleoperation2DStreamingWidget(QWidget *parent, NeedleSteeringRobot* robot, ControlAlgorithms* control, Mouse3DInput* mouse)
    : Teleoperation2DWidget(parent, robot, control, mouse) 
    , m_receiver(new RPPushReceiver((RPFrameHandler *)this))
  {
    ADD_CHILD_COLLECTION((ParameterCollection *)m_receiver);
  }

  void Teleoperation2DStreamingWidget::Initialize()
  {
  }

  void Teleoperation2DStreamingWidget::HandleFrame(RPData &rp)
  {
    BEGIN_TIMING(HandleFrame,10);

    m_data.Release();
    m_data = rp;

    BEGIN_TIMING(setImage,10);
    m_imageViewer->SetImage(&m_data, RPF_BPOST32);
    END_TIMING(setImage,10);

    m_control->controlHeartBeat(m_data);
    BEGIN_TIMING(UpdateTeleoperationVisualization,10);
    updateTeleoperationVisualization();
    END_TIMING(UpdateTeleoperationVisualization,10);

    if( m_control->inManualScanning() )
      displayScanTimeRemaining();

    END_TIMING(HandleFrame,10);
  }

  Teleoperation2DStreamingWidget::~Teleoperation2DStreamingWidget()
  {
  }



}
