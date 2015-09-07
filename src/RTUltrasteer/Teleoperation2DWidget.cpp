#include "Teleoperation2DWidget.h"

#define   TSCAN       4.0     // total manual scan time in ms
#define   MOUSE_MAX   0.010   // maximum reading per axis from the 3DConnexion input device 

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
    m_scanButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    connect(m_scanButton, SIGNAL(clicked()), this, SLOT(onStartManualNeedleScan()));

    m_taskControlButton = new QPushButton("Start Task-Space Teleoperation", parent);
    font = m_taskControlButton->font();
    font.setPointSize(16);
    m_taskControlButton->setFont(font);
    m_taskControlButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    connect(m_taskControlButton, SIGNAL(clicked()), this, SLOT(onStartStopTaskSpaceControl()));

    m_jointControlButton = new QPushButton("Start Joint-Space Teleoperation", parent);
    font = m_jointControlButton->font();
    font.setPointSize(16);
    m_jointControlButton->setFont(font);
    m_jointControlButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    connect(m_jointControlButton, SIGNAL(clicked()), this, SLOT(onStartStopJointSpaceControl()));

    m_leftSubLayout = new QHBoxLayout(parent);
    m_leftSubLayout->addWidget((QWidget *)(m_teleoperationVisualizer.get()));
    m_leftSubLayout->addWidget((QWidget *)(m_imageViewer.get()));

    m_rightSubLayout = new QVBoxLayout(parent); 
    m_rightSubLayout->addWidget(m_scanButton);
    m_rightSubLayout->addWidget(m_taskControlButton);
    m_rightSubLayout->addWidget(m_jointControlButton);
    m_rightSubLayout->setMargin(0);
    m_rightSubLayout->setContentsMargins(QMargins(0,0,0,0));
    m_rightSubLayout->setSpacing(0);

    m_layout = new QGridLayout(parent);
    m_layout->addLayout(m_leftSubLayout,0,0);    
    m_layout->addLayout(m_rightSubLayout,0,1);
    this->setLayout(m_layout);

    m_preScanTimer = std::tr1::shared_ptr<QTimer>((QTimer *)NULL);

    QObject::connect(m_mouse, SIGNAL(Move3d(std::vector<float>&)), this, SLOT(OnMove(std::vector<float>&)));

    // add framework params
    ADD_ENUM_PARAMETER(m_transducerType, "Transducer", CALLBACK_POINTER(onSetTransducerType, Teleoperation2DWidget), this, QtEnums::Transducer::LINEAR, "Transducer");
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
    checkCalibrations(); 

    if( m_control->startStopTaskSpaceControl() ){
      m_taskControlButton->setText("Stop Task-Space Teleoperation");
      m_jointControlButton->setEnabled(false);
    }else{
      m_taskControlButton->setText("Start Task-Space Teleoperation");
      m_jointControlButton->setEnabled(true);
    }
  }

  void Teleoperation2DWidget::onStartStopJointSpaceControl()
  {
    checkCalibrations(); 

    if( m_control->startStopJointSpaceControl() ){ // starting joint-space control
      m_jointControlButton->setText("Stop Joint-Space Teleoperation");
      m_taskControlButton->setEnabled(false);
    }else{ // stopping joint-space control
      m_control->setJointSpaceControlVelocities(0.0, 0.0);
      m_jointControlButton->setText("Start Joint-Space Teleoperation");
      m_taskControlButton->setEnabled(true);
    }
  }

  void Teleoperation2DWidget::onStartManualNeedleScan()
  {
    checkCalibrations();

    if(!m_preScanTimer) {
      m_preScanTimer = std::tr1::shared_ptr<QTimer>(new QTimer());
      connect(m_preScanTimer.get(), SIGNAL(timeout()), this, SLOT(onManualTimeout()));
      m_preScanTimer->setInterval(2000); }
    m_preScanTimer->start();

    // print prepare for scan message
    char str [100];
    int n = sprintf(str, "Prepare for needle scan");
    m_imageViewer->SetInstructionText(str);
  }

  void Teleoperation2DWidget::onManualTimeout()
  {
    // stop the preparation timer and begin the manual scan
    m_preScanTimer->stop();
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
      float dz = motionData[2];
      float rz = motionData[5];
      m_control->setJointSpaceControlVelocities(rz/MOUSE_MAX, dz/MOUSE_MAX);
    }
  }

  void Teleoperation2DWidget::checkCalibrations()
  {
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
    m_imageViewer->onUpdateOverlay();
    m_teleoperationVisualizer->onUpdateVisualization();
  }

  Teleoperation2DFileWidget::Teleoperation2DFileWidget(QWidget *parent, NeedleSteeringRobot* robot, ControlAlgorithms* control, Mouse3DInput* mouse)
    : Teleoperation2DWidget(parent, robot, control, mouse) 
    , m_rpReaders(NULL)
  {

    //ADD_OPEN_FILE_PARAMETER(m_rpFile, "RP Filename", CALLBACK_POINTER(onUpdateFile, Teleoperation2DFileWidget), this, "F:/TargetScans/TargetDataSet.b8", "Any File (*.*)");
    ADD_OPEN_FILE_PARAMETER(m_rpFile, "RP Filename", CALLBACK_POINTER(onUpdateFile, Teleoperation2DFileWidget), this, "F:/NeedleScan/8_25_15/TroyData/scan.b8", "Any File (*.*)");
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
  {
    ADD_STRING_PARAMETER(m_rpIp, "Ulterius IP", NULL, this, "192.168.1.129");
    ADD_BOOL_PARAMETER(m_init, "Initialize", CALLBACK_POINTER(onInitializeToggle, Teleoperation2DStreamingWidget), this, false);
    ADD_FLOAT_PARAMETER(m_mpp, "MPP", CALLBACK_POINTER(onInitializeToggle, Teleoperation2DStreamingWidget), this, 405, 20, 1000, 1.0);
    ADD_VEC2D_PARAMETER(m_origin, "Frame Origin", CALLBACK_POINTER(onInitializeToggle, Teleoperation2DStreamingWidget), this, Vec2d(330, -56), Vec2d(-500,-500), Vec2d(10000, 10000), Vec2d(1,1));

    onInitializeToggle();

    m_rpReaders = std::tr1::shared_ptr<RPUlteriusReaderCollectionPush>((RPUlteriusReaderCollectionPush *)NULL);
    
    m_receiver = new RPPushReceiver((RPFrameHandler *)this);
  }

  void Teleoperation2DStreamingWidget::onInitializeToggle()
  {
    if(m_init->GetValue()) {
      Vec2d mpp(m_mpp->GetValue(), m_mpp->GetValue());// TODO ADD SOS *m_sos->GetValue()/NOMINAL_SOS);
      m_rpReaders = std::tr1::shared_ptr < RPUlteriusReaderCollectionPush >(new RPUlteriusReaderCollectionPush(m_rpIp->GetValue().c_str(), mpp, m_origin->GetValue()));
      m_rpReaders->SetRPCallbackReceiver(m_receiver);
      Sleep(20);
      m_rpReaders->EnableMask(RPF_BPOST8|RPF_BPOST32|RPF_GPS|RPF_GPS2);
    }
  }

  void Teleoperation2DStreamingWidget::HandleFrame(RPData &rp)
  {
    if(!m_init->GetValue())
      return;

    m_data.Release();
    m_data = rp;

    m_imageViewer->SetImage(&m_data, RPF_BPOST32);

    m_control->controlHeartBeat(m_data);
    updateTeleoperationVisualization();

    if( m_control->inManualScanning() )
      displayScanTimeRemaining();
  }

  Teleoperation2DStreamingWidget::~Teleoperation2DStreamingWidget()
  {
  }



}
