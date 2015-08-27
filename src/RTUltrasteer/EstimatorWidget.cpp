#include "EstimatorWidget.h"
#include "NeedleTipCalibrationPP.h"
#include <QKeyEvent>

namespace Nf
{
#define INSERT_VEL 1 //mm/s

  static void SpoofRPDataWithNeedleTipCalibration(RPData &rp, const EMNeedleTipCalibrator *em)
  {
    Vec3d tipOffset; Matrix33d tipFrame;
    em->GetSolution(tipOffset, tipFrame, rp.gps2.pos, Matrix44d::FromCvMat(rp.gps2.pose).GetOrientation());
    rp.gps2.pos = tipOffset;
    Vec3d gps2Offset = Matrix44d::FromCvMat(rp.gps2.pose).GetPosition();
    rp.gps2.pose = Matrix44d::FromOrientationAndTranslation(tipFrame, gps2Offset).ToCvMat();
  }

  EstimatorFileWidget::EstimatorFileWidget(QWidget *parent)
    : RPFileWidget(parent, (USVisualizer *)new PFVisualizer(parent))
    , m_state(EFS_READY)
    , m_resultsAvailable(ERA_NONE)
  {
    ADD_ACTION_PARAMETER(m_doNeedleCalib, "Do Needle Calibration", CALLBACK_POINTER(onDoNeedleCalibrationPushed, EstimatorFileWidget), this, false);
    ADD_ENUM_PARAMETER(m_calibMode, "Calibration Mode", CALLBACK_POINTER(onSetCalibMode, EstimatorFileWidget), this, QtEnums::EstimatorCalibrationModes::ECM_NONE, "EstimatorCalibrationModes");
    ADD_SAVE_FILE_PARAMETER(m_tipCalibPath, "Tip Calibration Save Path", NULL, this, "C:/Joey/Data/TipCalibration/tipCalib.mat", "(*.mat)");
    ADD_OPEN_FILE_PARAMETER(m_tipCalibPathLoad, "Presaved Tip Calibration",CALLBACK_POINTER(onTipCalibPathChanged, EstimatorFileWidget), this, "C:/Joey/Data/TipCalibration/tipCalib.mat", "(*.mat)");
    ADD_SAVE_FILE_PARAMETER(m_pointsDataPath, "Point History Save Path", NULL, this, "C:/Joey/Data/TipCalibration/tipHistory.mat", "(*.mat)");
    ADD_OPEN_FILE_PARAMETER(m_pointsDataPathLoad, "Presaved Point History", CALLBACK_POINTER(onPointsDataPathChanged, EstimatorFileWidget), this, "C:/Joey/Data/TipCalibration/tipHistory.mat", "(*.mat)");
    ADD_ACTION_PARAMETER(m_clearCalibrationData, "Clear Calibration Data", CALLBACK_POINTER(onClearCalibrationData, EstimatorFileWidget), this, false);
    ADD_ACTION_PARAMETER(m_clearTipCalibration, "Clear Tip Calibration", CALLBACK_POINTER(onClearTipCalibration, EstimatorFileWidget), this, false);

    m_calibrationPointsTip = std::tr1::shared_ptr < PointCloudVisualizer > (new PointCloudVisualizer(1, Vec3d(0, 1, 0)));
    m_calibrationPointsCurvature = std::tr1::shared_ptr < PointCloudVisualizer > (new PointCloudVisualizer(1, Vec3d(1, 1, 0)));

    m_calibTip = std::tr1::shared_ptr < SphereVisualizer > (new SphereVisualizer(Vec3d(0,0,0), 1));
    m_calibTip->SetColor(Vec3d(1,0,0));

    m_calibTipFrame = vtkSmartPointer <vtkAxesActor>::New();
    m_calibTipFrame->SetTotalLength(5,5,5);

    m_planeVis->GetRenderer()->AddActor(m_calibrationPointsTip->GetActor());
    m_planeVis->GetRenderer()->AddActor(m_calibrationPointsCurvature->GetActor());
    onUpdateFile();
  }

  EstimatorFileWidget::~EstimatorFileWidget()
  {
  }

  void EstimatorFileWidget::onUpdateFrame()
  {
    if(m_data.gps.valid)
      m_data.Release();

    if(m_rpReaders)
      m_data = m_rpReaders->GetRPData(m_frame->GetValue());

    if(m_resultsAvailable&ERA_NEEDLE_TIP_CALIB)
      SpoofRPDataWithNeedleTipCalibration(m_data, &m_ntCalibrator);
    
    switch(m_state) {
    case EFS_READY: 
    case EFS_PRIMED:
      {
        break;
      }
    case EFS_NEEDLE_TIP_CALIB:
      {
        break;
      }
    case EFS_NEEDLE_CURVATURE_CALIB_GPS:
      {
        m_ncCalibrator.AddPoint(m_data.gps2.pos);
        m_calibrationPointsCurvature->AddPoint(m_data.gps2.pos);
        break;
      }
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
    
    UpdateCalibTipVis();

    m_imageViewer->SetImage(&m_data, (RP_TYPE)m_displayMode->GetValue());
    if(m_visTab->currentIndex() == 0)
      m_usVis->AddRPData(m_data);
    else
      m_planeVis->SetImage(&m_data, (RP_TYPE)m_displayMode->GetValue());
  }

  void EstimatorFileWidget::onUpdateFile()
  {
    RPFileWidget::onUpdateFile();
    
    bool validFile = m_frame->GetMax() > 0;
    switch(m_state) {
    case EFS_READY: 
    case EFS_PRIMED:
    case EFS_NEEDLE_TIP_CALIB:
    case EFS_NEEDLE_CURVATURE_CALIB_GPS:
    case EFS_NEEDLE_CURVATURE_CALIB_US:
      {
        if(validFile)
          m_state = EFS_PRIMED;
        else
          m_state = EFS_READY;

        onClearCalibrationData();

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

  void EstimatorFileWidget::onClearCalibrationData()
  {
    m_ncCalibrator.ClearPoints();
    m_calibrationPointsCurvature->ClearPoints();
    m_ncCalibrator.ResetSolution();
    m_ntCalibrator.ClearPoints();
    m_calibrationPointsTip->ClearPoints();
    m_ntCalibrator.ResetSolution();

    onUpdateFrame();

    m_resultsAvailable = m_resultsAvailable&~ERA_NEEDLE_TIP_CALIB;
    m_resultsAvailable = m_resultsAvailable&~ERA_NEEDLE_CURVATURE_CALIB;

    m_planeVis->repaint();
    m_usVis->repaint();
  }
  
  void EstimatorFileWidget::onClearTipCalibration()
  {
    m_ntCalibrator.ResetSolution();
    m_resultsAvailable = m_resultsAvailable&~ERA_NEEDLE_TIP_CALIB;
    onUpdateFrame();

    m_planeVis->repaint();
    m_usVis->repaint();
  }

  void EstimatorFileWidget::onSetCalibMode()
  {
    switch(m_state)
    {
    case EFS_READY: 
      {
        break;
      }
    case EFS_PRIMED:
    case EFS_NEEDLE_CURVATURE_CALIB_GPS:
    case EFS_NEEDLE_CURVATURE_CALIB_US:
      {
        m_state = EFS_NEEDLE_TIP_CALIB;

        m_planeVis->repaint();

        switch(m_calibMode->GetValue()) 
        {
        case QtEnums::EstimatorCalibrationModes::ECM_NONE: 
          {
            m_state = EFS_PRIMED;
            break;
          }
        case QtEnums::EstimatorCalibrationModes::ECM_NEEDLE_TIP:
          {
            vtkSmartPointer < vtkRenderWindowInteractor > interactor = m_imageViewer->GetWindowInteractor();
            vtkSmartPointer < vtkPointPicker > picker = vtkPointPicker::New();
            vtkSmartPointer < NeedleTipCalibrationPP > style = NeedleTipCalibrationPP::New();
            style->SetImageData(m_imageViewer->GetImageData());
            style->SetEstimatorWidget(this);
            interactor->SetPicker(picker);
            interactor->SetInteractorStyle(style);

            m_state = EFS_NEEDLE_TIP_CALIB;
            break;
          }
        case QtEnums::EstimatorCalibrationModes::ECM_CURVATURE_GPS:
          {
            m_state = EFS_NEEDLE_CURVATURE_CALIB_GPS;
            break;
          }
        case QtEnums::EstimatorCalibrationModes::ECM_CURVATURE_US:
          {
            vtkSmartPointer < vtkRenderWindowInteractor > interactor = m_imageViewer->GetWindowInteractor();
            vtkSmartPointer < vtkPointPicker > picker = vtkPointPicker::New();
            vtkSmartPointer < NeedleTipCalibrationPP > style = NeedleTipCalibrationPP::New();
            style->SetImageData(m_imageViewer->GetImageData());
            style->SetEstimatorWidget(this);
            interactor->SetPicker(picker);
            interactor->SetInteractorStyle(style);

            m_state = EFS_NEEDLE_CURVATURE_CALIB_US;
            break;
          }
        default:
          {
            throw std::runtime_error("EstimatorFileWidget::onSetCalibMode() Unknown calibration mode\n");
          }
        }
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
        m_calibrationPointsTip->ClearPoints();
        m_ntCalibrator.DoCalibration();
        m_resultsAvailable = m_resultsAvailable|ERA_NEEDLE_TIP_CALIB;
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
        m_calibrationPointsCurvature->SavePoints(m_pointsDataPath->GetValue().c_str());
        m_ncCalibrator.DoCalibration();
        m_resultsAvailable = m_resultsAvailable|ERA_NEEDLE_CURVATURE_CALIB;
        UpdateCalibTipVis();
        break;
      }
    default: 
      {
        throw std::runtime_error("EstimatorFileWidget: unknown state\n");
        break;
      }
    }
  }

  void EstimatorFileWidget::onTipCalibPathChanged()
  {
    arma::mat soln;
    soln.load(m_tipCalibPathLoad->GetValue());

    m_ntCalibrator.SetSolution(soln);

    m_resultsAvailable = m_resultsAvailable|ERA_NEEDLE_TIP_CALIB;
    onUpdateFrame();
    m_planeVis->repaint();
  }

  void EstimatorFileWidget::onPointsDataPathChanged()
  {
    arma::mat points;
    points.load(m_pointsDataPathLoad->GetValue());

    arma::rowvec curr(3);
    for(s32 i=0; i<points.n_rows; i++) {
      Vec3d thisOne(points(i,0), points(i,1), points(i,2));
      m_calibrationPointsCurvature->AddPoint(thisOne);
      m_ncCalibrator.AddPoint(thisOne);
    }
    onUpdateFrame();
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
        m_calibrationPointsTip->AddPoint(y);
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
        m_calibrationPointsCurvature->AddPoint(y);
        m_ncCalibrator.AddPoint(y);
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

    if(false) { //m_resultsAvailable&ERA_NEEDLE_TIP_CALIB) {
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
  
  //////////////////////////////////////////////////////////////////
  // END ESTIMATORFILEWIDGET BASE
  //////////////////////////////////////////////////////////////////
  
  
  //////////////////////////////////////////////////////////////////
  // BEGIN ESTIMATORSTREAMINGWIDGET BASE
  //////////////////////////////////////////////////////////////////
  EstimatorStreamingWidget::EstimatorStreamingWidget(QWidget *parent)
    : RPStreamingWidget(parent, (USVisualizer *)new PFVisualizer(parent))
    , m_state(ES_READY)
    , m_hwWidget(new RobotHardwareWidget(parent))
    , m_saveDataWidget(new SaveDataWidget(parent))
    , m_bottomRow(new QGridLayout(parent))
    , m_tpHistory(std::tr1::shared_ptr < PointCloudVisualizer > (new PointCloudVisualizer(1, Vec3d(1, 1, 0))))
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
    ADD_OPEN_FILE_PARAMETER(m_tipCalibPath, "Tip Calibration Path", CALLBACK_POINTER(onTipCalibPathChanged, EstimatorStreamingWidget), this, "C:/Joey/Data/TipCalibration/tipCalib.mat", "(*.mat)");
    ADD_OPEN_FILE_PARAMETER(m_tipHistoryPathLoad, "Presaved Tip History Path", CALLBACK_POINTER(onTipHistoryPathLoadChanged, EstimatorStreamingWidget), this, "C:/Joey/Data/TipCalibration/tipHistory.mat", "(*.mat)");
    ADD_SAVE_FILE_PARAMETER(m_tipHistoryPathSave, "Tip History Path", CALLBACK_POINTER(onTipHistoryPathSavedChanged, EstimatorStreamingWidget), this, "C:/Joey/Data/TipCalibration/tipHistory.mat", "(*.mat)");
    ADD_BOOL_PARAMETER(m_showPastTipPoints, "Show Past Tip Points", NULL, this, false);
    ADD_BOOL_PARAMETER(m_collectPastTipPoints, "Collect Past Tip Points", NULL, this, false);
    ADD_ACTION_PARAMETER(m_clearPastPoints, "Clear Past Tip Points", CALLBACK_POINTER(onClearPastPoints, EstimatorStreamingWidget), this, false);

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

  void EstimatorStreamingWidget::onTipHistoryPathLoadChanged()
  {
    arma::mat points;
    points.load(m_tipHistoryPathLoad->GetValue());

    arma::rowvec curr(3);
    for(s32 i=0; i<points.n_rows; i++) {
      Vec3d thisOne(points(i,0), points(i,1), points(i,2));
      m_pastTipPoints.push_back(thisOne);
    }
  }

  void EstimatorStreamingWidget::onTipHistoryPathSavedChanged()
  {
    m_tpHistory->SetPoints(m_pastTipPoints);
    m_tpHistory->SavePoints(m_tipHistoryPathSave->GetValue().c_str());
  }

  void EstimatorStreamingWidget::onClearPastPoints()
  {
    m_pastTipPoints.clear();
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

    SpoofRPDataWithNeedleTipCalibration(rp, &m_ntCalibrator);

    if(m_collectPastTipPoints->GetValue())
      m_pastTipPoints.push_back(rp.gps2.pos);

    HandleFrame(rp);
    HandleExtras();
    rp.Release();

  }

  void EstimatorStreamingWidget::HandleExtras()
  {
    if(!m_planeVis->GetRenderer() || !m_usVis->GetRenderer())
      return;
    m_tpHistory->SetPoints(m_pastTipPoints);
    bool planeHasTipHistoryPointCloud = m_planeVis->GetRenderer()->HasViewProp(m_tpHistory->GetActor());
    bool volHasTipHistoryPointCloud = m_usVis->GetRenderer()->HasViewProp(m_tpHistory->GetActor());

    if(m_showPastTipPoints->GetValue() && !planeHasTipHistoryPointCloud)
      m_planeVis->GetRenderer()->AddActor(m_tpHistory->GetActor());
    if(m_showPastTipPoints->GetValue() && !volHasTipHistoryPointCloud)
      m_usVis->GetRenderer()->AddActor(m_tpHistory->GetActor());
    if(!m_showPastTipPoints->GetValue() && planeHasTipHistoryPointCloud)
      m_planeVis->GetRenderer()->RemoveActor(m_tpHistory->GetActor());
    if(!m_showPastTipPoints->GetValue() && volHasTipHistoryPointCloud)
      m_usVis->GetRenderer()->RemoveActor(m_tpHistory->GetActor());

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
  //////////////////////////////////////////////////////////////////
  // END ESTIMATORSTREAMINGWIDGET BASE
  //////////////////////////////////////////////////////////////////
}