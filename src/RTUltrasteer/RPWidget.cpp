#include "RPWidget.h"

namespace Nf
{
  RPWidget::RPWidget(QWidget *parent, USVisualizer *vis, const char *name)
    : Nf::ParameterCollection(name)
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
  {
    ADD_ENUM_PARAMETER(m_displayMode, "Display Modality", CALLBACK_POINTER(onSetDisplayMode, RPWidget), this, QtEnums::DisplayModality::DM_BPOST8, "DisplayModality");

    m_imageViewer = std::tr1::shared_ptr<ImageViewerWidget>(new ImageViewerWidget(parent));
    m_usVis = std::tr1::shared_ptr<USVisualizer>(vis != NULL ? vis : new USVisualizer(parent, NULL));
    m_planeVis = std::tr1::shared_ptr<Image3DImagePlaneWidget>(new Image3DImagePlaneWidget(parent));

    m_visTab = new QTabWidget(parent);
    m_visTab->addTab(m_usVis.get(), "Volume Visualization");
    m_visTab->addTab(m_planeVis.get(), "Plane Visualization");
    m_visTab->setCurrentIndex(1);

    m_layout = new QGridLayout(parent);
    m_layout->addWidget((QWidget *)(m_imageViewer.get()), 0, 0);
    m_layout->addWidget((QWidget *)(m_visTab), 0, 1);
    m_layout->setContentsMargins(0,0,5,0);
    this->setLayout(m_layout);

    m_cal = Matrix44d(TRANSDUCER_CALIBRATION_COEFFICIENTS);

    ADD_CHILD_COLLECTION(m_usVis.get());
    ADD_CHILD_COLLECTION(m_imageViewer.get());
    ADD_CHILD_COLLECTION(m_planeVis.get());
  }

  RPWidget::~RPWidget()
  {
  }

  void RPWidget::onSetDisplayMode()
  {
    return;
  }

  void RPWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void RPWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
    m_imageViewer->UpdateSize(QSize(sz.width()/2-10, sz.height()));
    m_usVis->UpdateSize(QSize(sz.width()/2-10, sz.height()));
    m_planeVis->UpdateSize(QSize(sz.width()/2-10, sz.height()));
  }

  std::vector < QVTKWidget * > RPWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res = m_usVis->GetRepaintList();
    res.push_back(m_planeVis.get());
    res.push_back(m_imageViewer.get());
    return res;
  }

  
  RPFileWidget::RPFileWidget(QWidget *parent, USVisualizer *vis, const char *name)
    : RPWidget(parent, vis, name) 
    , m_rpReaders(NULL)
  {

    ADD_OPEN_FILE_PARAMETER(m_rpFile, "RP Filename", CALLBACK_POINTER(onUpdateFile, RPFileWidget), this, PATH_CAT("Trial3/Insertion/scan.b8"), "Any File (*.*)");
    ADD_INT_PARAMETER(m_frame, "Frame Index", CALLBACK_POINTER(onUpdateFrame, RPFileWidget), this, 1, 1, 100, 1);

    onUpdateFile();
  }

  void RPFileWidget::onUpdateFile()
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

    //BPOST32
    sprintf(temp,"%s.b32",fname.c_str());
    tempReader = new RPFileReader(temp);
    if(tempReader->GetHeader().frames != 0)
      m_rpReaders->AddReader(RPF_BPOST32, (RPReader *)tempReader);
    else
      delete tempReader;

    //GPS1
    sprintf(temp,"%s.gps1",fname.c_str());
    RPGPSReader *tempGPS = new RPGPSReader(temp);
    m_rpReaders->AddGPSReader((RPGPSReaderBasic *)tempGPS);

    sprintf(temp,"%s.gps2",fname.c_str());
    tempGPS = new RPGPSReader(temp);
    if(tempGPS->GetHeader().frames != 0)
      m_rpReaders->AddGPSReader2((RPGPSReaderBasic *)tempGPS);
    else
      delete tempGPS;

    sprintf(temp, "%s.u", fname.c_str());
    FILE *tempf = fopen(temp, "rb");
    if(tempf != NULL) {
      fclose(tempf);
      tempf = NULL;
      m_rpReaders->AddNSCommandReader(new NSCommandReader(temp));
    }

    m_frame->SetMax(nframes);

    if(nframes <= 0) {
      delete m_rpReaders;
      m_rpReaders = NULL;
    }

    //If we have a valid frame, set it
    m_frame->SetValue(1);
    m_frame->SetMin(1);

    onUpdateFrame();

    m_imageViewer->ResetView();

    m_usVis->Reinitialize();

    if(m_visTab->currentIndex() == 1)
      m_planeVis->onSetViewXY();
  }

  void RPFileWidget::onSetDisplayMode()
  {
    onUpdateFrame();
  }

  void RPFileWidget::onUpdateFrame()
  {
    if(m_data.gps.valid)
      m_data.Release();

    if(m_rpReaders)
      m_data = m_rpReaders->GetRPData(m_frame->GetValue());
    m_imageViewer->SetImage(&m_data, (RP_TYPE)m_displayMode->GetValue());
    if(m_visTab->currentIndex() == 0)
      m_usVis->AddRPData(m_data);
    else
      m_planeVis->SetImage(&m_data, (RP_TYPE)m_displayMode->GetValue());
  }

  RPFileWidget::~RPFileWidget()
  {
  }

  //////////////////////////////////////////////////////////////////
  // BEGIN RPPushReceiver
  //////////////////////////////////////////////////////////////////
  RPPushReceiver::RPPushReceiver(RPFrameHandler *frameHandler)
    : m_frameHandler(frameHandler)
    , ParameterCollection("RP Streaming")
  {
    ADD_STRING_PARAMETER(m_rpIp, "Ulterius IP", NULL, this, "192.168.1.129");
    ADD_BOOL_PARAMETER(m_init, "Initialize", CALLBACK_POINTER(onInitializeToggle, RPPushReceiver), this, false);
    ADD_FLOAT_PARAMETER(m_mpp, "Microns Per Pixel", CALLBACK_POINTER(onFrameInfoChanged, RPPushReceiver), this, 233, 20, 500, 1.0);
    ADD_FLOAT_PARAMETER(m_sos, "Speed of Sound", CALLBACK_POINTER(onFrameInfoChanged, RPPushReceiver), this, 1540, 1300, 1800, 1.0);
    ADD_VEC2D_PARAMETER(m_origin, "Frame Origin", CALLBACK_POINTER(onFrameInfoChanged, RPPushReceiver), this, Vec2d(330, -153), Vec2d(-500,-500), Vec2d(10000, 10000), Vec2d(1,1));
    ADD_BOOL_PARAMETER(m_rcvDoppler, "Receive Doppler", CALLBACK_POINTER(onDataToAcquireChanged, RPPushReceiver), this, false);
    ADD_BOOL_PARAMETER(m_rcvGps2, "Receive GPS2", CALLBACK_POINTER(onDataToAcquireChanged, RPPushReceiver), this, true);
    
    m_rpReaders = std::tr1::shared_ptr<RPUlteriusReaderCollectionPush>((RPUlteriusReaderCollectionPush *)NULL);
    
    connect(this, SIGNAL(frameSignal()), SLOT(onFrame()), Qt::QueuedConnection);
  }

  void RPPushReceiver::onInitializeToggle()
  {
    if(m_init->GetValue()) {
      Vec2d mpp(m_mpp->GetValue(), m_mpp->GetValue()*m_sos->GetValue()/NOMINAL_SOS);
      m_rpReaders = std::tr1::shared_ptr < RPUlteriusReaderCollectionPush >(new RPUlteriusReaderCollectionPush(m_rpIp->GetValue().c_str(), mpp, m_origin->GetValue()));
      m_rpReaders->SetRPCallbackReceiver(this);
      m_rpReaders->EnableType(RPF_BPOST8, 1);
      m_rpReaders->EnableType(RPF_GPS,1);
      if(m_rcvGps2->GetValue())
        m_rpReaders->EnableType(RPF_GPS2, 1);
      if(m_rcvDoppler->GetValue())
        m_rpReaders->EnableType(RPF_BPOST32, 1);
      Sleep(200);

      m_frameHandler->Initialize(m_init->GetValue());
    }
  }

  void RPPushReceiver::onDataToAcquireChanged()
  {
    if(m_init->GetValue()) {
      u32 mask = RPF_GPS|RPF_BPOST8;
      if(m_rcvDoppler->GetValue() == true)
        mask = mask|RPF_BPOST32;
      if(m_rcvGps2->GetValue() == true)
        mask = mask|RPF_GPS2;
      m_rpReaders->EnableMask(mask);
    }
  }

  void RPPushReceiver::onFrameInfoChanged()
  {
    Vec2d mpp(m_mpp->GetValue(), m_mpp->GetValue()*m_sos->GetValue()/NOMINAL_SOS);
    m_rpReaders->SetFrameInformation(mpp, m_origin->GetValue());
    m_frameHandler->FrameinfoChanged();
  }

  void RPPushReceiver::Callback(const RPData *rp)
  {
    if(rp && m_lock.tryLock(30)) {
      m_data.Release();
      m_data = *rp;
      m_lock.unlock();
      emit frameSignal();
    }
  }
  
  void RPPushReceiver::onFrame()
  {
    if(!m_lock.tryLock(30))
      return;
    
    RPData rp = m_data.Clone();
    m_lock.unlock();
    m_frameHandler->HandleFrame(rp);
  }


  //////////////////////////////////////////////////////////////////
  // END RPPushReceiver
  //////////////////////////////////////////////////////////////////

  
  RPStreamingWidget::RPStreamingWidget(QWidget *parent, USVisualizer *vis, const char *name)
    : RPWidget(parent, vis, name) 
    , m_isInit(false)
    , m_receiver(new RPPushReceiver((RPFrameHandler *)this))
  {
    ADD_BOOL_PARAMETER(m_addFrames, "Add Frames", CALLBACK_POINTER(onAddFramesToggle, RPStreamingWidget), this, false);
    ADD_CHILD_COLLECTION((ParameterCollection *)m_receiver);
  }

  void RPStreamingWidget::InitializeAssets()
  {
    if(!(m_data.b8 && m_data.gps.valid)) 
      return;

    m_usVis->Initialize(m_data);
    m_imageViewer->SetImage(&m_data, (RP_TYPE)m_displayMode->GetValue());
    m_isInit = true;
  }

  void RPStreamingWidget::onAddFramesToggle()
  {
    if(m_addFrames->GetValue()) {
      m_usVis->Reinitialize();
    }
  }

  void RPStreamingWidget::HandleFrame(RPData &rp)
  {
    if(rp.b8 == NULL || !rp.gps.valid)
      return;

    m_data.Release();
    m_data = rp;

    if(!m_isInit)
      InitializeAssets();
      

    if(!m_addFrames->GetValue()) {
      m_usVis->UpdatePos(m_data);
      m_imageViewer->SetImage(&m_data, (RP_TYPE)m_displayMode->GetValue());
    } else {
      m_imageViewer->SetImage(&m_data, (RP_TYPE)m_displayMode->GetValue());
      if(m_visTab->currentIndex() == 0)
        m_usVis->AddRPData(m_data);
      else
        m_planeVis->SetImage(&m_data, (RP_TYPE)m_displayMode->GetValue());
    }
  }

  void RPStreamingWidget::FrameinfoChanged()
  {
    m_imageViewer->ResetView();
  }

  RPStreamingWidget::~RPStreamingWidget()
  {
  }

}