#include "EstimatorWidget.h"
#include "NeedleTipCalibrationPP.h"
#include <QKeyEvent>

namespace Nf
{
#define INSERT_VEL 1 //mm/s

  EstimatorFileWidget::EstimatorFileWidget(QWidget *parent)
    : RPFileWidget(parent, (USVisualizer *)new PFVisualizer(parent))
    , m_state(EFS_READY)
    , m_resultsAvailable(ERA_NONE)
  {
    ADD_ACTION_PARAMETER(m_needleCalib, "Needle Calibration Mode", CALLBACK_POINTER(onNeedleCalibrationPushed, EstimatorFileWidget), this, false);
    ADD_ACTION_PARAMETER(m_doNeedleCalib, "Do Needle Calibration", CALLBACK_POINTER(onDoNeedleCalibrationPushed, EstimatorFileWidget), this, false);
    ADD_SAVE_FILE_PARAMETER(m_tipCalibPath, "Tip Calibration Path", NULL, this, "C:/Joey/Data/8_16_15/TipCalibration/tipCalib.mat", "(*.mat)");

    m_calibrationPoints = std::tr1::shared_ptr < PointCloudVisualizer > (new PointCloudVisualizer(1, Vec3d(0, 1, 0)));
    m_calibTip = std::tr1::shared_ptr < SphereVisualizer > (new SphereVisualizer(Vec3d(0,0,0), 1));
    m_calibTip->SetColor(Vec3d(1,0,0));

    m_calibTipFrame = vtkSmartPointer <vtkAxesActor>::New();
    m_calibTipFrame->SetTotalLength(5,5,5);

    m_planeVis->GetRenderer()->AddActor(m_calibrationPoints->GetActor());
    onUpdateFile();
  }

  EstimatorFileWidget::~EstimatorFileWidget()
  {
  }

  void EstimatorFileWidget::onUpdateFrame()
  {
    RPFileWidget::onUpdateFrame();

    UpdateCalibTipVis();
  }

  void EstimatorFileWidget::onUpdateFile()
  {
    RPFileWidget::onUpdateFile();
    
    bool validFile = m_frame->GetMax() > 0;
    switch(m_state) {
    case EFS_READY: 
    case EFS_NEEDLE_CURVATURE_CALIB_GPS:
    case EFS_NEEDLE_CURVATURE_CALIB_US:
      {
        if(validFile)
          m_state = EFS_PRIMED;
        else
          m_state = EFS_READY;

        m_calibrationPoints->ClearPoints();
        m_ntCalibrator.ClearPoints();
        m_resultsAvailable = ERA_NONE;
        m_planeVis->repaint();
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
        m_state = EFS_NEEDLE_TIP_CALIB;

        vtkSmartPointer < vtkRenderWindowInteractor > interactor = m_imageViewer->GetWindowInteractor();
        vtkSmartPointer < vtkPointPicker > picker = vtkPointPicker::New();
        vtkSmartPointer < NeedleTipCalibrationPP > style = NeedleTipCalibrationPP::New();
        style->SetImageData(m_imageViewer->GetImageData());
        style->SetEstimatorWidget(this);
        interactor->SetPicker(picker);
        interactor->SetInteractorStyle(style);

        m_ntCalibrator.ClearPoints();
        m_calibrationPoints->ClearPoints();
        m_resultsAvailable = (m_resultsAvailable&~ERA_NEEDLE_TIP_CALIB);
        break;
      }
    case EFS_NEEDLE_TIP_CALIB: 
      {
        m_state = EFS_PRIMED;
        m_ntCalibrator.ClearPoints();
        m_calibrationPoints->ClearPoints();
        m_planeVis->repaint();
        m_resultsAvailable = (m_resultsAvailable&~ERA_NEEDLE_TIP_CALIB);
        break;
      }
    case EFS_NEEDLE_CURVATURE_CALIB_GPS:
    case EFS_NEEDLE_CURVATURE_CALIB_US:
      {
        m_resultsAvailable = (m_resultsAvailable&~ERA_NEEDLE_CURVATURE_CALIB);
        break;
      }
    default: 
      {
        throw std::runtime_error("EstimatorFileWidget: unknown state\n");
        break;
      }
    }
  }

  void EstimatorFileWidget::onDoNeedleCalibrationPushed()
  {
    switch(m_state)
    {
    case EFS_READY: 
    case EFS_PRIMED:
      {
        break;
      }
    case EFS_NEEDLE_TIP_CALIB: 
      {
        m_calibrationPoints->ClearPoints();
        m_ntCalibrator.DoCalibration();
        m_resultsAvailable = m_resultsAvailable|ERA_NEEDLE_TIP_CALIBRATED;
        UpdateCalibTipVis();

        Vec3d tipOffset; Matrix33d tipFrame;
        m_ntCalibrator.GetSolution(tipOffset, tipFrame);

        arma::mat total = tipFrame.ToArmaMat();
        total = arma::join_vert(total, tipOffset.ToArmaVec().t());
        total.save(m_tipCalibPath->GetValue(), arma::raw_ascii);

        break;
      }
    case EFS_NEEDLE_CURVATURE_CALIB_GPS:
    case EFS_NEEDLE_CURVATURE_CALIB_US:
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

  void EstimatorFileWidget::onPointPushed(Vec2d point)
  {
    Vec2d mppScale(m_data.mpp.x/1000.0, m_data.mpp.y/1000.0);
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
    case EFS_NEEDLE_TIP_CALIB:
      {
        Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(m_data.gps.pose).GetOrientation(), m_data.gps.pos);
        Vec3d y = rpImageCoordToWorldCoord3(point, posePos, m_cal, m_data.origin, mppScale);
        m_calibrationPoints->AddPoint(y);
        m_planeVis->repaint();
        m_ntCalibrator.AddPoint(m_data.gps2.pos, Matrix44d::FromCvMat(m_data.gps2.pose).GetOrientation(), y);
      }
      break;
    case EFS_NEEDLE_CURVATURE_CALIB_GPS:
      {
        break;
      }
    case EFS_NEEDLE_CURVATURE_CALIB_US:
      {
        Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(m_data.gps.pose).GetOrientation(), m_data.gps.pos);
        Vec3d y = rpImageCoordToWorldCoord3(point, posePos, m_cal, m_data.origin, mppScale);
        m_calibrationPoints->AddPoint(y);
        m_planeVis->repaint();
        break;
      }
    default: 
      {
        throw std::runtime_error("EstimatorFileWidget: unknown state\n");
        break;
      }
    }
  }

  void EstimatorFileWidget::UpdateCalibTipVis()
  {
    Vec2d mppScale(m_data.mpp.x/1000.0, m_data.mpp.y/1000.0);

    if(m_resultsAvailable&ERA_NEEDLE_TIP_CALIB) {
        if(!m_planeVis->GetRenderer()->HasViewProp(m_calibTip->GetActor()))
          m_planeVis->GetRenderer()->AddActor(m_calibTip->GetActor());
        if(!m_planeVis->GetRenderer()->HasViewProp(m_calibTipFrame))
          m_planeVis->GetRenderer()->AddActor(m_calibTipFrame);
        vtkSmartPointer < vtkRenderer > renderer = m_usVis->GetRenderer();
        if(renderer != NULL) {
          if(!m_usVis->GetRenderer()->HasViewProp(m_calibTip->GetActor()))
            m_usVis->GetRenderer()->AddActor(m_calibTip->GetActor());
          if(!m_usVis->GetRenderer()->HasViewProp(m_calibTipFrame))
            m_usVis->GetRenderer()->AddActor(m_calibTipFrame);
        }

        Matrix33d tipFrame = Matrix44d::FromCvMat(m_data.gps2.pose).GetOrientation();
        Vec3d tipPosCalib; Matrix33d tipFrameCalib; m_ntCalibrator.GetSolution(tipPosCalib, tipFrameCalib, m_data.gps2.pos, tipFrame);
        m_calibTip->SetCenter(tipPosCalib);
        m_calibTipFrame->PokeMatrix(Matrix44d::FromOrientationAndTranslation(tipFrameCalib, tipPosCalib).GetVTKMatrix());
        m_planeVis->repaint();
    } else {
      if(m_planeVis->GetRenderer()->HasViewProp(m_calibTip->GetActor()))
        m_planeVis->GetRenderer()->RemoveActor(m_calibTip->GetActor());
      if(m_planeVis->GetRenderer()->HasViewProp(m_calibTipFrame))
        m_planeVis->GetRenderer()->RemoveActor(m_calibTipFrame);
      vtkSmartPointer < vtkRenderer > renderer = m_usVis->GetRenderer();
      if(renderer != NULL) {
        if(m_usVis->GetRenderer()->HasViewProp(m_calibTip->GetActor()))
          m_usVis->GetRenderer()->RemoveActor(m_calibTip->GetActor());
        if(m_usVis->GetRenderer()->HasViewProp(m_calibTipFrame))
          m_usVis->GetRenderer()->RemoveActor(m_calibTipFrame);
      }
    } 

    if(m_resultsAvailable&ERA_NEEDLE_CURVATURE_CALIB) {
    } else {
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
    
    m_u.v = 0;
    m_u.tick = 0;
    m_u.dtheta = 0;
    m_u.dutyCycle = 0;

    ADD_ACTION_PARAMETER(m_insertion, "[Stop] Insert", CALLBACK_POINTER(onInsertionPushed, EstimatorStreamingWidget), this, false);
    ADD_ACTION_PARAMETER(m_pauseInsertion, "[Un]Pause Insert", CALLBACK_POINTER(onPauseInsertionPushed, EstimatorStreamingWidget), this, false);
    ADD_ACTION_PARAMETER(m_saveGPS, "Save GPS", CALLBACK_POINTER(onSaveGPS, EstimatorStreamingWidget), this, false);
    ADD_OPEN_FILE_PARAMETER(m_tipCalibPath, "Tip Calibration Path", CALLBACK_POINTER(onTipCalibPathChanged, EstimatorStreamingWidget), this, "C:/Joey/Data/8_16_15/TipCalibration/tipCalib.mat", "(*.mat)");

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

  void EstimatorStreamingWidget::onTipCalibPathChanged()
  {
    arma::mat soln;
    soln.load(m_tipCalibPath->GetValue());

    m_ntCalibrator.SetSolution(soln);
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
      m_hwWidget->GetRCWidget()->InsertPosVel(INSERT_VEL);
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
        d.u.v = 0;
        m_saveDataWidget->SaveDataFrame(d);
        m_state = ES_RECORDING2;
        break;
      }
    case ES_RECORDING2: 
      {
        if(m_executeCommand)
          ExecuteCommand();
        m_saveDataWidget->SaveDataFrame(d);
        break;
      }
    case ES_PAUSED:
      {
        d.u.v = 0;
        // how we signal that this is a valid gps reading
        d.rp.gps2.offset[0] = 12345.0;
        m_saveDataWidget->SaveDataFrame(d);
        m_saveDataWidget->StopRecording();
        break;
      }
    default:
      {
        m_saveDataWidget->SaveDataFrame(d);
        break;
      }
    }

    Vec3d tipOffset; Matrix33d tipFrame;
    m_ntCalibrator.GetSolution(tipOffset, tipFrame, rp.gps2.pos, Matrix44d::FromCvMat(rp.gps2.pose).GetOrientation());
    rp.gps2.pos = tipOffset;
    Vec3d gps2Offset = Matrix44d::FromCvMat(rp.gps2.pose).GetPosition();
    rp.gps2.pose = Matrix44d::FromOrientationAndTranslation(tipFrame, gps2Offset).ToCvMat();

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
      break;
    case ES_RECORDING1:
    case ES_RECORDING2:
    case ES_PAUSED:
      m_hwWidget->GetRCWidget()->StopInsertion();
      m_u.v = 0;
      m_state = ES_PRIMED;
      m_saveDataWidget->StopRecording();
      break;
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
      m_saveDataWidget->ResumeRecording();
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
      m_saveDataWidget->ResumeRecording();
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