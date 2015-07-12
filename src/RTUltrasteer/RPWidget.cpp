#include "RPWidget.h"

namespace Nf
{
  RPWidget::RPWidget(QWidget *parent, USVisualizer *vis)
    : Nf::ParameterCollection("RP Image Viewer")
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
  {
    m_imageViewer = std::tr1::shared_ptr<ImageViewerWidget>(new ImageViewerWidget(parent));
    m_usVis = std::tr1::shared_ptr<USVisualizer>(vis != NULL ? vis : new USVisualizer(parent, NULL));
    m_planeVis = std::tr1::shared_ptr<Image3DImagePlaneWidget>(new Image3DImagePlaneWidget(parent));

    m_visTab = new QTabWidget(parent);
    m_visTab->addTab(m_usVis.get(), "Volume Visualization");
    m_visTab->addTab(m_planeVis.get(), "Plane Visualization");

    m_layout = new QGridLayout(parent);
    m_layout->addWidget((QWidget *)(m_imageViewer.get()), 0, 0);
    m_layout->addWidget((QWidget *)(m_visTab), 0, 1);
    m_layout->setContentsMargins(0,0,5,0);
    this->setLayout(m_layout);
    

    ADD_CHILD_COLLECTION(m_usVis.get());
    ADD_CHILD_COLLECTION(m_imageViewer.get());
    ADD_CHILD_COLLECTION(m_planeVis.get());
  }

  RPWidget::~RPWidget()
  {
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
    res.push_back(m_imageViewer.get());
    return res;
  }

  
  RPFileWidget::RPFileWidget(QWidget *parent, USVisualizer *vis)
    : RPWidget(parent, vis) 
    , m_rpReaders(NULL)
  {

    ADD_OPEN_FILE_PARAMETER(m_rpFile, "RP Filename", CALLBACK_POINTER(onUpdateFile, RPFileWidget), this, "F:/NeedleScan/11_19_14_Liverscan/Scan4_CurvedFull/scan.b8", "Any File (*.*)");
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
    sprintf(temp,"%s.b32",fname.c_str());
    tempReader = new RPFileReader(temp);
    m_rpReaders->AddReader(RPF_BPOST32, (RPReader *)tempReader);
    sprintf(temp,"%s.gps1",fname.c_str());
    m_rpReaders->AddGPSReader((RPGPSReaderBasic *)new RPGPSReader(temp));  

    m_frame->SetMax(nframes);

    if(nframes <= 0) {
      delete m_rpReaders;
      m_rpReaders = NULL;
    }

    m_usVis->Reinitialize();
    
    onUpdateFrame();
  }

  void RPFileWidget::onUpdateFrame()
  {
    if(m_data.gps.valid)
      m_data.Release();

    if(m_rpReaders)
      m_data = m_rpReaders->GetRPData(m_frame->GetValue());
    m_imageViewer->SetImage(&m_data);
    m_usVis->AddRPData(m_data);
    m_planeVis->SetImage(&m_data);
  }

  RPFileWidget::~RPFileWidget()
  {
  }

  
  RPStreamingWidget::RPStreamingWidget(QWidget *parent, USVisualizer *vis)
    : RPWidget(parent, vis) 
  {
    ADD_STRING_PARAMETER(m_rpIp, "Ulterius IP", NULL, this, "192.168.1.64");
    ADD_BOOL_PARAMETER(m_init, "Initialize", CALLBACK_POINTER(onInitializeToggle, RPStreamingWidget), this, false);
    ADD_BOOL_PARAMETER(m_addFrames, "Add Frames", CALLBACK_POINTER(onAddFramesToggle, RPStreamingWidget), this, false);
    ADD_INT_PARAMETER(m_framerate, "Ulterius Framerate", CALLBACK_POINTER(onFramerateChanged, RPStreamingWidget), this, 11, 1, 30, 1);
    ADD_FLOAT_PARAMETER(m_mpp, "MPP", CALLBACK_POINTER(onInitializeToggle, RPStreamingWidget), this, 60, 20, 150, 1.0);
    ADD_VEC2D_PARAMETER(m_origin, "Frame Origin", CALLBACK_POINTER(onInitializeToggle, RPStreamingWidget), this, Vec2d(330, 42), Vec2d(0,0), Vec2d(10000, 10000), Vec2d(1,1));

    onInitializeToggle();
    onAddFramesToggle();
    
    m_tick = std::tr1::shared_ptr<QTimer>((QTimer *)NULL);
    m_rpReaders = std::tr1::shared_ptr<RPUlteriusProcessManager>((RPUlteriusProcessManager *)NULL);
  }

  void RPStreamingWidget::onInitializeToggle()
  {
    if(m_init->GetValue()) {
      m_rpReaders = std::tr1::shared_ptr < RPUlteriusProcessManager >(new RPUlteriusProcessManager(m_rpIp->GetValue().c_str(), (f64)m_mpp->GetValue(), m_origin->GetValue(), m_framerate->GetValue()));
      Sleep(30);  //Wait for old processes to die
      m_rpReaders->EnableType(RPF_BPOST8, 1);
      m_rpReaders->EnableType(RPF_GPS,1);
      Sleep(300);

      m_data = m_rpReaders->GetNextRPData();
      m_usVis->Initialize(m_data);
      m_imageViewer->SetImage(&m_data);

      if(!m_tick) {
        m_tick = std::tr1::shared_ptr<QTimer>(new QTimer());
        connect(m_tick.get(), SIGNAL(timeout()), this, SLOT(onTick()));
        m_tick->setInterval(90);
        m_tick->start();
      }
    }
  }

  void RPStreamingWidget::onAddFramesToggle()
  {
    //Don't do anything if we're not initialized
    if(!m_init->GetValue())
      return;

    if(m_addFrames->GetValue()) {
      m_data = m_rpReaders->GetNextRPData();
      m_usVis->Reinitialize();
    }
  }

  void RPStreamingWidget::onTick()
  {
    if(!m_init->GetValue())
      return;

    if(m_data.gps.valid)
      m_data.Release();
    m_data = m_rpReaders->GetNextRPData();
    if(m_data.b8 == NULL || !m_data.gps.valid)
      return;

    if(!m_addFrames->GetValue()) {
      m_usVis->UpdatePos(m_data);
    } else {
      m_usVis->AddRPData(m_data);
      m_planeVis->SetImage(&m_data);
      m_imageViewer->SetImage(&m_data);
    }
  }

  void RPStreamingWidget::onFramerateChanged()
  {
    if(!m_init->GetValue())
      return;

    m_rpReaders->SetFrameRate(m_framerate->GetValue());
  }

  RPStreamingWidget::~RPStreamingWidget()
  {
  }

}