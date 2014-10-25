#include "RPWidget.h"

namespace Nf
{
  RPWidget::RPWidget(QWidget *parent)
    : Nf::ParameterCollection("RP Image Viewer")
    , QWidget(parent)
    , m_rpReaders(NULL)
  {
    ADD_OPEN_FILE_PARAMETER(m_rpFile, "RP Filename", CALLBACK_POINTER(onUpdateFile, RPWidget), this, "V:/NeedleScan/Feb13_LiverScan/Scan 1/scan.b8", "Any File (*.*)");
    ADD_INT_PARAMETER(m_frame, "Frame Index", CALLBACK_POINTER(onUpdateFrame, RPWidget), this, 1, 1, 100, 1);

    m_imageViewer = std::tr1::shared_ptr<ImageViewerWidget>(new ImageViewerWidget(parent));

    m_layout = new QGridLayout(parent);
    m_layout->addWidget((QWidget *)(m_imageViewer.get()), 0, 0);
    this->setLayout(m_layout);
  }

  RPWidget::~RPWidget()
  {
  }

  
  void RPWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
    m_imageViewer->UpdateSize(QSize(sz.width()-10, sz.height()-2*(mr+ml)));
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

    onUpdateFrame();
  }

  void RPWidget::onUpdateFrame()
  {
    if(m_data.gps.valid)
      m_data.Release();

    m_data = m_rpReaders->GetRPData(m_frame->GetValue());
    m_imageViewer->SetImage(m_data.b8);
  }

  std::vector < QVTKWidget * > RPWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    res.push_back(m_imageViewer.get());
    return res;
  }

}