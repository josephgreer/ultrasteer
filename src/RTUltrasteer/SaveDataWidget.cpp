#include "SaveDataWidget.h"
#include "RPFileWriter.h"

namespace Nf
{
#define MAX_TIMESTEPS 800
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
    } else {
      ui.saveDataButton->setEnabled(enabled);
      ui.startRecordingButton->setEnabled(enabled);
    }
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
      m_isRecording = !m_isRecording;
    }
  }

  void SaveDataWidget::StartRecording()
  {
    FreeData();
    m_isRecording = true;
    ui.startRecordingButton->setText("Stop Recording");
  }

  void SaveDataWidget::FreeData()
  {
    for(s32 i=0; i<m_dataToSave.size(); i++) {
      m_dataToSave[i].Release();
    }

    m_dataToSave.clear();
  }

  void SaveDataWidget::StopRecording()
  {
    m_isRecording = false;
    ui.startRecordingButton->setText("Start Recording");
  }

  void SaveDataWidget::ResumeRecording()
  {
    m_isRecording = false;
    ui.startRecordingButton->setText("Stop Recording");
  }

  void SaveDataWidget::SaveDataFrame(const RPData &data)
  {
    if(!m_isRecording)
      return;
    RPData d = data.Clone();
    m_dataToSave.push_back(d);

    ui.progressBar->setValue(m_dataToSave.size());
    if(m_dataToSave.size() == MAX_TIMESTEPS)
      DoneRecording();
  }

  void SaveDataWidget::DoneRecording()
  {
    ui.startRecordingButton->setText("Start Recording");
    m_isRecording = false;
  }

  void SaveDataWidget::SaveData(RPFileHeader header)
  {

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString("C:/Joey/Data/7_27_15"), "*.b8");
    if(fileName.length() <= 0) {
      return;
    }

    std::string fname = fileName.toStdString();
    std::string dir = fname.substr(0,fname.find_last_of("/"));
    std::string name = fname.substr(fname.find_last_of("/")+1,std::string::npos);
    name = name.substr(0, name.find_last_of("."));

    //TODO: Save The Data
    std::tr1::shared_ptr < RPFileWriterCollection > rpw(new RPFileWriterCollection(std::string(dir+std::string("/")+name).c_str(), &header));
    rpw->AddWriter(RPF_BPOST8);
    if(m_dataToSave.size() > 0 && m_dataToSave.front().color != NULL)
      rpw->AddWriter(RPF_BPOST32);
    rpw->AddGPSWriter();
    rpw->AddGPSWriter2();
    rpw->AddNSCommandWriter();

    header.frames = m_dataToSave.size();
    header.sf = (s32)(m_dataToSave[0].mpp.y/m_dataToSave[0].mpp.x*NOMINAL_SOS+0.5);
    header.dr = (s32)(m_dataToSave[0].mpp.x+0.5);
    header.ld = (s32)(m_dataToSave[0].origin.x+0.5);
    header.extra = (s32)(m_dataToSave[0].origin.y+0.5);

    rpw->Cleanup(&header);
    FreeData();
    ui.progressBar->setValue(0);
  }

}

