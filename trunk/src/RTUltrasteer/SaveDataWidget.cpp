#include "SaveDataWidget.h"
#include "RPFileWriter.h"

namespace Nf
{
#define MAX_TIMESTEPS 500
  SaveDataWidget::SaveDataWidget(QWidget *parent, Qt::WFlags flags)
    : ResizableQFrame(parent, QSize(VIS_WIDTH, VIS_HEIGHT))
    , m_memB8(NULL)
    , m_memColor(NULL)
    , m_isRecording(false)
  {
    ui.setupUi(this);

    SetEnabled(false);
    SetProgressBarRange(MAX_TIMESTEPS);
    SetProgressBarValue(0);

    Connect(ui.startRecordingButton, SIGNAL(clicked()), SLOT(onStartRecordingClicked()));
    Connect(ui.saveDataButton, SIGNAL(clicked()), SLOT(onSaveDataClicked()));
  }

  SaveDataWidget::~SaveDataWidget()
  {
  }

  void SaveDataWidget::SetEnabled(bool enabled)
  {
    ui.progressBar->setEnabled(enabled);
    if(!enabled) {
      ui.startRecordingButton->setEnabled(enabled);
      ui.saveDataButton->setEnabled(enabled);
    }
    ui.allocateDataButton->setEnabled(enabled);
  }


  void SaveDataWidget::SetProgressBarValue(s32 value)
  {
    ui.progressBar->setValue(value);
  }

  void SaveDataWidget::SetProgressBarRange(s32 range)
  {
    ui.progressBar->setRange(0, range);
  }

  void SaveDataWidget::onStartRecordingClicked()
  {
    if(m_isRecording) {
      DoneRecording();
    } else {
      ui.startRecordingButton->setText("Stop Recording");
      ui.saveDataButton->setEnabled(false);
      m_isRecording = !m_isRecording;
    }
  }

  void SaveDataWidget::AllocateData(RPData &data)
  {
    //s32 colorFrameSz = data.color->widthStep*data.color->height;
    s32 colorFrameSz = data.b8->widthStep*data.b8->height*4;
    s32 b8FrameSz = data.b8->widthStep*data.b8->height;
    m_memColor = (u8 *)malloc(colorFrameSz*MAX_TIMESTEPS);
    m_memB8 = (u8 *)malloc(b8FrameSz*MAX_TIMESTEPS);

    u8 *currColor = m_memColor;
    u8 * currB8 = m_memB8;

    DataFrame d;
    RPData rp;
    for(s32 i=0; i<MAX_TIMESTEPS; i++) {
      rp.b8 = cvCreateImageHeader(cvSize(data.b8->width, data.b8->height), IPL_DEPTH_8U, 1);
      rp.color = cvCreateImageHeader(cvSize(data.b8->width, data.b8->height), IPL_DEPTH_8U, 4);
      cvSetImageData(rp.b8, currB8, data.b8->widthStep);
      cvSetImageData(rp.color, currColor, data.b8->widthStep*4);
      d.rp = rp;
      m_unusedData.push_back(d);

      currColor += colorFrameSz;
      currB8 += b8FrameSz;
    }
    
    ui.startRecordingButton->setEnabled(true);
    ui.saveDataButton->setEnabled(false);
    ui.allocateDataButton->setEnabled(false);
  }

  void SaveDataWidget::SaveDataFrame(DataFrame &data)
  {
    if(!m_isRecording)
      return;
    DataFrame d = m_unusedData[m_unusedData.size()-1];

    IplImage *b8 = d.rp.b8;
    IplImage *color = d.rp.color;
    d.rp = data.rp;
    cvCopyImage(data.rp.b8, b8);
    if(data.rp.color != NULL)
      cvCopyImage(data.rp.color, color);
    d.rp.b8 = b8;
    d.rp.color = color;
    m_dataToSave.push_back(d);
    m_unusedData.pop_back();

    ui.progressBar->setValue(m_dataToSave.size());
    if(m_unusedData.size() == 0)
      DoneRecording();
  }

  void SaveDataWidget::DoneRecording()
  {
    ui.startRecordingButton->setText("Start Recording");
    m_isRecording = false;
    ui.startRecordingButton->setEnabled(false);
    ui.saveDataButton->setEnabled(true);
    ui.allocateDataButton->setEnabled(false);
  }

  void SaveDataWidget::onSaveDataClicked()
  {
    if(m_isRecording || m_memColor == NULL || m_memB8 == NULL)
      return;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", "*.b8");
    if(fileName.length() <= 0) {
      return;
    }

    std::string fname = fileName.toStdString();
    std::string dir = fname.substr(0,fname.find_last_of("/"));
    std::string name = fname.substr(fname.find_last_of("/")+1,std::string::npos);

    //TODO: Save The Data
    //std::tr1::shared_ptr < RPFileWriterCollection > rpw(new RPFileWriterCollection(dir+name, RPFileHeader));

    //for(s32 i=0; i<m_dataToSave.size(); i++) {

    //}
    ////////////////////////////////////
    
    free(m_memColor); 
    m_memColor = NULL;
    free(m_memB8);
    m_memB8 = NULL;
    ui.allocateDataButton->setEnabled(true);
    ui.saveDataButton->setEnabled(false);
    ui.startRecordingButton->setEnabled(false);
  }

}

