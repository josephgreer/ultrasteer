#include "EstimatorWidget.h"
#include "NeedleTipCalibrationPP.h"
#include <regex>
#include <vtkProperty.h>
#include <vtkFloatArray.h>
#include <vtkCamera.h>
#include <QKeyEvent>

namespace Nf
{
#define INSERT_VEL 0.5 //mm/s

  void SpoofRPDataWithNeedleTipCalibration(RPData &rp, const EMNeedleTipCalibrator *em, s32 frame)
  {
    Vec3d tipOffset; Matrix33d tipFrame;
    em->GetSolution(tipOffset, tipFrame, rp.gps2.pos, Matrix44d::FromCvMat(rp.gps2.pose).GetOrientation(), frame);
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

  static PFData AssemblePFData(const NeedlePoint &np, const Matrix44d &cal, const RPData &rp, f64 rho, const PolyCurve &curve)
  {
    PFData res;
    res.m = MeasurementFromRPDataAndNeedlePoint(np, cal, rp);
    res.t = TipStateFromGPS2(rp, rho);
    res.u = rp.u;
    res.mpp = rp.mpp;
    res.measCurve = curve;
    return res;
  }

  ParticleFilterVisualizer::ParticleFilterVisualizer(Updateable *update, QWidget *parent)
    : ParameterCollection("Particle Filter")
    , m_update(update)
    , m_init(false)
    , m_segmenter(new Nf::NeedleSegmenter(0,0,this))
    , m_lastFrame(0)
    , m_pf(NULL)
    , m_barChartWidget(new BarChartWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT/4)))
    , m_lineChartWidget(new LineChartWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT/4)))
  {
    m_pfParams = std::tr1::shared_ptr < PFParams > ((PFParams *)new PFFullStateParams());
    m_pfParamsMarg = std::tr1::shared_ptr < PFParams > ((PFParams *)new PFMarginalizedParams());

    ADD_FLOAT_PARAMETER(m_roc, "Expected ROC (mm)", NULL, this, 76.8, 20, 1000, 0.1);
    ADD_ACTION_PARAMETER(m_clearEstimatorData, "Clear Estimator Data", CALLBACK_POINTER(onClearEstimatorData, ParticleFilterVisualizer), this, false);
    ADD_BOOL_PARAMETER(m_collectMeasurements, "Collect US Measurements", NULL, this, false);
    ADD_BOOL_PARAMETER(m_showParticlePos, "Show Particle Positions", CALLBACK_POINTER(onVisibilityChanged, ParticleFilterVisualizer), this, true);
    ADD_BOOL_PARAMETER(m_showExpectedPos, "Show Estimated Position", CALLBACK_POINTER(onVisibilityChanged, ParticleFilterVisualizer), this, true);
    ADD_BOOL_PARAMETER(m_showExpectedOrientation, "Show Estimated Orientation", CALLBACK_POINTER(onVisibilityChanged, ParticleFilterVisualizer), this, true);
    ADD_BOOL_PARAMETER(m_showMeasurements, "Show Measurement Positions", CALLBACK_POINTER(onVisibilityChanged, ParticleFilterVisualizer), this, true);
    ADD_BOOL_PARAMETER(m_showMeasurementCurve, "Show Measurement Curve", CALLBACK_POINTER(onVisibilityChanged, ParticleFilterVisualizer), this, true);
    ADD_BOOL_PARAMETER(m_showCurvePoints, "Show Curve Points", CALLBACK_POINTER(onVisibilityChanged, ParticleFilterVisualizer), this, true);
    ADD_INT_PARAMETER(m_nParticles, "Number of Particles", CALLBACK_POINTER(onNumParticlesChanged, ParticleFilterVisualizer), this, 500, 25, 5000, 10);
    ADD_INT_PARAMETER(m_nVisSkip, "NVis Skip", CALLBACK_POINTER(onNVisSkipChanged, ParticleFilterVisualizer), this, 5, 1, 100, 1);
    ADD_ENUM_PARAMETER(m_pfMethod, "Particle Filter Method", CALLBACK_POINTER(onPFMethodChanged, ParticleFilterVisualizer), this, QtEnums::ParticleFilterMethod::PFM_FULL_STATE, "ParticleFilterMethod");
    ADD_CHILD_COLLECTION(m_segmenter.get());
    ADD_CHILD_COLLECTION(m_pfParams.get());
    ADD_CHILD_COLLECTION(m_pfParamsMarg.get());

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

    m_measCurveVis = std::tr1::shared_ptr < CurveVisualizer > (new CurveVisualizer());
    m_measCurveVis->GetActor()->SetVisibility(false);
    m_measCurveVis->SetColor(Vec3d(1,0,1));

    m_curvePoints = std::tr1::shared_ptr < PointCloudVisualizer > (new PointCloudVisualizer(1, Vec3d(1, 1, 0)));
    m_curvePoints->GetActor()->SetVisibility(false);
    m_curvePoints->SetColor(Vec3d(1,1,0));

    m_barChartWidget->Initialize();
    m_lineChartWidget->Initialize();

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
    renderer->AddActor(m_measCurveVis->GetActor());
    renderer->AddActor(m_curvePoints->GetActor());
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
      m_measCurveVis->GetActor()->SetVisibility(m_showMeasurementCurve->GetValue());
      m_curvePoints->GetActor()->SetVisibility(m_showCurvePoints->GetValue());
    } else {
      m_pfPoints->GetActor()->SetVisibility(false);
      m_pfExpectedPos->GetActor()->SetVisibility(false);
      m_pfExpectedOrientation->SetVisibility(false);
      m_measurementPoints->GetActor()->SetVisibility(false);
      m_measCurveVis->GetActor()->SetVisibility(false);
      m_curvePoints->GetActor()->SetVisibility(false);
    }
  }
  
  void ParticleFilterVisualizer::DoSegmentation(RPData *rp, NeedleFrame &doppler, NeedleFrame &bmode, PolyCurve *curve, bool imageOnly)
  {
    if(!m_segmenter->IsInit())
      m_segmenter->Initialize(rp->b8->width, rp->b8->height);

    RPCoordTransform transform(rp->mpp, rp->origin, Matrix44d(TRANSDUCER_CALIBRATION_COEFFICIENTS), rp->gps);
    if(imageOnly) {
      m_segmenter->ProcessColor(rp->color, rp->b8, &transform);
    } else {
      m_segmenter->UpdateModel(curve, rp->color, rp->b8, &transform,true);
      m_segmenter->GetSegmentationResults(bmode, doppler);
    }
    rp->dis = m_segmenter->GetDisplayImage();

    if(m_collectMeasurements->GetValue()) {
      if(doppler.segments.size() > 0)
        m_measurementPoints->AddPoint(doppler.segments[0].pts[0].point);
    }
  }

  void ParticleFilterVisualizer::onClearEstimatorData()
  {
    m_measurementPoints->ClearPoints();
    m_segmenter->Clear();

    m_pfFramesProcessed.clear();
    m_update->onUpdate();
    m_init = false;
    onVisibilityChanged();
  }

  void ParticleFilterVisualizer::onPFMethodChanged()
  {
    onClearEstimatorData();
    Initialize(m_basePath.c_str());
  }

  void ParticleFilterVisualizer::onNumParticlesChanged()
  {
    onPFMethodChanged();
  }

  std::tr1::shared_ptr < PFParams > ParticleFilterVisualizer::GetParams(s32 frame)
  {
    PFData pd = m_pfFramesProcessed[frame];

    m_pfParams->usw = norm(pd.m.fur-pd.m.ful);
    m_pfParams->ush = norm(pd.m.ful-pd.m.fbl);
    m_pfParams->mpp = pd.mpp;
    m_pfParams->measurementOffsetSigma = Vec2d(pd.mpp.x/1000.0*m_pfParams->measurementOffsetSigmaPx->GetValue().x, pd.mpp.y/1000.0*m_pfParams->measurementOffsetSigmaPx->GetValue().y);
    

    m_pfParamsMarg->usw = norm(pd.m.fur-pd.m.ful);
    m_pfParamsMarg->ush = norm(pd.m.ful-pd.m.fbl);
    m_pfParamsMarg->mpp = pd.mpp;
    m_pfParamsMarg->measurementOffsetSigma = Vec2d(pd.mpp.x/1000.0*m_pfParamsMarg->measurementOffsetSigmaPx->GetValue().x, pd.mpp.y/1000.0*m_pfParamsMarg->measurementOffsetSigmaPx->GetValue().y);

    //res->sigB0 =    -1.0515;
    //res->sigB1 =    0.0905;
    //res->onNeedleDopplerLUTPath = PATH_CAT("Trial2/Insertion/pdopoverneedle.dat");
    //res->offNeedleDopplerLUTPath = PATH_CAT("Trial2/Insertion/pdopnotoverneedle.dat");
    //res->offFrameB0 = -5;
    //res->offFrameB1 = 5;

    //res->particleSigmaPos =

    return m_pfMethod->GetValue() == QtEnums::PFM_FULL_STATE ? m_pfParams : m_pfParamsMarg;
  }

  void ParticleFilterVisualizer::Initialize(const char *basePath)
  {
    std::tr1::shared_ptr < PFParams > params = GetParams(-1);

    //Check if we can use the baseNeedlePath
    std::string overNeedlePath = (std::string(basePath)+std::string("/")+std::string("pdopoverneedle.dat"));
    std::string offNeedlePath = (std::string(basePath)+std::string("/")+std::string("pdopnotoverneedle.dat"));
    QFileInfo onp(overNeedlePath.c_str());
    QFileInfo ofp(offNeedlePath.c_str());

    if(onp.exists() && onp.isReadable() && ofp.exists() && ofp.isReadable()) {
      params->onNeedleDopplerLUTPath->SetValue(overNeedlePath);
      params->offNeedleDopplerLUTPath->SetValue(offNeedlePath);
    }

    if(m_pfMethod->GetValue() == QtEnums::PFM_FULL_STATE) {
      m_pf = std::tr1::shared_ptr < ParticleFilter > (new ParticleFilterFullState(m_nParticles->GetValue(), 
        std::tr1::shared_ptr < Distribution > (new LUTDist(params->onNeedleDopplerLUTPath->GetValue().c_str())), 
        std::tr1::shared_ptr < Distribution > (new LUTDist(params->offNeedleDopplerLUTPath->GetValue().c_str())), params.get()));
    } else {
      m_pf = std::tr1::shared_ptr < ParticleFilter > (new ParticleFilterMarginalized(m_nParticles->GetValue(), 
        std::tr1::shared_ptr < Distribution > (new LUTDist(params->onNeedleDopplerLUTPath->GetValue().c_str())), 
        std::tr1::shared_ptr < Distribution > (new LUTDist(params->offNeedleDopplerLUTPath->GetValue().c_str())), params.get()));
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

  static PFData grabDataIndex(s32 idx, const std::map < s32, PFData > &pfData)
  {
    std::map< s32, PFData >::const_iterator i=pfData.begin();
    std::advance(i,idx);

    return i->second;
  }


#if 0
  std::vector < Measurement > ParticleFilterVisualizer::AssembleMeasurements(s32 frame)
  {
    std::vector < Measurement > res;
    std::tr1::shared_ptr < PFParams > p = GetParams(frame);
    s32 n = p->n->GetValue();

    res.push_back(m_pfFramesProcessed[frame].m);

    s32 N = m_pfFramesProcessed.size();
    n = min(n*2,N);

    arma::uvec idxs = arma::linspace<arma::uvec>(1, N-1, N-1);
    idxs = arma::shuffle(idxs);

    for(s32 i=0; i<min(idxs.n_rows,(u32)n); i++) {
      res.push_back(grabDataIndex(idxs(i), m_pfFramesProcessed).m);
    }

    return res;
  }
#else
  std::vector < Measurement > ParticleFilterVisualizer::AssembleMeasurements(s32 frame)
  {
    std::vector < Measurement > res;
    std::tr1::shared_ptr < PFParams > p = GetParams(frame);
    s32 n = p->n->GetValue();

    while(m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end() && res.size() < (s32)(n*2+0.5)) {
      res.push_back(m_pfFramesProcessed[frame].m);
      frame--;
    }
    return res;
  }
#endif

  std::vector < Measurement > ParticleFilterVisualizer::AssembleAllMeasurements(s32 frame)
  {
    std::vector < Measurement > res;
    while(m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end()) {
      res.push_back(m_pfFramesProcessed[frame].m);
      frame--;
    }

    std::reverse(res.begin(), res.end());

    return res;
  }

  std::vector < NSCommand > ParticleFilterVisualizer::AssembleCommands(s32 frame)
  {
    std::vector < NSCommand > res;
    std::tr1::shared_ptr < PFParams > p = GetParams(frame);
    s32 n = p->n->GetValue();
    while(m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end() && res.size() < n) {
      res.push_back(m_pfFramesProcessed[frame].u);
      frame--;
    }

    return res;
  }

  std::vector < NSCommand > ParticleFilterVisualizer::AssembleAllCommands(s32 frame)
  {
    std::vector < NSCommand > res;
    while(m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end()) {
      res.push_back(m_pfFramesProcessed[frame].u);
      frame--;
    }

    std::reverse(res.begin(), res.end());

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

  arma::vec ParticleFilterVisualizer::AssembleAllDts(s32 frame)
  {
    arma::vec res = arma::zeros(NumberOfMeasurementsUpToAndIncludingFrame(frame));
    
    s32 curIdx = 0;
    while(m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end() && m_pfFramesProcessed.find(frame-1) != m_pfFramesProcessed.end()) {
      res(curIdx) = (m_pfFramesProcessed[frame].u.tick-m_pfFramesProcessed[frame-1].u.tick)/1000.0;
      curIdx++;
      frame--;
    }
    res = arma::flipud(res);
    return res;
  }

  static void SaveParameters(const PFParams *p, FILE *f)
  {
    QFileInfo fi(p->onNeedleDopplerLUTPath->GetValue().c_str());

    fprintf(f, "n %d\n", p->n->GetValue());
    fprintf(f, "neff %f\n", p->neff->GetValue());
    fprintf(f, "mpp %f, %f\n", p->mpp.x, p->mpp.y);
    fprintf(f, "usw %f\n", p->usw);
    fprintf(f, "ush %f\n", p->usw);
    fprintf(f, "tipOffset %f, %f, %f\n", p->tipOffset->GetValue().x, p->tipOffset->GetValue().y, p->tipOffset->GetValue().z);  
    fprintf(f, "lutBasePath %s\n", (fi.dir().path().toStdString()+"/").c_str());
    fprintf(f, "useLUT %d\n", p->useLut->GetValue());
    fprintf(f, "minimumMeasurements %d\n", p->minimumMeasurements->GetValue());
    fprintf(f, "minLength %f\n", p->minLength->GetValue());
    fprintf(f, "dopplerLambda %f\n", p->lambdaDop->GetValue());
    fprintf(f, "dopplerSigB0 %f\n", p->sigB0->GetValue());
    fprintf(f, "dopplerSigB1 %f\n", p->sigB1->GetValue());
    fprintf(f, "offFrameB0 %f\n", p->offFrameB0->GetValue());
    fprintf(f, "offFrameB1 %f\n", p->offFrameB1->GetValue());
    fprintf(f, "measurementOffsetSigma %f, %f\n", p->measurementOffsetSigma.x, p->measurementOffsetSigma.y);
    fprintf(f, "measurementOffsetSigmaPx %f, %f\n", p->measurementOffsetSigmaPx->GetValue().x, p->measurementOffsetSigmaPx->GetValue().y);
    fprintf(f, "initPosSigma %f, %f, %f\n", p->initPosSigma->GetValue().x, p->initPosSigma->GetValue().y, p->initPosSigma->GetValue().z);
    fprintf(f, "initOrientationSigma %f, %f, %f\n", p->initOrientationSigma->GetValue().x, p->initOrientationSigma->GetValue().y, p->initOrientationSigma->GetValue().z);
    fprintf(f, "initRhoSigma %f\n", p->initRhoSigma->GetValue());
    fprintf(f, "particleSigmaOr %f, %f, %f\n", p->particleSigmaOr->GetValue().x, p->particleSigmaOr->GetValue().y, p->particleSigmaOr->GetValue().z);
    fprintf(f, "particleSigmaPos %f, %f, %f\n", p->particleSigmaPos->GetValue().x, p->particleSigmaPos->GetValue().y, p->particleSigmaPos->GetValue().z);
    fprintf(f, "particleSigmaRho %f\n", p->particleSigmaRho->GetValue());
    fprintf(f, "particleSigmaVel %f\n", p->particleSigmaVel->GetValue());
  }

  static void SaveParameters(const PFFullStateParams *p, FILE *f)
  {
    SaveParameters((const PFParams *)p, f);
  }

  static void SaveParameters(const PFMarginalizedParams *p, FILE *f)
  {
    SaveParameters((const PFParams *)p, f);

    fprintf(f, "subsetSize %d\n", p->subsetSize->GetValue());
    fprintf(f, "procrustesIt %d\n", p->procrustesIt->GetValue());
    fprintf(f, "measurementSigma %f, %f, %f\n", p->measurementSigma->GetValue().x, p->measurementSigma->GetValue().y, p->measurementSigma->GetValue().z);
    fprintf(f, "nPoints %d\n", p->nPoints->GetValue());
  }

  void ParticleFilterVisualizer::SaveParticleFilterResults(s32 frame, const char *directory)
  {
    std::vector < TipState > trueStates;
    std::vector < TipState > estimatedStates;

    // create the results directory
    if(!QDir(directory).exists())
      QDir().mkdir(directory);
    
    s32 origFrame = frame;
    while(m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end()) {
      trueStates.push_back(m_pfFramesProcessed[frame].t);
      estimatedStates.push_back(m_pfFramesProcessed[frame].est);
      frame--;
    }

    std::reverse(trueStates.begin(), trueStates.end());
    std::reverse(estimatedStates.begin(), estimatedStates.end());

    std::string method;
    method = std::to_string((_Longlong)this->m_nParticles->GetValue());
    if(this->m_pfMethod->GetValue() == QtEnums::PFM_FULL_STATE)
      method = method+"FullState";
    else
      method = method+"Marginalized";

    char temp[200] = {0};
    sprintf(temp, "%s/%sGroundTruth", directory, method.c_str());
    saveTipHistory(temp, trueStates);
    sprintf(temp, "%s/%sEstimated", directory,  method.c_str());
    saveTipHistory(temp, estimatedStates);

    std::vector < NSCommand > commands = AssembleAllCommands(origFrame);
    std::vector < Measurement > measurements = AssembleAllMeasurements(origFrame);
    arma::vec dts = AssembleAllDts(origFrame);

    sprintf(temp, "%s/", directory);
    saveCommands(temp, commands);
    saveMeasurements(temp, measurements);
    saveTimes(temp, dts);

    std::tr1::shared_ptr < PFParams > params = GetParams(origFrame);
    FILE *paramFile;
    if(this->m_pfMethod->GetValue() == QtEnums::PFM_FULL_STATE) {
      sprintf(temp, "%s/paramsFullState.dat", directory);
      paramFile = fopen(temp, "w");
      SaveParameters((PFFullStateParams *)params.get(), paramFile);
    }
    else {
      sprintf(temp, "%s/paramsMarginalized.dat", directory);
      paramFile = fopen(temp, "w");
      SaveParameters((PFMarginalizedParams *)params.get(), paramFile);
    }
    if(paramFile != NULL)
      fclose(paramFile);

  }

  void ParticleFilterVisualizer::onNVisSkipChanged()
  {
    UpdateVisualizations(m_lastFrame);
  }

  static arma::mat CurvePoints(const PolyCurve *p, s32 n, f64 desiredLength, f64 ds, arma::vec latestMeasurement, f64 maxLength = 10.0)
  {
    Polynomial pp = PolyCurveToPolynomial(p);
    f64 ta, tb;
    if(arma::norm(pp(p->dRange.x)-latestMeasurement) < arma::norm(pp(p->dRange.y)-latestMeasurement)) {
      ta = p->dRange.x;
      tb = p->dRange.y;
    } else {
      ta = p->dRange.y;
      tb = p->dRange.x;
    }
    arma::mat res = pp.UniformlySpacedPoints(ta, tb, ds, 3);
    f64 totalLength = 0;
    s32 i=1;
    while(totalLength < desiredLength && i < res.n_rows) {
      totalLength += arma::norm(res.row(i)-res.row(i-1));
      i++;
    }

    if(i < res.n_rows-1)
      res = res.rows(0,i);

    // we want directions of each measurement
    arma::mat derivs = res.rows(0,res.n_rows-2)-res.rows(1,res.n_rows-1);
    derivs = arma::join_vert(derivs, derivs.row(derivs.n_rows-1));
    arma::vec spacings = arma::zeros(derivs.n_rows);
    for(s32 i=0; i<derivs.n_rows; i++) {
      spacings(i) = arma::norm(derivs.row(i));
      derivs.row(i) = derivs.row(i)/spacings(i);
    }

    f64 dlen = arma::mean(spacings);

    for(f64 extraLen=0; extraLen <= maxLength; extraLen += dlen) {
      res = arma::join_vert(res.row(0)+derivs.row(derivs.n_rows/10)*dlen, res);
    }

    if(res.n_rows > n) {
      arma::uvec ris = arma::linspace<arma::uvec>(0, res.n_rows-1, n);
			res = res.rows(ris);
    }
    return res;
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

    if(m_pfFramesProcessed[frame].measCurve.rmsError > 0) {
      Polynomial curr = PolyCurveToPolynomial(&m_pfFramesProcessed[frame].measCurve);
      Vec2d range(m_pfFramesProcessed[frame].measCurve.dRange.x, m_pfFramesProcessed[frame].measCurve.dRange.y);
      m_measCurveVis->SetCurve(&curr, range, 0.5);

#if 0
      arma::mat curvePoints = CurvePoints(&m_pfFramesProcessed[frame].measCurve, (s32)(1*GetParams(frame)->n->GetValue()), 2*GetParams(frame)->minLength->GetValue(), 
        (m_pfFramesProcessed[frame].u.tick-m_pfFramesProcessed[frame-1].u.tick)/1000.0*m_pfFramesProcessed[frame].u.v, m_pfFramesProcessed[frame].m.pos.col(0), 0);
      std::vector < Vec3d > cpoints;
      for(s32 i=0; i<curvePoints.n_rows; i++) 
        cpoints.push_back(Vec3d(curvePoints(i,0), curvePoints(i,1), curvePoints(i,2)));
      m_curvePoints->SetPoints(cpoints);
#endif
    }

    m_update->onRepaint();
  }

  static void SaveParticleFilterState(const std::vector <NSCommand> &us, const arma::vec &dts, 
    const std::vector < Measurement > &meas, const PartMethod1 &parts, const arma::mat &curvePoints, const PFFullStateParams *p)
  {
		char *directory = "C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/";
		char basePath[200]; 
		strcpy(basePath, directory); strcat(basePath, "testCMeasure");

    saveCommands(basePath, us);
    saveTimes(basePath, dts);
    saveMeasurements(basePath, meas);
    saveParticlesMethod1(basePath, parts);
    saveCurvePoints(basePath, curvePoints);

		char temp[200];
		sprintf(temp, "%sparamsFullState.dat", directory);
		FILE *paramFile = fopen(temp, "w");
		SaveParameters(p, paramFile);
		fclose(paramFile);
  }

  static void SaveParticleFilterState(const std::vector <NSCommand> &us, const arma::vec &dts, 
    const std::vector < Measurement > &meas, const PartMethod3 &parts, const arma::mat &curvePoints, const PFMarginalizedParams *p)
  {
		char *directory = "C:/Joey/ultrasteer/src/MATLAB_CurveFit/NeedleSimulationsParticleFilter/ctests/data/";
		char basePath[200]; 
		strcpy(basePath, directory); strcat(basePath, "testCMeasure");

    saveCommands(basePath, us);
    saveTimes(basePath, dts);
    saveMeasurements(basePath, meas);
    saveParticlesMethod3(basePath, parts);
    saveCurvePoints(basePath, curvePoints);

		char temp[200];
		sprintf(temp, "%sparamsMarginalized.dat", directory);
		FILE *paramFile = fopen(temp, "w");
		SaveParameters(p, paramFile);
		fclose(paramFile);
  }

  static PartMethod1 AssembleParticles(const std::tr1::shared_ptr < ParticleFilterFullState > pf, const PFParams *p)
  {
    PartMethod1 res;

    res.pos = pf->GetParticlePositions(p);
    res.rhos = pf->GetParticleRhos(p);
    res.Rs = pf->GetParticleOrientations(p);
    res.ws = pf->GetWeights();

    return res;
  }

  static PartMethod3 AssembleParticles(const std::tr1::shared_ptr < ParticleFilterMarginalized > pf, const PFParams *p)
  {
    PartMethod3 res;

    res.pos = pf->GetParticlePositions(p);
    res.rhos = pf->GetParticleRhos(p);
    res.ws = pf->GetWeights();
    
    std::vector < arma::mat33 > Rs = pf->GetParticleOrientations(p);
    std::vector < arma::mat33 > sigmas = pf->GetParticleOrientationSigmas(p);
    res.Rs = std::vector < OrientationKF >(sigmas.size());

    for(s32 i=0; i<Rs.size(); i++) {
      res.Rs[i] = OrientationKF(Rs[i], sigmas[i]);
    }

    return res;
  }

  bool ParticleFilterVisualizer::ShouldInitialize(s32 frame)
  {
    PFMarginalizedParams *p = (PFMarginalizedParams *)GetParams(frame).get();
    if(this->m_pfMethod->GetValue() == QtEnums::PFM_FULL_STATE && !m_init && NumberOfMeasurementsUpToAndIncludingFrame(frame) >= GetParams(frame)->minimumMeasurements->GetValue())
      return true;
    else if(this->m_pfMethod->GetValue() == QtEnums::PFM_MARGINALIZED_ORIENTATION && !m_init && totalLength(AssembleAllCommands(frame), AssembleAllDts(frame)) > p->minTotalLength->GetValue())
      return true;
    return false;
  }

  void ParticleFilterVisualizer::Update(RPData *rp, s32 frame)
  {
    bool alreadyProcessed = m_pfFramesProcessed.find(frame) != m_pfFramesProcessed.end();
    m_lastFrame =  frame;
    arma::arma_rng::set_seed(frame);
    NeedleFrame doppler, bmode;

    PolyCurve curve;
    DoSegmentation(rp, doppler, bmode, &curve, alreadyProcessed);

    if(m_pf == NULL)
      Initialize(m_basePath.c_str());

    // If we've already run this data through our particle filter, bail.
    if(alreadyProcessed) {
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

    m_pfFramesProcessed[frame] = AssemblePFData(np, Matrix44d(TRANSDUCER_CALIBRATION_COEFFICIENTS), *rp, m_roc->GetValue(), curve);

    std::tr1::shared_ptr < PFParams > params = GetParams(frame);
    if(ShouldInitialize(frame)) {
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
      //NTrace("Dt %f", (m_pfFramesProcessed[frame].u.tick-m_pfFramesProcessed[frame-1].u.tick)/1000.0);

      arma::mat curvePoints;
      if(frame > 0 && curve.rmsError > 0) {
         curvePoints = CurvePoints(&m_pfFramesProcessed[frame].measCurve, (s32)(1*GetParams(frame)->n->GetValue()), 2*GetParams(frame)->minLength->GetValue(), 
          (m_pfFramesProcessed[frame].u.tick-m_pfFramesProcessed[frame-1].u.tick)/1000.0*m_pfFramesProcessed[frame].u.v, m_pfFramesProcessed[frame].m.pos.col(0));
      } 
      
      f64 tLength = totalLength(AssembleAllCommands(frame), AssembleAllDts(frame));
      if(false) {//frame == 492) {
        if(this->m_pfMethod->GetValue() == QtEnums::PFM_FULL_STATE) {
          SaveParticleFilterState(AssembleCommands(frame), AssembleDts(frame), AssembleMeasurements(frame),
            AssembleParticles(std::tr1::shared_ptr < ParticleFilterFullState > ((ParticleFilterFullState *)m_pf.get()), GetParams(-1).get()), curvePoints,
						(const PFFullStateParams *)GetParams(frame).get());
        } else {
          SaveParticleFilterState(AssembleCommands(frame), AssembleDts(frame), AssembleMeasurements(frame),
            AssembleParticles(std::tr1::shared_ptr < ParticleFilterMarginalized > ((ParticleFilterMarginalized *)m_pf.get()), GetParams(-1).get()), curvePoints,
						(const PFMarginalizedParams *)GetParams(frame).get());
        }
      }
      NTrace("Frame %d\n", frame);
      m_pf->ApplyMeasurement(AssembleMeasurements(frame), AssembleCommands(frame), AssembleDts(frame), params.get(), totalLength(AssembleAllCommands(frame), AssembleAllDts(frame)), curvePoints.t());

      // Push in estimate from particle filter
      m_pfFramesProcessed[frame].est = m_pf->GetExpectedValue(params.get());
      m_pfFramesProcessed[frame].particleRs = m_pf->GetParticleOrientations(params.get());
      m_pfFramesProcessed[frame].particlePos = m_pf->GetParticlePositions(params.get());
      m_pfFramesProcessed[frame].w = m_pf->GetWeights();
      m_pfFramesProcessed[frame].overNeedle = m_pf->GetProbOverNeedle();
      UpdateVisualizations(frame);

      m_barChartWidget->UpdateVisualization(m_pfFramesProcessed[frame]);
      m_lineChartWidget->UpdateVisualization(m_pfFramesProcessed[frame]);

      // Resample if effective number of particles drops below threshold
      f64 nParts = m_pf->EffectiveSampleSize();
      if(nParts < params->neff->GetValue()*m_pf->GetNumberOfParticles())
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


  void ParticleFilterVisualizer::AddWidgetsToLayout(QGridLayout *layout)
  {
    layout->addWidget(m_barChartWidget.get(), 0, 0);
    layout->addWidget(m_lineChartWidget.get(), 0, 1);
  }

  /////////////////////////////////////////////////////////////////////////////////
  //END PFVisualizer
  /////////////////////////////////////////////////////////////////////////////////

  EstimatorFileWidget::EstimatorFileWidget(QWidget *parent, const char *name)
    : RPFileWidget(parent, (USVisualizer *)new PFVisualizer(parent), name)
    , m_state(EFS_READY)
    , m_resultsAvailable(ERA_NONE)
    , m_pfVisualizer(new ParticleFilterVisualizer(this, parent))
    , m_screenWriter(std::tr1::shared_ptr < ScreenWriter >(new ScreenWriter(m_planeVis->GetRenderWindow())))
    , m_imageScreenWriter(std::tr1::shared_ptr < ScreenWriter >(new ScreenWriter(m_imageViewer->GetRenderWindow())))
    , m_barChartScreenWriter(std::tr1::shared_ptr < ScreenWriter >(new ScreenWriter(m_pfVisualizer->GetBarChartWidget()->GetRenderWindow())))
    , m_lineChartScreenWriter(std::tr1::shared_ptr < ScreenWriter >(new ScreenWriter(m_pfVisualizer->GetLineChartWidget()->GetRenderWindow())))
    , m_lastFrame(1)
    , m_topRow(new QGridLayout(parent))
    , m_probeVis(std::tr1::shared_ptr < STLVisualizer >(new STLVisualizer(STL_PATH_CAT("L14-5GPS.STL"))))
    , m_groundVis(std::tr1::shared_ptr < PlaneVisualizer >(new PlaneVisualizer(Vec3d(0,0,0), Vec3d(0,0,1))))
  {
    m_pfVisualizer->AddWidgetsToLayout(m_topRow.get());
    m_layout->addLayout(m_topRow.get(), 1, 0, 1, 2);

    ADD_ACTION_PARAMETER(m_doNeedleCalib, "Do Needle Calibration", CALLBACK_POINTER(onDoNeedleCalibrationPushed, EstimatorFileWidget), this, false);
    ADD_ENUM_PARAMETER(m_operationMode, "Operation Mode", CALLBACK_POINTER(onSetOperationMode, EstimatorFileWidget), this, QtEnums::EstimatorOperationMode::EOM_NONE, "EstimatorOperationMode");
    ADD_SAVE_FILE_PARAMETER(m_tipCalibPath, "Tip Calibration Save Path", NULL, this, BASE_PATH_CAT("TipCalibration/ShallowInsertion/tipCalib.mat"), "(*.mat)");
    ADD_OPEN_FILE_PARAMETER(m_tipCalibPathLoad, "Presaved Tip Calibration",CALLBACK_POINTER(onTipCalibPathChanged, EstimatorFileWidget), this, BASE_PATH_CAT("TipCalibration/ShallowInsertion/tipCalib.mat"), "(*.mat)");
    ADD_SAVE_FILE_PARAMETER(m_pointsDataPath, "Point History Save Path", NULL, this, PATH_CAT("Trial1/PreInsertionGPS/TipHistory.mat"), "(*.mat)");
    ADD_OPEN_FILE_PARAMETER(m_pointsDataPathLoad, "Presaved Point History", CALLBACK_POINTER(onPointsDataPathChanged, EstimatorFileWidget), this, PATH_CAT("Trial1/PreInsertionGPS/TipHistory.mat"), "(*.mat)");
    ADD_ACTION_PARAMETER(m_clearCalibrationData, "Clear Calibration Data", CALLBACK_POINTER(onClearCalibrationData, EstimatorFileWidget), this, false);
    ADD_ACTION_PARAMETER(m_clearTipCalibration, "Clear Tip Calibration", CALLBACK_POINTER(onClearTipCalibration, EstimatorFileWidget), this, false);
    ADD_BOOL_PARAMETER(m_addNewFrame, "Add New Calib Frame", NULL, this, false);
    ADD_CHILD_COLLECTION(m_pfVisualizer.get());
    ADD_CHILD_COLLECTION(m_screenWriter.get());
    ADD_CHILD_COLLECTION(m_barChartScreenWriter.get());
    ADD_CHILD_COLLECTION(m_lineChartScreenWriter.get());

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

    m_planeVis->GetRenderer()->AddActor(m_probeVis->GetActor());
    m_planeVis->GetRenderer()->AddActor(m_groundVis->GetActor());

    onUpdateFile();

    connect(this, SIGNAL(updateFrame()), this, SLOT(onUpdateFrame()), Qt::QueuedConnection);
  }

  EstimatorFileWidget::~EstimatorFileWidget()
  {
  }

  void EstimatorFileWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
    m_imageViewer->UpdateSize(QSize(sz.width()/2-10, (3*sz.height())/4));
    m_usVis->UpdateSize(QSize(sz.width()/2-10, (3*sz.height())/4));
    m_planeVis->UpdateSize(QSize(sz.width()/2-10, (3*sz.height())/4));
    m_pfVisualizer->GetBarChartWidget()->UpdateSize(QSize(sz.width()/4-10, sz.height()/4));
    m_pfVisualizer->GetLineChartWidget()->UpdateSize(QSize(3*sz.width()/4-10, sz.height()/4));
  }
  
  void EstimatorFileWidget::onUpdateFrame()
  {
    if(m_data.gps.valid)
      m_data.Release();

    if(m_rpReaders)
      m_data = m_rpReaders->GetRPData(m_frame->GetValue());

    if(m_resultsAvailable&ERA_NEEDLE_TIP_CALIB)
      SpoofRPDataWithNeedleTipCalibration(m_data, &m_ntCalibrator, m_frame->GetValue());
    
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
    case EFS_ESTIMATE_SCAN:
      {
        m_pfVisualizer->Update(&m_data, m_frame->GetValue());
        if(m_frame->GetValue() < m_frame->GetMax()) {
          m_frame->SetValue(m_frame->GetValue()+1);
          emit updateFrame();
        } else {
          this->onDoNeedleCalibrationPushed();
          m_state = EFS_PRIMED;
          m_operationMode->SetValue(QtEnums::EOM_NONE);
        }
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

    m_screenWriter->WriteLatest();
  }

  void EstimatorFileWidget::onUpdateFile()
  {
    RPFileWidget::onUpdateFile();

    QFileInfo fi(m_rpFile->GetValue().c_str());
    QFileInfo tpth(m_tipCalibPath->GetValue().c_str());
    
    m_pfVisualizer->SetBasePath(fi.dir().path().toStdString().c_str());

    this->m_tipCalibPath->SetValue(fi.dir().path().toStdString()+std::string("/")+tpth.fileName().toStdString());
    this->m_tipCalibPathLoad->SetValue(fi.dir().path().toStdString()+std::string("/TipCalib.mat"));
    
    bool validFile = m_frame->GetMax() > 0;
    switch(m_state) {
    case EFS_READY: 
    case EFS_PRIMED:
    case EFS_NEEDLE_TIP_CALIB:
    case EFS_NEEDLE_CURVATURE_CALIB_GPS:
    case EFS_NEEDLE_CURVATURE_CALIB_US:
    case EFS_ESTIMATE:
    case EFS_ESTIMATE_SCAN:
      {
        if(validFile)
          m_state = EFS_PRIMED;
        else
          m_state = EFS_READY;

        onClearCalibrationData();

        this->m_operationMode->SetValue(QtEnums::EOM_NONE);

        this->onRepaint();

        m_resultsAvailable = ERA_NONE;
        break;
      }
    default: 
      {
        throw std::runtime_error("EstimatorFileWidget: unknown state\n");
        break;
      }
    }

    fi = QFileInfo(m_tipCalibPathLoad->GetValue().c_str());
    if(fi.isFile() && fi.isReadable())
      this->onTipCalibPathChanged();

    if(validFile)
      m_lastFrame = m_frame->GetValue();

    Vec2d mppScale = m_data.mpp*1e-3;
    Matrix44d posePos = Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(m_data.gps.pose).GetOrientation(), m_data.gps.pos);
    
    f64 len = 500;
    Vec3d xAx = rpImageCoordToWorldCoord3(Vec2d(1,0), posePos, this->m_cal, m_data.origin, mppScale)-rpImageCoordToWorldCoord3(Vec2d(0,0), posePos, this->m_cal, m_data.origin, mppScale); 
    Vec3d yAx = rpImageCoordToWorldCoord3(Vec2d(0,1), posePos, this->m_cal, m_data.origin, mppScale)-rpImageCoordToWorldCoord3(Vec2d(0,0), posePos, this->m_cal, m_data.origin, mppScale); 
    xAx = xAx.normalized();
    yAx = yAx.normalized();
    Vec3d zAx = xAx.cross(yAx);
    m_groundVis->SetPlane(m_data.gps2.pos+yAx*40-xAx*(len/2.0)-zAx*(len/2.0), xAx*len, zAx*len);
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
    m_lastFrame = 1;
  }
  
  void EstimatorFileWidget::onClearTipCalibration()
  {
    m_ntCalibrator.ResetSolution();
    m_resultsAvailable = m_resultsAvailable&~ERA_NEEDLE_TIP_CALIB;
    onUpdateFrame();

    m_planeVis->repaint();
    m_usVis->repaint();
    m_lastFrame = 1;
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
    case EFS_ESTIMATE_SCAN:
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
            QFileInfo fi(m_rpFile->GetValue().c_str());
            m_pfVisualizer->Initialize(fi.dir().path().toStdString().c_str());
            m_state = EFS_ESTIMATE;
            break;
          }
        case QtEnums::EstimatorOperationMode::EOM_ESTIMATE_SEQUENTIAL:
          {
            QFileInfo fi(m_rpFile->GetValue().c_str());
            m_pfVisualizer->Initialize(fi.dir().path().toStdString().c_str());
            m_state = EFS_ESTIMATE_SCAN;
            m_frame->SetValue(m_frame->GetMin());
            emit updateFrame();
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

        QFileInfo fi(QString(m_tipCalibPath->GetValue().c_str()));
        m_ntCalibrator.SaveSolution((fi.dir().path()+"/"+fi.baseName()).toStdString().c_str(), fi.suffix().toStdString().c_str());

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
    case EFS_ESTIMATE_SCAN:
      {
        std::string fl = m_rpFile->GetValue();
        QFileInfo fi(fl.c_str());
        std::string dir = fi.dir().path().toStdString()+"/resultsNumParticles";
        m_pfVisualizer->SaveParticleFilterResults(m_frame->GetValue(), dir.c_str());
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
    QStringList filters;
    
    // Find all files of form basePath*.dat in the directory
    QFileInfo info(QString(m_tipCalibPathLoad->GetValue().c_str()));
    QString dirpth = info.dir().path();
    QString fname = info.baseName();
    QString extension = info.suffix();

    filters << fname+"*."+extension;
    QDir dir(dirpth);
    QStringList res = dir.entryList(filters);
    // res now holds all calibration filenames

    // need to extract calibration data and key frame from each file
    // first one is frame 1
    std::vector < s32 > keyframes; keyframes.push_back(1);
    std::vector < std::string > calibs;
    calibs.push_back(dirpth.toStdString()+"/"+res.at(0).toStdString());


    for(s32 i=1; i<res.size(); i++) {
      std::smatch sm;
      std::regex rx(".*?([0-9]+)\\."+extension.toStdString());
      std::string curr = res.at(i).toStdString();
      std::regex_match(curr, sm, rx);

      s32 frame = -1;
      if(sm.size() > 1)
        frame = atoi(sm[1].str().c_str());
      else
        throw std::runtime_error("Failed to find match\n");

      keyframes.push_back(frame);
      calibs.push_back(dirpth.toStdString()+"/"+res.at(i).toStdString());
    }

    m_ntCalibrator.SetSolution(calibs, keyframes);

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
        
        if(m_addNewFrame->GetValue()) 
          m_lastFrame = m_frame->GetValue();
        m_ntCalibrator.AddPoint(m_data.gps2.pos, Matrix44d::FromCvMat(m_data.gps2.pose).GetOrientation(), y, m_lastFrame);
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
    case EFS_ESTIMATE_SCAN:
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

      m_probeVis->GetActor()->PokeMatrix(Matrix44d::FromOrientationAndTranslation(Matrix44d::FromCvMat(m_data.gps.pose).GetOrientation(), m_data.gps.pos).GetVTKMatrix());
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
  EstimatorStreamingWidget::EstimatorStreamingWidget(QWidget *parent, const char *name)
    : RPStreamingWidget(parent, (USVisualizer *)new PFVisualizer(parent), name)
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

    disconnect(m_saveDataWidget->ui.saveDataButton, SIGNAL(clicked()), 0, 0);
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
    m_u.tick = rp.b8->BorderMode[0];
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

    SpoofRPDataWithNeedleTipCalibration(rp, &m_ntCalibrator, 1);

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