#include "Teleoperation2DWidget.h"

namespace Nf
{
  Teleoperation2DWidget::Teleoperation2DWidget(QWidget *parent)
    : Nf::ParameterCollection("Teleoperation 2D")
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
    , m_robot(NULL)
    , m_control(NULL)
  {

    m_imageViewer = std::tr1::shared_ptr<ImageViewer2DTeleoperationWidget>(new ImageViewer2DTeleoperationWidget(parent));

    m_layout = new QGridLayout(parent);
    m_layout->addWidget((QWidget *)(m_imageViewer.get()), 0, 0);
    this->setLayout(m_layout);

    m_imageViewer->SetTeleoperation2DWidget(this);

    ADD_CHILD_COLLECTION(m_imageViewer.get());

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

  void Teleoperation2DWidget::setRobot(NeedleSteeringRobot* robot)
  {
    m_robot = robot;
  }

  void Teleoperation2DWidget::setControl(ControlAlgorithms* control)
  {
    m_control = control;
  }

  void Teleoperation2DWidget::UpdateTargetPoint(Vec2d pt)
  {
    qDebug() << "Update Target Point\n";

    if(m_data.gps.valid)
    { 
      // Convert the input image point to world coordinates using the GPS data
      Matrix44d cal(14.8449, 0.9477, -0.0018, 0.0, 15.0061, 0.0016, 1.00, 0.0, 0.1638, 0.0166, 0.0052, 0.0, 0.0, 0.0, 0.0, 1.0);
      Matrix44d tPose = Matrix44d::FromCvMat(m_data.gps.pose);
      Matrix33d pose = tPose.GetOrientation();
      Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, m_data.gps.pos);
      Vec2d scale(m_data.mpp/1000.0, m_data.mpp/1000.0);
      Vec3d pt_world = rpImageCoordToWorldCoord3(pt, posePos, cal, m_data.origin, scale);

      // Update the control algorithm target point
      m_control->SetTarget(pt_world);

      // Update the overlay
      onUpdateOverlay();
    }
  }

  Teleoperation2DFileWidget::Teleoperation2DFileWidget(QWidget *parent)
    : Teleoperation2DWidget(parent) 
    , m_rpReaders(NULL)
  {

    ADD_OPEN_FILE_PARAMETER(m_rpFile, "RP Filename", CALLBACK_POINTER(onUpdateFile, Teleoperation2DFileWidget), this, "F:/TargetScans/TargetDataSet.b8", "Any File (*.*)");
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
    m_imageViewer->SetImage(m_data.b8);
    onUpdateOverlay();

  }

  void Teleoperation2DWidget::onUpdateOverlay()
  {
    if( m_control && m_control->isTargetDefined() )
    {
      // Get the image pose/position information
      Matrix44d cal(14.8449, 0.9477, -0.0018, 0.0, 15.0061, 0.0016, 1.00, 0.0, 0.1638, 0.0166, 0.0052, 0.0, 0.0, 0.0, 0.0, 1.0);
      Matrix44d tPose = Matrix44d::FromCvMat(m_data.gps.pose);
      Matrix33d pose = tPose.GetOrientation();
      Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, m_data.gps.pos);
      Vec2d scale(m_data.mpp/1000.0, m_data.mpp/1000.0);

      // Convert the world target coordinates to the 3D image frame using the GPS data
      Vec3d pt_world;
      m_control->GetTarget(pt_world);
      Vec3d pt_img3D = rpWorldCoord3ToImageCoord(pt_world, posePos, cal, m_data.origin, scale);

      // Interpet circle location and draw the overlay circle
      int z = fabs(pt_img3D.z);
      m_imageViewer->SetTargetOverlay(40-z, Vec2d(pt_img3D.x,pt_img3D.y));
    }
  }

  Teleoperation2DFileWidget::~Teleoperation2DFileWidget()
  {

  }
  
  Teleoperation2DStreamingWidget::Teleoperation2DStreamingWidget(QWidget *parent)
    : Teleoperation2DWidget(parent) 
  {
    ADD_STRING_PARAMETER(m_rpIp, "Ulterius IP", NULL, this, "192.168.1.64");
    ADD_BOOL_PARAMETER(m_init, "Initialize", CALLBACK_POINTER(onInitializeToggle, Teleoperation2DStreamingWidget), this, false);
    ADD_BOOL_PARAMETER(m_addFrames, "Add Frames", CALLBACK_POINTER(onAddFramesToggle, Teleoperation2DStreamingWidget), this, false);
    ADD_INT_PARAMETER(m_framerate, "Ulterius Framerate", CALLBACK_POINTER(onFramerateChanged, Teleoperation2DStreamingWidget), this, 11, 1, 30, 1);
    ADD_FLOAT_PARAMETER(m_mpp, "MPP", CALLBACK_POINTER(onInitializeToggle, Teleoperation2DStreamingWidget), this, 60, 20, 150, 1.0);
    ADD_VEC2D_PARAMETER(m_origin, "Frame Origin", CALLBACK_POINTER(onInitializeToggle, Teleoperation2DStreamingWidget), this, Vec2d(330, 42), Vec2d(0,0), Vec2d(10000, 10000), Vec2d(1,1));

    onInitializeToggle();
    onAddFramesToggle();

    m_tick = std::tr1::shared_ptr<QTimer>((QTimer *)NULL);
    m_rpReaders = std::tr1::shared_ptr<RPUlteriusProcessManager>((RPUlteriusProcessManager *)NULL);
  }

  void Teleoperation2DStreamingWidget::onInitializeToggle()
  {
    if(m_init->GetValue()) {
      m_rpReaders = std::tr1::shared_ptr < RPUlteriusProcessManager >(new RPUlteriusProcessManager(m_rpIp->GetValue().c_str(), (f64)m_mpp->GetValue(), m_origin->GetValue(), m_framerate->GetValue()));
      Sleep(30);  //Wait for old processes to die
      m_rpReaders->EnableType(RPF_BPOST8, 1);
      m_rpReaders->EnableType(RPF_GPS,1);
      Sleep(300);

      m_data = m_rpReaders->GetNextRPData();
      m_imageViewer->SetImage(m_data.b8);

      if(!m_tick) {
        m_tick = std::tr1::shared_ptr<QTimer>(new QTimer());
        connect(m_tick.get(), SIGNAL(timeout()), this, SLOT(onTick()));
        m_tick->setInterval(90);
        m_tick->start();
      }
    }
  }

  void Teleoperation2DStreamingWidget::onAddFramesToggle()
  {
    //Don't do anything if we're not initialized
    if(!m_init->GetValue())
      return;

    if(m_addFrames->GetValue()) {
      m_data = m_rpReaders->GetNextRPData();
    }
  }

  void Teleoperation2DStreamingWidget::onTick()
  {
    if(!m_init->GetValue())
      return;

    if(m_data.gps.valid)
      m_data.Release();
    m_data = m_rpReaders->GetNextRPData();
    if(m_data.b8 == NULL || !m_data.gps.valid)
      return;

    if(m_addFrames->GetValue()) {
      m_imageViewer->SetImage(m_data.b8);
      onUpdateOverlay();
    }
  }

  void Teleoperation2DStreamingWidget::onFramerateChanged()
  {
    if(!m_init->GetValue())
      return;

    m_rpReaders->SetFrameRate(m_framerate->GetValue());
  }

  Teleoperation2DStreamingWidget::~Teleoperation2DStreamingWidget()
  {
  }

}
