#include "SaveDataWidget.h"
#include "RPFileWriter.h"

namespace Nf
{
#define MAX_TIMESTEPS 4000
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

  void SaveDataWidget::onSaveDataClicked()
  {
    RPFileHeader header = {0};
    SaveData(header);
  }

  bool SaveDataWidget::HasData() const
  {
    return m_dataToSave.size() > 0;
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

  void SaveDataWidget::SaveData(RPFileHeader header, const char *filename)
  {
    if(m_dataToSave.size() <= 0)
      return;
    std::string fname(filename);
    std::string dir = fname.substr(0,fname.find_last_of("/"));
    std::string name = fname.substr(fname.find_last_of("/")+1,std::string::npos);
    name = name.substr(0, name.find_last_of("."));

    //TODO: Save The Data
    std::tr1::shared_ptr < RPFileWriterCollection > rpw(new RPFileWriterCollection(std::string(dir+std::string("/")+name).c_str(), &header));
    if(m_dataToSave.size() > 0 && m_dataToSave.front().b8 != NULL)
      rpw->AddWriter(RPF_BPOST8);
    if(m_dataToSave.size() > 0 && m_dataToSave.front().color != NULL)
      rpw->AddWriter(RPF_BPOST32);
    rpw->AddGPSWriter();
    rpw->AddGPSWriter2();
    rpw->AddNSCommandWriter();
    rpw->AddForceWriter();

    header.frames = m_dataToSave.size();
    header.sf = (s32)(m_dataToSave[0].mpp.y/m_dataToSave[0].mpp.x*NOMINAL_SOS+0.5);
    header.dr = (s32)(m_dataToSave[0].mpp.x+0.5);
    header.ld = (s32)(m_dataToSave[0].origin.x+0.5);
    header.extra = (s32)(m_dataToSave[0].origin.y+0.5);

    for(s32 i=0; i<m_dataToSave.size(); i++) {
      rpw->WriteNextRPData(&m_dataToSave[i]);
    }

    rpw->Cleanup(&header);
    FreeData();
    ui.progressBar->setValue(0);
  }

  void SaveDataWidget::SaveDataWithSnap(RPFileHeader header, const char *filename, RPData &snap)
  {
    if(m_dataToSave.size() <= 0)
      return;
    std::string fname(filename);
    std::string dir = fname.substr(0,fname.find_last_of("/"));
    std::string name = fname.substr(fname.find_last_of("/")+1,std::string::npos);
    name = name.substr(0, name.find_last_of("."));

    //TODO: Save The Data
    std::tr1::shared_ptr < RPFileWriterCollection > rpw(new RPFileWriterCollection(std::string(dir+std::string("/")+name).c_str(), &header));
    if(m_dataToSave.size() > 0 && m_dataToSave.front().b8 != NULL)
      rpw->AddWriter(RPF_BPOST8);
    if(m_dataToSave.size() > 0 && m_dataToSave.front().color != NULL)
      rpw->AddWriter(RPF_BPOST32);
    rpw->AddGPSWriter();
    rpw->AddGPSWriter2();
    rpw->AddNSCommandWriter();
    rpw->AddForceWriter();

    header.frames = m_dataToSave.size();
    header.sf = (s32)(m_dataToSave[0].mpp.y/m_dataToSave[0].mpp.x*NOMINAL_SOS+0.5);
    header.dr = (s32)(m_dataToSave[0].mpp.x+0.5);
    header.ld = (s32)(m_dataToSave[0].origin.x+0.5);
    header.extra = (s32)(m_dataToSave[0].origin.y+0.5);

    for(s32 i=0; i<m_dataToSave.size(); i++) {
      rpw->WriteNextRPData(&m_dataToSave[i]);
    }

    arma::mat force;
    arma::mat pos;

    Vec2d mppScale(snap.mpp.x/1000.0, snap.mpp.y/1000.0);
    Matrix44d tPose = Matrix44d::FromCvMat(snap.gps.pose);
    Matrix33d pose = tPose.GetOrientation();
    Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(pose, snap.gps.pos);
    for(s32 i=0; i<m_dataToSave.size(); i++) {
      arma::mat row = arma::zeros(1,6);
      row(0,0) = m_dataToSave[i].force.force.x; 
      row(0,1) = m_dataToSave[i].force.force.y;
      row(0,2) = m_dataToSave[i].force.force.z;
      row(0,3) = m_dataToSave[i].force.torque.x; 
      row(0,4) = m_dataToSave[i].force.torque.y;
      row(0,5) = m_dataToSave[i].force.torque.z;
      force = arma::join_vert(force, row);

      Vec3d image = rpWorldCoord3ToImageCoord(m_dataToSave[i].gps2.pos, posePos, Matrix44d(TRANSDUCER_CALIBRATION_COEFFICIENTS), snap.origin, mppScale);
      row = arma::zeros(1,6);
      row(0,0) = image.x;
      row(0,1) = image.y;
      row(0,2) = image.z;
      row(0,3) = m_dataToSave[i].gps2.pos.x;
      row(0,4) = m_dataToSave[i].gps2.pos.y;
      row(0,5) = m_dataToSave[i].gps2.pos.z;
      pos = arma::join_vert(pos, row);
    }

    force.save(std::string(dir+std::string("/force.mat")).c_str(), arma::raw_ascii);
    pos.save(std::string(dir+std::string("/pos.mat")).c_str(), arma::raw_ascii);

    rpw->Cleanup(&header);
    FreeData();
    ui.progressBar->setValue(0);
  }

  void SaveDataWidget::SaveData(RPFileHeader header)
  {

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString("C:/Joey/Data/7_27_15"), "*.b8");
    if(fileName.length() <= 0) {
      return;
    }

    SaveData(header, fileName.toStdString().c_str());
  }

}

