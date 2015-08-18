#include "EstimatorWidget.h"
#include "NeedleTipCalibrationPP.h"
#include <QKeyEvent>

namespace Nf
{
#define INSERT_VEL 2 //mm/s

  EstimatorFileWidget::EstimatorFileWidget(QWidget *parent)
    : RPFileWidget(parent, (USVisualizer *)new PFVisualizer(parent))
    , m_state(EFS_READY)
  {
    ADD_ACTION_PARAMETER(m_needleCalib, "Needle Calibration Mode", CALLBACK_POINTER(onNeedleCalibrationPushed, EstimatorFileWidget), this, false);
    onUpdateFile();
  }

  EstimatorFileWidget::~EstimatorFileWidget()
  {
  }

  void EstimatorFileWidget::onUpdateFile()
  {
    RPFileWidget::onUpdateFile();
    
    bool validFile = m_frame->GetMax() > 0;
    switch(m_state) {
    case EFS_READY: 
      {
        if(validFile)
          m_state = EFS_PRIMED;
        break;
      }
    case EFS_PRIMED: 
      {
        break;
      }
    case EFS_NEEDLE_CALIB:
      {
        break;
      }
    default: 
      {
        throw std::runtime_error("EstimatorFileWidget: unknown state\n");
        break;
      }
    }
  }

  void EstimatorFileWidget::onNeedleCalibrationPushed()
  {
    switch(m_state)
    {
    case EFS_READY: 
      {
        break;
      }
    case EFS_PRIMED:
      {
        m_state = EFS_NEEDLE_CALIB;

        vtkSmartPointer < vtkRenderWindowInteractor > interactor = m_imageViewer->GetWindowInteractor();
        vtkSmartPointer < vtkPointPicker > picker = vtkPointPicker::New();
        vtkSmartPointer < NeedleTipCalibrationPP > style = NeedleTipCalibrationPP::New();
        style->SetImageData(m_imageViewer->GetImageData());
        interactor->SetPicker(picker);
        interactor->SetInteractorStyle(style);
        break;
      }
    case EFS_NEEDLE_CALIB: 
      {
        m_state = EFS_READY;
        break;
      }
    default: 
      {
        throw std::runtime_error("EstimatorFileWidget: unknown state\n");
        break;
      }
    }
  }

  void EstimatorFileWidget::onPointPushed(f64 point[])
  {
    switch(m_state)
    {
    case EFS_READY:
      {
        break;
      }
    case EFS_PRIMED:
      {
        break;
      }
    case EFS_NEEDLE_CALIB:
      {
        break;
      }
    default: 
      {
        throw std::runtime_error("EstimatorFileWidget: unknown state\n");
        break;
      }
    }
  }

  EstimatorStreamingWidget::EstimatorStreamingWidget(QWidget *parent)
    : RPStreamingWidget(parent, (USVisualizer *)new PFVisualizer(parent))
    , m_state(ES_READY)
    , m_hwWidget(new RobotHardwareWidget(parent))
    , m_saveDataWidget(new SaveDataWidget(parent))
    , m_bottomRow(new QGridLayout(parent))
  {
    m_bottomRow->addWidget(m_hwWidget.get(), 0, 0);
    m_bottomRow->addWidget(m_saveDataWidget.get(), 0, 1, Qt::Alignment(Qt::AlignTop));
    m_layout->addLayout(m_bottomRow.get(), 1, 0, 1, 2);

    m_tipOffset = Vec3d(0, 0, 0);
    
    m_u.v = 0;
    m_u.tick = 0;
    m_u.dtheta = 0;
    m_u.dutyCycle = 0;

    ADD_ACTION_PARAMETER(m_insertion, "[Stop] Insert", CALLBACK_POINTER(onInsertionPushed, EstimatorStreamingWidget), this, false);
    ADD_ACTION_PARAMETER(m_pauseInsertion, "[Un]Pause Insert", CALLBACK_POINTER(onPauseInsertionPushed, EstimatorStreamingWidget), this, false);
    ADD_ACTION_PARAMETER(m_saveGPS, "Save GPS", CALLBACK_POINTER(onSaveGPS, EstimatorStreamingWidget), this, false);
    ADD_CHILD_COLLECTION(m_hwWidget.get());

    Connect(m_saveDataWidget->ui.saveDataButton, SIGNAL(clicked()), SLOT(onSaveDataClicked()));
  }

  void EstimatorStreamingWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
    m_imageViewer->UpdateSize(QSize(sz.width()/2-10, (3*sz.height())/4));
    m_usVis->UpdateSize(QSize(sz.width()/2-10, (3*sz.height())/4));
    m_planeVis->UpdateSize(QSize(sz.width()/2-10, (3*sz.height())/4));
    m_hwWidget->UpdateSize(QSize(3*sz.width()/4-10, (sz.height())/4));
    m_saveDataWidget->UpdateSize(QSize(sz.width()/4-10, (sz.height())/4));
  }

  void EstimatorStreamingWidget::onInitializeToggle()
  {
    RPStreamingWidget::onInitializeToggle();
    m_saveDataWidget->SetEnabled(m_init->GetValue());
    switch(m_state) {
    case ES_READY:
    case ES_PRIMED:
      m_state = ES_PRIMED;
      break;
    case ES_RECORDING1:
    case ES_RECORDING2:
    case ES_PAUSED:
    default:
      break;
    }
  }

  static RPFileHeader RPFileHeader_uDataDesc(const uDataDesc &desc)
  {
    RPFileHeader res;  memset(&res, 0, sizeof(RPFileHeader));
    res.ul[0] = desc.roi.ulx;
    res.ul[1] = desc.roi.uly;
    res.bl[0] = desc.roi.blx;
    res.bl[1] = desc.roi.bly;
    res.br[0] = desc.roi.brx;
    res.br[1] = desc.roi.bry;
    res.ur[0] = desc.roi.urx;
    res.ur[1] = desc.roi.ury;
    res.width = desc.w;
    res.height = desc.h;
    res.frames = 0;
    return res;
  }

  void EstimatorStreamingWidget::onSaveDataClicked()
  {
    //TODO CHANGE THSI

    RPFileHeader header = RPFileHeader_uDataDesc(m_rpReaders->GetImageDesc(RPF_COLOR));
    m_saveDataWidget->SaveData(header);

  }

  void EstimatorStreamingWidget::ExecuteCommand()
  {
    if(ABS(m_u.v - INSERT_VEL) < 1e-6)
      m_hwWidget->GetRCWidget()->InsertPosVel();
    else
      m_hwWidget->GetRCWidget()->StopInsertion();
    m_executeCommand = false;
  }

  void EstimatorStreamingWidget::onFrame()
  {
    if(!m_init->GetValue() || !m_lock.tryLock(30))
      return;
    
    RPData rp = m_data.Clone();
    
    DataFrame d;
    d.rp = rp;
    if(d.rp.b8 != NULL)
      m_u.tick = (u32)d.rp.b8->BorderMode[0];
    d.u = m_u;
    switch(m_state) {
    case ES_READY:
    case ES_PRIMED: 
      {
        m_saveDataWidget->SaveDataFrame(d);
        break;
      }
    case ES_RECORDING1: 
      {
        if(m_executeCommand)
          ExecuteCommand();
        break;
        d.u.v = 0;
        m_saveDataWidget->SaveDataFrame(d);
        m_state = ES_RECORDING2;
      }
    case ES_RECORDING2: 
      {
        if(m_executeCommand)
          ExecuteCommand();
        break;
        m_saveDataWidget->SaveDataFrame(d);
      }
    case ES_PAUSED:
      {
        d.u.v = 0;
        // how we signal that this is a valid gps reading
        d.rp.gps2.offset[0] = 12345.0;
        m_saveDataWidget->SaveDataFrame(d);
        m_saveDataWidget->StopRecording();
      }
    default:
      {
        m_saveDataWidget->SaveDataFrame(d);
        break;
      }
    }
    
    cv::Mat pose = rp.gps2.pose.t();
    rp.gps2.pos = rp.gps2.pos+Vec3d(pose.at<f64>(0, 0), pose.at<f64>(0, 1), pose.at<f64>(0, 2))*m_tipOffset.x
      +Vec3d(pose.at<f64>(1, 0), pose.at<f64>(1, 1), pose.at<f64>(1, 2))*m_tipOffset.y+Vec3d(pose.at<f64>(2, 0), pose.at<f64>(2, 1), pose.at<f64>(2, 2))*m_tipOffset.z;
    HandleFrame(rp);
    rp.Release();

  }

  void EstimatorStreamingWidget::onInsertionPushed()
  {
    switch(m_state) {
    case ES_READY:
      m_state = ES_READY;
      break;
    case ES_PRIMED:
      m_executeCommand = true;
      m_u.v = INSERT_VEL;
      m_saveDataWidget->StartRecording();
      m_state = ES_RECORDING1;
    case ES_RECORDING1:
    case ES_RECORDING2:
    case ES_PAUSED:
      m_hwWidget->GetRCWidget()->StopInsertion();
      m_u.v = 0;
      m_state = ES_PRIMED;
      m_saveDataWidget->StopRecording();
    default:
      break;
    }
  }


  void EstimatorStreamingWidget::onPauseInsertionPushed()
  {
    switch(m_state) {
    case ES_READY:
    case ES_PRIMED:
      break;
    case ES_RECORDING1:
    case ES_RECORDING2:
      m_hwWidget->GetRCWidget()->StopInsertion();
      m_saveDataWidget->StopRecording();
      m_state = ES_PAUSED;
      break;
    case ES_PAUSED:
      m_executeCommand = true;
      m_u.v = INSERT_VEL;
      m_state = ES_RECORDING1;
      m_saveDataWidget->StartRecording();
      break;
    default:
      break;
    }
  }


  void EstimatorStreamingWidget::onSaveGPS()
  {
    switch(m_state) {
    case ES_READY:
    case ES_PRIMED:
    case ES_RECORDING1:
    case ES_RECORDING2:
      break;
    case ES_PAUSED:
      //if we're paused and saveGPS is called, then we want to save a dataframe worth of GPS then stop recording.
      //stop recording is set after a frame's worth of data is received
      m_saveDataWidget->StartRecording();
      break;
    default:
      break;
    }
  }

  void EstimatorStreamingWidget::SetRobot(NeedleSteeringRobot *robot)
  {
    m_robot = robot;
    m_hwWidget->setRobot(robot);
  }

  EstimatorStreamingWidget::~EstimatorStreamingWidget()
  {
  }
}