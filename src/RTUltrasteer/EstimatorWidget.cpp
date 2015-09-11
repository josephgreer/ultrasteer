#include "EstimatorWidget.h"
#include "NeedleTipCalibrationPP.h"
#include <vtkProperty.h>
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

  
  /////////////////////////////////////////////////////////////////////////////////
  //BEGIN PFVisualizer
  /////////////////////////////////////////////////////////////////////////////////
  class RPCoordTransform : public ImageCoordTransform
  {
  protected:
    Vec2d m_mpp;
    Vec2d m_origin;
    Matrix44d m_cal;
    GPS_Data m_gps;

  public:
    RPCoordTransform(const Vec2d &mpp, const Vec2d &origin, const Matrix44d &cal, const GPS_Data &gps)
      : m_mpp(mpp)
      , m_origin(origin)
      , m_gps(gps)
      , m_cal(cal)
    {
    }

    Vec3d Transform(const Vec2d &image) const
    {
      Vec2d scale(m_mpp.x/1000.0, m_mpp.y/1000.0);

      Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(m_gps.pose).GetOrientation(), m_gps.pos);
      return rpImageCoordToWorldCoord3(image, posePos, m_cal, m_origin, scale);
    }
  };


  static Measurement MeasurementFromRPDataAndNeedlePoint(const NeedlePoint &np, const Matrix44d &cal, const RPData &rp)
  {
    Measurement m;
    Vec2d mppScale(rp.mpp.x/1000.0, rp.mpp.y/1000.0);
    Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(rp.gps.pose).GetOrientation(), rp.gps.pos);
    m.ful = rpImageCoordToWorldCoord3((Vec2d)rp.roi.ul, posePos, cal, rp.origin, mppScale).ToArmaVec();
    m.fbl = rpImageCoordToWorldCoord3(Vec2d(rp.roi.ul.x, rp.roi.lr.y), posePos, cal, rp.origin, mppScale).ToArmaVec();
    m.fbr = rpImageCoordToWorldCoord3((Vec2d)rp.roi.lr, posePos, cal, rp.origin, mppScale).ToArmaVec();
    m.fur = rpImageCoordToWorldCoord3(Vec2d(rp.roi.lr.x, rp.roi.ul.y), posePos, cal, rp.origin, mppScale).ToArmaVec();
    m.doppler = arma::ones(1,1)*np.dopplerSum/255.0;
    m.fbx = rpImageCoordToWorldCoord3(Vec2d(1,0), posePos, cal, rp.origin, mppScale).ToArmaVec()-rpImageCoordToWorldCoord3(Vec2d(0,0), posePos, cal, rp.origin, mppScale).ToArmaVec();
    m.fby = rpImageCoordToWorldCoord3(Vec2d(0,1), posePos, cal, rp.origin, mppScale).ToArmaVec()-rpImageCoordToWorldCoord3(Vec2d(0,0), posePos, cal, rp.origin, mppScale).ToArmaVec();
    m.fbx = m.fbx/norm(m.fbx);
    m.fby = m.fby/norm(m.fby);


    //should be in R^(3xn)
    m.pos = np.point.ToArmaVec();
    //should be in R^(2xn)
    Vec2d uv = (np.imagePoint-(Vec2d)rp.roi.ul);
    m.uv = Vec2d(uv.x*mppScale.x, uv.y*mppScale.y).ToArmaVec();

    return m;
  }

  static TipState TipStateFromGPS2(const RPData &rp, f64 rho)
  {
    TipState t;
    t.pos = rp.gps2.pos.ToArmaVec();
    t.R = Matrix44d::FromCvMat(rp.gps2.pose).GetOrientation().ToArmaMat();
    t.rho = rho;
    return t;
  }

  static PFData AssemblePFData(const NeedlePoint &np, const Matrix44d &cal, const RPData &rp, f64 rho)
  {
    PFData res;
    res.m = MeasurementFromRPDataAndNeedlePoint(np, cal, rp);
    res.t = TipStateFromGPS2(rp, rho);
    res.u = rp.u;
    res.mpp = rp.mpp;
    return res;
  }

  ParticleFilterVisualizer::ParticleFilterVisualizer(Updateable *update)
    : ParameterCollection("Particle Filter")
    , m_update(update)
    , m_init(false)
    , m_segmenter(new Nf::NeedleSegmenter(0,0,this))
    , m_lastFrame(0)
    , m_pf(NULL)
  {
    ADD_FLOAT_PARAMETER(m_roc, "Expected ROC (mm)", NULL, this, 76.8, 20, 1000, 0.1);
    ADD_ACTION_PARAMETER(m_clearEstimatorData, "Clear Estimator Data", CALLBACK_POINTER(onClearEstimatorData, ParticleFilterVisualizer), this, false);
    ADD_BOOL_PARAMETER(m_collectMeasurements, "Collect US Measurements", NULL, this, false);
    ADD_BOOL_PARAMETER(m_showParticlePos, "Show Particle Positions", CALLBACK_POINTER(onVisibilityChanged, ParticleFilterVisualizer), this, true);
    ADD_BOOL_PARAMETER(m_showExpectedPos, "Show Estimated Position", CALLBACK_POINTER(onVisibilityChanged, ParticleFilterVisualizer), this, true);
    ADD_BOOL_PARAMETER(m_showExpectedOrientation, "Show Estimated Orientation", CALLBACK_POINTER(onVisibilityChanged, ParticleFilterVisualizer), this, true);
    ADD_BOOL_PARAMETER(m_showMeasurements, "Show Measurement Positions", CALLBACK_POINTER(onVisibilityChanged, ParticleFilterVisualizer), this, true);
    ADD_INT_PARAMETER(m_nParticles, "Number of Particles", CALLBACK_POINTER(onNumParticlesChanged, ParticleFilterVisualizer), this, 500, 25, 5000, 10);
    ADD_INT_PARAMETER(m_nVisSkip, "NVis Skip", CALLBACK_POINTER(onNVisSkipChanged, ParticleFilterVisualizer), this, 5, 1, 20, 1);
    ADD_FLOAT_PARAMETER(m_measurementNoise, "Measurement Noise (Pixels)", NULL, this, 5, 1, 1000, 1);
    ADD_ENUM_PARAMETER(m_pfMethod, "Particle Filter Method", CALLBACK_POINTER(onPFMethodChanged, ParticleFilterVisualizer), this, QtEnums::ParticleFilterMethod::PFM_FULL_STATE, "ParticleFilterMethod");
    ADD_CHILD_COLLECTION(m_segmenter.get());

    m_pfExpectedOrientation = vtkSmartPointer <vtkAxesActor>::New();
    m_pfExpectedOrientation->SetTotalLength(5,5,5);
    m_pfExpectedOrientation->SetVisibility(false);

    m_measurementPoints = std::tr1::shared_ptr < PointCloudVisualizer > (new PointCloudVisualizer(1, Vec3d(1, 0, 1)));
    m_measurementPoints->GetActor()->SetVisibility(false);
    m_measurementPoints->SetColor(Vec3d(1,0,1));

    m_pfExpectedPos = std::tr1::shared_ptr < SphereVisualizer > (new SphereVisualizer(Vec3d(0,0,0), 1));
    m_pfExpectedPos->GetActor()->SetVisibility(false);
    m_pfExpectedPos->SetColor(Vec3d(1,0,0));

    m_pfPoints = std::tr1::shared_ptr < PointCloudVisualizer > (new PointCloudVisualizer(1, Vec3d(1, 1, 0)));
    m_pfPoints->GetActor()->SetVisibility(false);
    m_pfPoints->SetColor(Vec3d(0,0,1));
    m_pfPoints->GetActor()->GetProperty()->SetOpacity(0.2);

    onVisibilityChanged();
  }

  ParticleFilterVisualizer::~ParticleFilterVisualizer()
  {
  }
  
  void ParticleFilterVisualizer::AddActorsToRenderer(vtkSmartPointer < vtkRenderer > renderer)
  {
    renderer->AddActor(m_pfExpectedOrientation);
    renderer->AddActor(m_pfPoints->GetActor());
    renderer->AddActor(m_pfExpectedPos->GetActor());
    renderer->AddActor(m_measurementPoints->GetActor());
  }

  void ParticleFilterVisualizer::SetVisiblity(bool visible)
  {
    m_pfExpectedOrientation->SetVisibility(visible);
    m_pfPoints->GetActor()->SetVisibility(visible);
    m_pfExpectedPos->GetActor()->SetVisibility(visible);
    m_measurementPoints->GetActor()->SetVisibility(visible);
  }

  void ParticleFilterVisualizer::onVisibilityChanged()
  {
    if(m_init) {
      m_pfPoints->GetActor()->SetVisibility(m_showParticlePos->GetValue());
      m_pfExpectedPos->GetActor()->SetVisibility(m_showExpectedPos->GetValue());
      m_pfExpectedOrientation->SetVisibility(m_showExpectedOrientation->GetValue());
      m_measurementPoints->GetActor()->SetVisibility(m_showMeasurements->GetValue());
    } else {
      m_pfPoints->GetActor()->SetVisibility(false);
      m_pfExpectedPos->GetActor()->SetVisibility(false);
      m_pfExpectedOrientation->SetVisibility(false);
      m_measurementPoints->GetActor()->SetVisibility(false);
    }
  }
  
  void ParticleFilterVisualizer::DoSegmentation(RPData *rp, NeedleFrame &doppler, NeedleFrame &bmode)
  {
    if(!m_segmenter->IsInit())
      m_segmenter->Initialize(rp->b8->width, rp->b8->height);

    RPCoordTransform transform(rp->mpp, rp->origin, Matrix44d(TRANSDUCER_CALIBRATION_COEFFICIENTS), rp->gps);
    m_segmenter->ProcessColor(rp->color, rp->b8, &transform);
    m_segmenter->GetSegmentationResults(bmode, doppler);
    rp->dis = m_segmenter->GetDisplayImage();

    if(m_collectMeasurements->GetValue()) {
      if(doppler.segments.size() > 0)
        m_measurementPoints->AddPoint(doppler.segments[0].pts[0].point);

    }
  }

  void ParticleFilterVisualizer::onClearEstimatorData()
  {
    m_measurementPoints->ClearPoints();

    m_pfFramesProcessed.clear();
    m_update->onUpdate();
    m_init = false;
    onVisibilityChanged();
  }

  void ParticleFilterVisualizer::onPFMethodChanged()
  {
    onClearEstimatorData();
    Initialize();
  }

  void ParticleFilterVisualizer::onNumParticlesChanged()
  {
    if(m_pf != NULL) {
      std::tr1::shared_ptr < PFParams > params = GetParams(-1);
      m_pf->Resample(m_nParticles->GetValue(), params.get());
    }
    if(!m_pfFramesProcessed.empty())
      UpdateVisualizations(m_lastFrame);
  }

  std::tr1::shared_ptr < PFParams > ParticleFilterVisualizer::GetParams(s32 frame)
  {
    std::tr1::shared_ptr < PFParams > res;
    if(frame < 0) {
      if(m_pfMethod->GetValue() == QtEnums::PFM_FULL_STATE) {
        res = std::tr1::shared_ptr < PFParams > (new PFFullStateParams());
      } else {
        res = std::tr1::shared_ptr < PFParams > (new PFMarginalizedParams());
      }
    } else {
      PFData pd = m_pfFramesProcessed[frame];
      if(m_pfMethod->GetValue() == QtEnums::PFM_FULL_STATE) {
        res = std::tr1::shared_ptr < PFParams > (new PFFullStateParams(pd.mpp));
      } else {
        res = std::tr1::shared_ptr < PFParams > (new PFMarginalizedParams(pd.mpp));
      }
      res->usw = norm(pd.m.fur-pd.m.ful);
      res->ush = norm(pd.m.ful-pd.m.fbl);
      res->measurementOffsetSigma = (Vec2d(pd.mpp.x/1000.0, pd.mpp.y/1000.0)*m_measurementNoise->GetValue()).ToArmaVec();
    }
    res->minimumMeasurements = 10;

    return res;
  }

  void ParticleFilterVisualizer::Initialize()
  {
    std::tr1::shared_ptr < PFParams > params = GetParams(-1);
    if(m_pfMethod->GetValue() == QtEnums::PFM_FULL_STATE) {
      m_pf = std::tr1::shared_ptr < ParticleFilter > (new ParticleFilterFullState(m_nParticles->GetValue(), 
        std::tr1::shared_ptr < Distribution > (new LUTDist(params->onNeedleDopplerLUTPath.c_str())), 
        std::tr1::shared_ptr < Distribution > (new LUTDist(params->offNeedleDopplerLUTPath.c_str())), params.get()));
    } else {
      m_pf = std::tr1::shared_ptr < ParticleFilter > (new ParticleFilterMarginalized(m_nParticles->GetValue(), 
        std::tr1::shared_ptr < Distribution > (new LUTDist(params->onNeedleDopplerLUTPath.c_str())), 
        std::tr1::shared_ptr < Distribution > (new LUTDist(params->offNeedleDopplerLUTPath.c_str())), params.get()));
    }
    m_init = false;
  }

  s32 ParticleFilterVisualizer::NumberOfMeasurementsUpToAndIncludingFrame(s32 frame)
  {
    s32 res = 0;
    while(m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end()) {
      res++;
      frame--;
    }
    return res;
  }

  std::vector < TipState > ParticleFilterVisualizer::AssembleTipHistory(s32 frame)
  {
    std::vector < TipState > res;
    while(m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end()) {
      res.push_back(m_pfFramesProcessed[frame].t);
      frame--;
    }
    return res;
  }

  std::vector < Measurement > ParticleFilterVisualizer::AssembleMeasurements(s32 frame)
  {
    std::vector < Measurement > res;
    std::tr1::shared_ptr < PFParams > p = GetParams(frame);
    while(m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end() && res.size() < (s32)(p->n*1.5+0.5)) {
      res.push_back(m_pfFramesProcessed[frame].m);
      frame--;
    }

    return res;
  }

  std::vector < NSCommand > ParticleFilterVisualizer::AssembleCommands(s32 frame)
  {
    std::vector < NSCommand > res;
    std::tr1::shared_ptr < PFParams > p = GetParams(frame);
    while(m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end() && res.size() < p->n) {
      res.push_back(m_pfFramesProcessed[frame].u);
      frame--;
    }

    return res;
  }

  arma::vec ParticleFilterVisualizer::AssembleDts(s32 frame)
  {
    arma::vec res = arma::zeros(NumberOfMeasurementsUpToAndIncludingFrame(frame));
    
    std::tr1::shared_ptr < PFParams > p = GetParams(frame);
    s32 curIdx = 0;
    while(m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end() && m_pfFramesProcessed.find(frame-1) != m_pfFramesProcessed.end()) {
      res(curIdx) = (m_pfFramesProcessed[frame].u.tick-m_pfFramesProcessed[frame-1].u.tick)/1000.0;
      curIdx++;
      frame--;
    }
    return res;
  }

  void ParticleFilterVisualizer::onNVisSkipChanged()
  {
    UpdateVisualizations(m_lastFrame);
  }

  void ParticleFilterVisualizer::UpdateVisualizations(s32 frame)
  {
    arma::mat pos = m_pfFramesProcessed[frame].particlePos;
    arma::mat w = m_pfFramesProcessed[frame].w;
    std::vector < Vec3d > particlePos;
    std::vector < f64 > particleWs;
    for(s32 i=0; i<pos.n_cols; i+=m_nVisSkip->GetValue())  {
      particlePos.push_back(Vec3d(pos(0, i), pos(1, i), pos(2, i)));
      particleWs.push_back(w(i));
    }
    m_pfPoints->SetPoints(particlePos);//, particleWs);
    m_pfExpectedPos->SetCenter(Vec3d::FromArmaVec(m_pfFramesProcessed[frame].est.pos));

    Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(Matrix33d::FromArmaMatrix3x3(m_pfFramesProcessed[frame].est.R), Vec3d::FromArmaVec(m_pfFramesProcessed[frame].est.pos));
    m_pfExpectedOrientation->PokeMatrix(posePos.GetVTKMatrix());

    m_update->onRepaint();
  }

  void ParticleFilterVisualizer::Update(RPData *rp, s32 frame)
  {
    m_lastFrame=  frame;
    NeedleFrame doppler, bmode;
    DoSegmentation(rp, doppler, bmode);

    if(m_pf == NULL)
      Initialize();

    // If we've already run this data through our particle filter, bail.
    if(m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end()) {
      UpdateVisualizations(frame);
      return;
    }

    NeedlePoint np; memset(&np, 0, sizeof(NeedlePoint));
    if(doppler.segments.size() > 0) {
      //For now use doppler centroid
      np = doppler.segments[0].pts[0];
    } else {
      arma::mat rands = arma::randu(2);
      Vec2d roiSize = rp->roi.lr-rp->roi.ul;
      //Create a random point
      np.imagePoint = Vec2d(roiSize.x*rands(0), roiSize.x*rands(1));
      Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(rp->gps.pose).GetOrientation(), rp->gps.pos);
      np.point = rpImageCoordToWorldCoord3((Vec2d)np.imagePoint+(Vec2d)rp->roi.ul, posePos, Matrix44d(TRANSDUCER_CALIBRATION_COEFFICIENTS), 
        rp->origin, Vec2d(rp->mpp.x/1000, rp->mpp.y/1000.0));
    }

    onVisibilityChanged();

    m_pfFramesProcessed[frame] = AssemblePFData(np, Matrix44d(TRANSDUCER_CALIBRATION_COEFFICIENTS), *rp, m_roc->GetValue());

    std::tr1::shared_ptr < PFParams > params = GetParams(frame);
    if(!m_init && NumberOfMeasurementsUpToAndIncludingFrame(frame) >= params->minimumMeasurements) {
      //We are not initialized, but we can initialize now
      m_pf->InitializeParticleFilter(AssembleTipHistory(frame), params.get());
      m_init = true;
      // Push in estimate from particle filter
      m_pfFramesProcessed[frame].est = m_pf->GetExpectedValue(params.get());
      m_pfFramesProcessed[frame].particleRs = m_pf->GetParticleOrientations(params.get());
      m_pfFramesProcessed[frame].particlePos = m_pf->GetParticlePositions(params.get());
      m_pfFramesProcessed[frame].w = m_pf->GetWeights();
      UpdateVisualizations(frame);
    } else if(m_init) {
      //We're initialized
      m_pf->Propagate(&m_pfFramesProcessed[frame].u, 
        (m_pfFramesProcessed[frame].u.tick-m_pfFramesProcessed[frame-1].u.tick)/1000.0, params.get());
      m_pf->ApplyMeasurement(AssembleMeasurements(frame), AssembleCommands(frame), AssembleDts(frame), params.get());

      // Push in estimate from particle filter
      m_pfFramesProcessed[frame].est = m_pf->GetExpectedValue(params.get());
      m_pfFramesProcessed[frame].particleRs = m_pf->GetParticleOrientations(params.get());
      m_pfFramesProcessed[frame].particlePos = m_pf->GetParticlePositions(params.get());
      m_pfFramesProcessed[frame].w = m_pf->GetWeights();
      UpdateVisualizations(frame);

      // Resample if effective number of particles drops below threshold
      f64 nParts = m_pf->EffectiveSampleSize();
      if(nParts < params->neff*m_pf->GetNumberOfParticles())
        m_pf->Resample(m_pf->GetNumberOfParticles(), params.get());
      

      //NTrace("Frame %d, dt %f, u.v %f\n", frame, (m_pfFramesProcessed[frame].u.tick-m_pfFramesProcessed[frame-1].u.tick)/1000.0, m_pfFramesProcessed[frame].u.v);
    }

  }

  void ParticleFilterVisualizer::onUpdate()
  {
    m_update->onUpdate();
  }

  void ParticleFilterVisualizer::onRepaint()
  {
    m_update->onRepaint();
  }

  /////////////////////////////////////////////////////////////////////////////////
  //END PFVisualizer
  /////////////////////////////////////////////////////////////////////////////////

  EstimatorFileWidget::EstimatorFileWidget(QWidget *parent)
    : RPFileWidget(parent, (USVisualizer *)new PFVisualizer(parent))
    , m_state(EFS_READY)
    , m_resultsAvailable(ERA_NONE)
    , m_pfVisualizer(new ParticleFilterVisualizer(this))
  {
    ADD_ACTION_PARAMETER(m_doNeedleCalib, "Do Needle Calibration", CALLBACK_POINTER(onDoNeedleCalibrationPushed, EstimatorFileWidget), this, false);
    ADD_ENUM_PARAMETER(m_operationMode, "Operation Mode", CALLBACK_POINTER(onSetOperationMode, EstimatorFileWidget), this, QtEnums::EstimatorOperationMode::EOM_NONE, "EstimatorOperationMode");
    ADD_SAVE_FILE_PARAMETER(m_tipCalibPath, "Tip Calibration Save Path", NULL, this, PATH_CAT("TipCalibration/ShallowInsertion/tipCalib.mat"), "(*.mat)");
    ADD_OPEN_FILE_PARAMETER(m_tipCalibPathLoad, "Presaved Tip Calibration",CALLBACK_POINTER(onTipCalibPathChanged, EstimatorFileWidget), this, PATH_CAT("TipCalibration/ShallowInsertion/tipCalib.mat"), "(*.mat)");
    ADD_SAVE_FILE_PARAMETER(m_pointsDataPath, "Point History Save Path", NULL, this, PATH_CAT("Trial1/PreInsertionGPS/TipHistory.mat"), "(*.mat)");
    ADD_OPEN_FILE_PARAMETER(m_pointsDataPathLoad, "Presaved Point History", CALLBACK_POINTER(onPointsDataPathChanged, EstimatorFileWidget), this, PATH_CAT("Trial1/PreInsertionGPS/TipHistory.mat"), "(*.mat)");
    ADD_ACTION_PARAMETER(m_clearCalibrationData, "Clear Calibration Data", CALLBACK_POINTER(onClearCalibrationData, EstimatorFileWidget), this, false);
    ADD_ACTION_PARAMETER(m_clearTipCalibration, "Clear Tip Calibration", CALLBACK_POINTER(onClearTipCalibration, EstimatorFileWidget), this, false);
    ADD_CHILD_COLLECTION(m_pfVisualizer.get());

    m_calibrationPointsTip = std::tr1::shared_ptr < PointCloudVisualizer > (new PointCloudVisualizer(1, Vec3d(0, 1, 0)));
    m_calibrationPointsCurvature = std::tr1::shared_ptr < PointCloudVisualizer > (new PointCloudVisualizer(1, Vec3d(1, 1, 0)));

    m_calibTip = std::tr1::shared_ptr < SphereVisualizer > (new SphereVisualizer(Vec3d(0,0,0), 1));
    m_calibTip->SetColor(Vec3d(1,0,0));

    m_calibTipFrame = vtkSmartPointer <vtkAxesActor>::New();
    m_calibTipFrame->SetTotalLength(5,5,5);

    m_planeVis->GetRenderer()->AddActor(m_calibrationPointsTip->GetActor());
    m_planeVis->GetRenderer()->AddActor(m_calibrationPointsCurvature->GetActor());

    m_pfVisualizer->SetVisiblity(false);
    m_pfVisualizer->AddActorsToRenderer(m_planeVis->GetRenderer());

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
    case EFS_ESTIMATE:
      {
        m_pfVisualizer->Update(&m_data, m_frame->GetValue());
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
    case EFS_ESTIMATE:
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

  void EstimatorFileWidget::onUpdate()
  {
    onUpdateFrame();
  }

  void EstimatorFileWidget::onRepaint()
  {
    m_planeVis->repaint();
    m_usVis->repaint();
    m_imageViewer->repaint();
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

  void EstimatorFileWidget::onSetOperationMode()
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
    case EFS_ESTIMATE:
      {
        m_state = EFS_NEEDLE_TIP_CALIB;

        m_planeVis->repaint();

        switch(m_operationMode->GetValue()) 
        {
        case QtEnums::EstimatorOperationMode::EOM_NONE: 
          {
            m_state = EFS_PRIMED;
            break;
          }
        case QtEnums::EstimatorOperationMode::EOM_CALIB_NEEDLE_TIP:
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
        case QtEnums::EstimatorOperationMode::EOM_CALIB_CURVATURE_GPS:
          {
            m_state = EFS_NEEDLE_CURVATURE_CALIB_GPS;
            break;
          }
        case QtEnums::EstimatorOperationMode::EOM_CALIB_CURVATURE_US:
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
        case QtEnums::EstimatorOperationMode::EOM_ESTIMATE: 
          {
            m_pfVisualizer->Initialize();
            m_state = EFS_ESTIMATE;
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
    onUpdateFrame();
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
    case EFS_ESTIMATE:
      break;
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
    case EFS_ESTIMATE:
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
    ADD_OPEN_FILE_PARAMETER(m_tipCalibPath, "Tip Calibration Path", CALLBACK_POINTER(onTipCalibPathChanged, EstimatorStreamingWidget), this, PATH_CAT("TipCalibration/ShallowInsertion/tipCalib.mat"), "(*.mat)");
    ADD_OPEN_FILE_PARAMETER(m_tipHistoryPathLoad, "Presaved Tip History Path", CALLBACK_POINTER(onTipHistoryPathLoadChanged, EstimatorStreamingWidget), this, PATH_CAT("Trial1/PreInsertGPS/TipHistory.mat"), "(*.mat)");
    ADD_SAVE_FILE_PARAMETER(m_tipHistoryPathSave, "Tip History Path", CALLBACK_POINTER(onTipHistoryPathSavedChanged, EstimatorStreamingWidget), this, PATH_CAT("Trial1/PreInsertGPS/TipHistory.mat"), "(*.mat)");
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

  void EstimatorStreamingWidget::Initialize(bool init)
  {
    RPStreamingWidget::Initialize(init);
    m_saveDataWidget->SetEnabled(init);
    switch(m_state) {
    case ES_READY:
      break;
    case ES_PRIMED:
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

    RPFileHeader header = RPFileHeader_uDataDesc(m_receiver->GetReaders()->GetImageDesc(RPF_COLOR));
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

  }

  void EstimatorStreamingWidget::HandleFrame(RPData &rp)
  { 
    rp.u = m_u;
    switch(m_state) {
    case ES_READY:
      {
        m_state = ES_PRIMED;
        m_saveDataWidget->SaveDataFrame(rp);
        break;
      }
    case ES_PRIMED: 
      {
        m_saveDataWidget->SaveDataFrame(rp);
        break;
      }
    case ES_RECORDING1: 
      {
        if(m_executeCommand)
          ExecuteCommand();
        rp.u.v = 0;
        m_saveDataWidget->SaveDataFrame(rp);
        m_state = ES_RECORDING2;
        break;
      }
    case ES_RECORDING2: 
      {
        if(m_executeCommand)
          ExecuteCommand();
        m_saveDataWidget->SaveDataFrame(rp);
        break;
      }
    case ES_PAUSED:
      {
        rp.u.v = 0;
        // how we signal that this is a valid gps reading
        rp.gps2.offset[0] = 12345.0;
        m_saveDataWidget->SaveDataFrame(rp);
        m_saveDataWidget->StopRecording();
        break;
      }
    default:
      {
        m_saveDataWidget->SaveDataFrame(rp);
        break;
      }
    }

    SpoofRPDataWithNeedleTipCalibration(rp, &m_ntCalibrator);

    if(m_collectPastTipPoints->GetValue())
      m_pastTipPoints.push_back(rp.gps2.pos);

    RPStreamingWidget::HandleFrame(rp);
    HandleExtras();
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