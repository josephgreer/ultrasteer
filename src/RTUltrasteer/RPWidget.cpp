#include "RPWidget.h"

namespace Nf
{
  RPWidget::RPWidget(QWidget *parent)
    : Nf::ParameterCollection("RP Image Viewer")
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
    , m_rpReaders(NULL)
  {

    m_imageViewer = std::tr1::shared_ptr<ImageViewerWidget>(new ImageViewerWidget(parent));
    m_usVis = std::tr1::shared_ptr<USVisualizer>(new USVisualizer(parent, NULL));

    m_usVisDock = new QDockWidget(this);
    m_imViewDock = new QDockWidget(this);

    m_usVisDock->setWidget(m_usVis);
    m_imageViewer->setWidget(m_imageViewer);
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, m_imViewDock);
    addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, m_usVisDock);


    ADD_OPEN_FILE_PARAMETER(m_rpFile, "RP Filename", CALLBACK_POINTER(onUpdateFile, RPWidget), this, "V:/NeedleScan/Feb13_LiverScan/Scan 1/scan.b8", "Any File (*.*)");
    ADD_INT_PARAMETER(m_frame, "Frame Index", CALLBACK_POINTER(onUpdateFrame, RPWidget), this, 1, 1, 100, 1);
    ADD_CHILD_COLLECTION(m_usVis.get());
    ADD_CHILD_COLLECTION(m_imageViewer.get());

    onUpdateFile();
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
  }

  void RPWidget::onUpdateFile()
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
    m_rpReaders->AddReader(RPF_BPOST8, (RPReader *)new RPFileReader(temp));
    sprintf(temp,"%s.b32",fname.c_str());
    RPFileReader *tempReader = new RPFileReader(temp);
    s32 nframes = tempReader->GetHeader().frames;
    m_rpReaders->AddReader(RPF_BPOST32, (RPReader *)tempReader);
    sprintf(temp,"%s.gps1",fname.c_str());
    m_rpReaders->AddGPSReader((RPGPSReaderBasic *)new RPGPSReader(temp));  

    m_frame->SetMax(nframes);

    m_usVis->Reinitialize();
    
    onUpdateFrame();
  }

  void RPWidget::onUpdateFrame()
  {
    if(m_data.gps.valid)
      m_data.Release();

    m_data = m_rpReaders->GetRPData(m_frame->GetValue());
    m_imageViewer->SetImage(m_data.b8);
    m_usVis->AddRPData(m_data);
  }

  std::vector < QVTKWidget * > RPWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res = m_usVis->GetRepaintList();
    res.push_back(m_imageViewer.get());
    return res;
  }

}