#pragma once

#include "ParticleFilter.h"

using namespace arma;
namespace Nf
{
  using ::s32;
  using ::u8;

  PFParams::PFParams(Vec2d mpp)
  {
    vec dd;

    dd << PI << PI << PI << endr;
    initOrientationSigma = (mat33)diagmat(1e-4*dd);
    initOrientationMu = zeros<vec>(3);

    dd.clear(); dd << 5 << 5 << 5 << endr;
    initPosSigma = (mat33)diagmat(dd);
    initPosMu = zeros<vec>(3);

    initRhoSigma = 0;
    initRhoMu = 0;
    
    particleMuPos = zeros<vec>(3);
    dd.clear(); dd << 1 << 1 << 1 << endr;
    particleSigmaPos = (mat33)diagmat(dd/10.0);

    particleMuOr = zeros<vec>(3);
    dd.clear(); dd << PI << PI << PI << endr;
    particleSigmaOr = (mat33)diagmat(1.0/5000.0*dd);

    particleSigmaVel = 5;
    particleMuVel = 0;

    particleSigmaRho = 3;
    particleMuRho = 0;

    this->mpp = mpp;

    measurementOffsetSigma << mpp.x*5*1e-3 << endr << mpp.y*5*1e-3;

    offNeedleDopplerMu = 0.56;                                           
    offNeedleDopplerSigma = 0.75;                                        
    onNeedleDopplerMu = 2.33;                                            
    onNeedleDopplerSigma = 0.098;  

    minimumMeasurements = 1;

    usw = mpp.x*630*1e-3;
    ush = mpp.y*480*1e-3;

    sigB0 = -2.37901785297659;
    sigB1 = 0.0534736687985484;
    offFrameB0 = -5;
    offFrameB1 = 20;

    n = 50;
    neff = 0.5;

    onNeedleDopplerLUTPath = std::string(PATH_CAT("Trial3/Insertion/pdopoverneedle.dat"));
    offNeedleDopplerLUTPath = std::string(PATH_CAT("Trial3/Insertion/pdopnotoverneedle.dat"));
  }

  PFFullStateParams::PFFullStateParams(Vec2d mpp)
    : PFParams(mpp)
  {
  }

  PFMarginalizedParams::PFMarginalizedParams(Vec2d mpp)
    : PFParams(mpp)
  {
    minimumMeasurements = 15;
    vec3 dd; dd << 5e-3 << 5e-3 << 0.5 << endr;
    measurementSigma = (mat33)diagmat(dd);

    distanceThreshSq = 5*5;
    subsetSize = 15;
    procrustesIt = 3;
  }

  TipState TipState::PropagateLength(const NSCommand &u, f64 dl, const PFParams *p)
  {
    TipState res;

    //k' coordinate system expressed in global coordinates.
    // note rotation order is flipped because using intrinsic rotations with
    // extrinisc rotation matrices
    mat33 kp = this->R*Rz(u.dtheta);

    // location of tip expressed in Rk' coordinates
    f64 phi = dl/this->rho;
    vec3 kp_x; 
    kp_x << 0 << endr << this->rho*(1-cos(phi)) << endr << this->rho*sin(phi) << endr;


    // new coordinate frame expressed in global coordinates.
    // new coordinate frame is old coordinate frame rhoated by u.dtheta around
    // needle tip's z-axis then rotated about the needle tip's x axis due to
    // following the curved arc.  Note taht rotation order is flipped due to
    //  intrinsic axis rotations used rather than extrinsic.
    res.R = (mat33)(kp*Rx(-phi));

    // convert needle tip location into global coordinates
    res.pos = kp*kp_x+this->pos;
    // pass through rho
    res.rho = this->rho;

    return res;
  }

  TipState TipState::Propagate(const NSCommand &u, f64 dt, const PFParams *p)
  {
    return PropagateLength(u, u.v*dt, p);
  }

  std::vector < TipState > TipState::PropagateBack(const std::vector < NSCommand > &u, const vec &dts, const PFParams *p)
  {
    std::vector < TipState > res;
    //push back current state
    //res[0] = current state
    res.push_back(*this);

    //u[0] = current control input
    NSCommand uc = u[0];
    uc.dtheta = 0;

    f64 dt = dts(0);

    TipState xc;
    for(s32 i=1; i<u.size(); i++) {
      xc = res[i-1]; 
      // reverse for propagating backward in time
      xc.R = (mat33)(xc.R*Ry(PI));

      xc = xc.Propagate(uc,dt,p);

      xc.R = (mat33)(xc.R*Ry(PI));
      res.push_back(xc);
      uc = u[i];
      uc.dtheta = u[i-1].dtheta;
      dt = dts(i);
    }

    return res;
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////
  /// Begin Helper Functions
  //////////////////////////////////////////////////////////////////////////////////////////
  
  // convert a vector of tip histories to a matrix of points
  // return : 
  // [x1 ... xn] \in R^(3xn) x_i \in R^3
  static mat tipHistoryToPointMatrix(const std::vector < TipState > &hist)
  {
    using ::s32;

    mat res = zeros(3,hist.size());
    for(s32 i=0; i<hist.size(); i++) {
      res.col(i) = hist[i].pos;
    }

    return res;
  }

  // convert a vector of measurement histories to a matrix of points
  // return : 
  // [x1 ... xn] \in R^(3xn) x_i \in R^3
  static mat measurementToPointMat(const std::vector < Measurement > &meas)
  {
    using ::s32;

    mat res = zeros(3,meas.size());
    for(s32 i=0; i<meas.size(); i++) {
      res.col(i) = meas[i].pos;
    }

    return res;
  }

  static f64 sigmoid(f64 x, f64 b0, f64 b1)
  {
    return 1/(1+exp(-b0-x*b1));
  }

  //X = [x1 x2 ... xn] x_i \in R^3
  //Y = [y1 y2 ... ym] y_i \in R^3
  //D = D_ij = |(x_i-y_j)|^2
  static mat distanceMatrix(const mat &X, const mat &Y)
  {
    using ::s32;

    s32 m = X.n_cols;
    s32 n = Y.n_cols;

    //mat D = repmat(((mat)(X.t()*X)).diag(), 1, n)-2*X.t()*Y+repmat(((mat)(Y.t()*Y)).diag().t(), m, 1);
    mat D = zeros(m,n);
    vec3 delta;
    for(s32 i=0; i<m; i++) {
      for(s32 j=0; j<n; j++) {
        delta = (vec3)(X.col(i) - Y.col(j));
        D(i,j) = delta(0)*delta(0)+delta(1)*delta(1)+delta(2)*delta(2);
      }
    }

    return D;
  }

  // sample without replacmenet
  static uvec sample(uvec x, s32 n)
  {
    using ::s32;

    uvec seq = linspace<uvec>(0, x.n_rows-1, x.n_rows);
    seq = shuffle(seq);
    
    uvec res(n, 1);
    for(s32 i=0; i<n; i++)
      res(i) = x(seq(i));

    return res;
  }

  //model = current state projected back
  //model = [x1 x2 ... xn] \in R^(3xn), x1 = current point, ... xn = point n timesteps ago
  //measurements = [x1 x2 ... xm] \in R^(3xm), x1 = current measurement point, ..., xm = measurement m timesteps ago
  static mat33 optimalRotationForModel(mat model, mat measurements, const mat &offset, const PFParams *p)
  {
    using ::s32;

    const PFMarginalizedParams *pfm = (const PFMarginalizedParams *)p;

    //offset so that model.col(0) is origin (we rotate about that point)
    measurements = measurements-repmat(offset,1,measurements.n_cols);

    mat33 R = eye(3,3);

    mat cTemplate = model;

    vec rotZ; rotZ << 1 << 1 << -1 << endr;
    mat33 reflect = diagmat(rotZ);
    
    uvec minTemplate, goodDs, shuf;
    mat D,U,V,X,Y,XYt,dR;
    vec S, minD;
    s32 mm = 0;
    for(s32 i=0; i<pfm->procrustesIt; i++) {
      //D_ij = distanceSq(measurements(i), cTemplate(j))
#if 1
      D = distanceMatrix(measurements,cTemplate);
      //D = ones(measurements.n_cols, cTemplate.n_cols);

      minD = min(D, 1);
      for(s32 r=0; r<D.n_rows; r++) {
        minTemplate = join_vert(minTemplate,find(D.row(r) == minD(r)));
      }
      goodDs = find(minD < pfm->distanceThreshSq);
      goodDs = sample(goodDs, MIN(pfm->subsetSize, goodDs.n_rows));
      //goodDs = goodDs.subvec(span(0,MAX(MIN(pfm->subsetSize-1, goodDs.n_rows-1),0)));
      //goodDs = sort(goodDs);

      minTemplate = minTemplate.rows(goodDs);

      X = cTemplate.cols(minTemplate);
      Y = measurements.cols(goodDs);
#else 
      mm = MIN(MIN(pfm->subsetSize, measurements.n_cols),cTemplate.n_cols);

      // X = cTemplate.cols(span(0,mm-1));
	  // Y = measurements.cols(span(0,mm-1));
      X = randu(3, mm);
      Y = randu(3, mm);
#endif
      XYt = X*Y.t();
      svd(U,S,V,XYt);
      dR = V*U.t();
      if(det(dR) < 0) {
        dR = V*reflect*U.t();
      }
      R = (mat33)(dR*R);
      cTemplate = dR*cTemplate;
      minTemplate.clear();
    }
    return R;
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  /// End Helper Functions
  //////////////////////////////////////////////////////////////////////////////////////////
  
  //////////////////////////////////////////////////////////////////////////////////////////
  /// Begin LUT Distribution function
  //////////////////////////////////////////////////////////////////////////////////////////
  LUTDist::LUTDist(const char *path)
  {
    arma::mat temp;
    temp.load(path);

    m_x = (vec)temp.row(0).t();
    m_p = (vec)temp.row(1).t();
  }

  f64 LUTDist::P(f64 x) const
  {
    vec dist = abs(m_x-ones(m_x.n_elem)*x);
    u32 minIdx = 0;
    dist.min(minIdx);
    return m_p(minIdx);
  }
  //////////////////////////////////////////////////////////////////////////////////////////
  /// End LUT Distribution function
  //////////////////////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////////////////////
  /// Begin Basic Particle Filter
  //////////////////////////////////////////////////////////////////////////////////////////
  ParticleFilter::ParticleFilter(s32 nParticles, const char *name, 
    std::tr1::shared_ptr < Distribution > pDopOverNeedle, std::tr1::shared_ptr < Distribution > pDopNotOverNeedle, const PFParams *p)
    : m_nParticles(nParticles)
    , m_pDopOverNeedle(pDopOverNeedle)
    , m_pDopNotOverNeedle(pDopNotOverNeedle)
  {
    m_w = ones(1,m_nParticles)/(f64)m_nParticles;
  }

  ParticleFilter::~ParticleFilter()
  {
  }

  void ParticleFilter::SetNumberOfParticles(s32 nParticles, const PFParams *p)
  {
    Resample(nParticles, p);
    m_nParticles = nParticles;
  }

  void ParticleFilter::SetWeights(const arma::mat &w)
  {
    m_w = w;
  }

  mat ParticleFilter::GetWeights()
  {
    return m_w;
  }


  f64 ParticleFilter::EffectiveSampleSize()
  {
    mat neff = 1/(m_w*m_w.t());
    return neff(0,0);
  }

  void ParticleFilter::ApplyWeights(const arma::mat &pw)
  {
    m_w = m_w%pw;
    mat tot = sum(m_w,1);
    m_w = m_w/tot(0,0);
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  /// End Basic Particle Filter
  //////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////////////////
  /// Begin Particle Filter Full State
  //////////////////////////////////////////////////////////////////////////////////////////
  ParticleFilterFullState::ParticleFilterFullState(s32 nParticles, std::tr1::shared_ptr < Distribution > pDopOverNeedle, std::tr1::shared_ptr < Distribution > pDopNotOverNeedle, const PFParams *p)
    : ParticleFilter(nParticles, "ParticleFilterFullState", pDopOverNeedle, pDopNotOverNeedle, p)
  {
  }

  ParticleFilterFullState::~ParticleFilterFullState()
  {
  }

  void ParticleFilterFullState::InitializeParticleFilter(const std::vector < TipState > &hist, const PFParams *p)
  {
    const PFFullStateParams *pp = (const PFFullStateParams *)p;

    TipState xc = hist[0];
    Gaussian<vec3,mat33> posNoise((vec3)(xc.R*p->initPosMu), p->initPosSigma);
    Gaussian<vec,mat> rhoNoise(vec(&p->initRhoMu,1), mat(&p->initRhoSigma,1,1));
    Gaussian<vec3,mat33> orNoise(p->initOrientationMu, p->initOrientationSigma);

    m_rho = xc.rho*ones(1,m_nParticles);
    m_rho = m_rho+rhoNoise.Sample(m_nParticles);
    m_rho = max(PF_MIN_RHO*ones(1,m_nParticles), m_rho);

    m_pos = repmat(xc.pos, 1, m_nParticles)+posNoise.Sample(m_nParticles);

    for(s32 i=0; i<m_nParticles; i++) {
      m_R.push_back((mat33)(xc.R*SO3Exp(orNoise.Sample())));
    }

  }

  void ParticleFilterFullState::Propagate(const NSCommand *u, f64 dt, const PFParams *p)
  {
    //TODO: This can be optimized to not propagate each particle separately
    //Especially if we get rid of velocity randomness term.
    //All orientations changes can be combined into one matrix multiply for all particles, etc.

    PFFullStateParams *pp = (PFFullStateParams *)p;
    Gaussian<vec3, mat33> noisePos(p->particleMuPos, p->particleSigmaPos);
    Gaussian<vec3, mat33> noiseOrientation(p->particleMuOr, p->particleSigmaOr);
    Gaussian<vec, mat> noiseRho(vec(&p->particleMuRho,1), mat(&p->particleSigmaRho,1,1));
    Gaussian<vec, mat> noiseVel(vec(&p->particleMuVel,1), mat(&p->particleSigmaVel,1,1));

    mat noiseP = noisePos.Sample(m_nParticles);
    mat noiseO = noiseOrientation.Sample(m_nParticles);
    mat noiseR = noiseRho.Sample(m_nParticles);
    mat noiseV = noiseVel.Sample(m_nParticles);

    TipState t;
    NSCommand uc; memcpy(&uc, u, sizeof(NSCommand));
    mat velocities = u->v*ones(1,m_nParticles)+noiseV;
    //For each particle...
    for(s32 i=0; i<m_nParticles; i++) {
      //perturb orientation
      uc.v = velocities(0,i);
      t.pos = m_pos.col(i);
      t.R = (mat33)(m_R[i]*SO3Exp((vec3)noiseO.col(i)));
      t.rho = m_rho(i);

      //Propagate
      t = t.Propagate(uc, dt, p);

      m_pos.col(i) = t.pos;
      m_rho(i) = t.rho;
      m_R[i] = t.R;
    }

    //now add noise
    m_pos = m_pos+noiseP;
    m_rho = max(m_rho+noiseR, PF_MIN_RHO*ones(1,m_nParticles));
  }

  void ParticleFilterFullState::ApplyMeasurement(const std::vector < Measurement > &m, const std::vector < NSCommand > &u, const vec &dts, const PFParams *p)
  {
    const PFFullStateParams *params = (const PFFullStateParams *)p;

    // project first particle state backward in time as template 
    // each particle will rotate and offset this history based on its orientation
    TipState t;
    t.pos = m_pos.col(0);
    t.R = m_R[0];
    t.rho = m_rho(0, 0);
    std::vector < TipState > ts = t.PropagateBack(u, dts, p);
    
    // backward projected points from particle 0.
    // this will be adjusted for each particle
    mat modelHist = tipHistoryToPointMatrix(ts);
    mat cModelHist = zeros(modelHist.n_rows, modelHist.n_cols);
    
    mat33 Rdelta;
    mat33 frameMat;
    mat33 A, invA;
    mat ds;
    vec2 suv, duv, a, b;
    f64 p_uvxOffFrame, p_uvxOnFrame, p_dxOffFrame, p_dxOnFrame, p_offFrame, pin, dop;

    f64 minVal, maxVal, minAbsVal;
    uword minAbsIdx;

    vec usFrameParams;
    usFrameParams << params->usw << endr << params->ush << endr;

    mat pw = zeros(1,m_nParticles);

    // For projecting particle position onto ultrasound frame
    A.col(0) = m[0].fbx; A.col(1) = m[0].fby; A.col(2) = cross(m[0].fbx, m[0].fby);
    invA = (mat33)inv(A);

    s32 sum1, sum2;

    // For each particle...
    for(s32 i=0; i<m_nParticles; i++) {
      //delta rotation from particle 0 to particle i
      Rdelta = (mat33)(m_R[i]*m_R[0].t());

      dop = m[0].doppler(0,0);

      // ultrasound frame behind the needle tip
      //subtract off first point so we rotate about it
      cModelHist = modelHist-repmat(modelHist.col(0),1,modelHist.n_cols);
      // rotate history by Rdelta
      cModelHist = Rdelta*cModelHist;
      // offset so that first point is now particle i point
      cModelHist = cModelHist+repmat(m_pos.col(i),1,cModelHist.n_cols);

      // find flagella point with minimum distance projection onto ultrasound frame
      ds = zeros(3,cModelHist.n_cols);

      // for each history point...
      for(s32 j=0; j<cModelHist.n_cols; j++) {
        ds.col(j) = invA*(cModelHist.col(j)-m[0].ful);
      }


      // if point projections were outside frame, get rid of them
      s32 j = 0;
      while(j < ds.n_cols) {
        // did it not intersect the frame then set the projection distance to extremely high.
        if((sum(sum(zeros(2,1) <= ds.submat(span(0,1), span(j,j)))) == 2) && sum(sum(ds.submat(span(0,1), span(j,j)) <= usFrameParams)) == 2)
          j++;
        else
          ds.shed_col(j);
      }

      if(ds.n_cols == 0) {
        p_offFrame = 1.0; //no part of particle flagella intersects frame, so just set p(offFrame) = 1 and p_uvxOnFrame = 0
        p_uvxOnFrame = 0;
      } else {

        minVal = ds.row(2).min();
        maxVal = ds.row(2).max();
        minAbsVal = abs(ds.row(2)).min(minAbsIdx);

        // if projection value changes from positive to negative, then the frame intersects the particle flagella
        if(minVal < 0 && maxVal > 0)
          minAbsVal = 0;

        // p(off frame | distance of projection)
        p_offFrame = sigmoid(minAbsVal, params->offFrameB0, params->offFrameB1);

        // gaussian centered at measurement intersections
        suv = (vec2)(ds.submat(span(0,1), span(minAbsIdx,minAbsIdx)));

        // measurement.uv = measurement in image coordinaates in (mm)
        // p((u,v)|d,x) ~ truncated gaussian centered at shaft particle interesection
        // calculate limits for truncated gaussian
        a = suv-usFrameParams;  // if shaft (u,v) - (u,v) < shaft (u,v) - br, then  (u,v) > br
        b = suv;                                            // if shaft (u,v) - (u,v) > shaft (u,v), then (u,v) < (0,0) 

        duv = suv-m[0].uv.col(0);

        // calculate p(frame interesects with flagella|doppler)
        pin = sigmoid(m[0].doppler(0,0), params->sigB0, params->sigB1);
        p_uvxOnFrame = pin*TruncatedIndependentGaussianPDF2(duv, (vec2)zeros(2,1), params->measurementOffsetSigma, a, b)+
          (1-pin)*(1/(params->ush*params->usw));
      }

      p_uvxOffFrame = 1/(params->ush*params->usw);

      //p(doppler | over needle)
      p_dxOnFrame  = m_pDopOverNeedle->P(dop);

      //p(doppler | not over needle)
      p_dxOffFrame = m_pDopNotOverNeedle->P(dop);

      pw(0,i) = (p_uvxOnFrame*p_dxOnFrame)*(1-p_offFrame)+(p_uvxOffFrame*p_dxOffFrame*p_offFrame);
    }

    ApplyWeights(pw);
  }

  mat ParticleFilterFullState::GetParticlePositions(const PFParams *p)
  {
    return m_pos;
  }

  std::vector < mat33 > ParticleFilterFullState::GetParticleOrientations(const PFParams *p)
  {
    return m_R;
  }

  mat ParticleFilterFullState::GetParticleRhos(const PFParams *p)
  {
    return m_rho;
  }

  TipState ParticleFilterFullState::GetExpectedValue(const PFParams *p)
  {
    TipState res;
    res.pos = (vec3)((m_w*m_pos.t()).t());
    res.R = SO3Mean(m_R, m_w, 1e-2);
    res.rho = ((mat)(m_w*m_rho.t()))(0,0);

    return res;
  }

  void ParticleFilterFullState::Resample(s32 nParticles, const PFParams *p)
  {
    uvec pids = Sample(m_w.t(), m_nParticles);
    m_pos = m_pos.cols(pids);
    m_rho = m_rho.cols(pids);

    std::vector <mat33> newRs(m_nParticles);
    for(s32 i=0; i<m_nParticles; i++)
      newRs[i] = m_R[pids(i)];
    m_R = newRs;
    m_w = ones(1,m_nParticles)/(f64)m_nParticles;
  }
  
  void ParticleFilterFullState::SetOrientations(const std::vector < arma::mat33 > &ors)
  {
    m_R = ors;
  }

  void ParticleFilterFullState::SetPositions(const arma::mat &pos)
  {
    m_pos = pos;
  }

  void ParticleFilterFullState::SetRhos(const arma::mat &rhos)
  {
    m_rho = rhos;
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  /// End Particle Filter Full State
  //////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////////////////
  /// Begin Particle Filter With Kalman Filter for Orientation
  //////////////////////////////////////////////////////////////////////////////////////////
  ParticleFilterMarginalized::ParticleFilterMarginalized(s32 nParticles, std::tr1::shared_ptr < Distribution > pDopOverNeedle, std::tr1::shared_ptr < Distribution > pDopNotOverNeedle, const PFParams *p)
    : ParticleFilter(nParticles, "MarginalizedParticleFilter", pDopOverNeedle, pDopNotOverNeedle, p)
  {
  }

  ParticleFilterMarginalized::~ParticleFilterMarginalized()
  {
  }

  void ParticleFilterMarginalized::InitializeParticleFilter(const std::vector < TipState > &hist, const PFParams *p)
  {
    const PFFullStateParams *pp = (const PFFullStateParams *)p;

    TipState xc = hist[0];
    Gaussian<vec3,mat33> posNoise((vec3)(xc.R*p->initPosMu), p->initPosSigma);
    Gaussian<vec,mat> rhoNoise(vec(&p->initRhoMu,1), mat(&p->initRhoSigma,1,1));

    m_rho = xc.rho*ones(1,m_nParticles)+rhoNoise.Sample(m_nParticles);
    m_rho = max(PF_MIN_RHO*ones(1,m_nParticles), m_rho);

    m_pos = repmat(xc.pos, 1, m_nParticles)+posNoise.Sample(m_nParticles);

    for(s32 i=0; i<m_nParticles; i++) {
      m_R.push_back(OrientationKF(xc.R, p->initOrientationSigma));
    }
  }

  void ParticleFilterMarginalized::Propagate(const NSCommand *u, f64 dt, const PFParams *p)
  {
    //TODO: This can be optimized to not propagate each particle separately
    //Especially if we get rid of velocity randomness term.
    //All orientations changes can be combined into one matrix multiply for all particles, etc.

    const PFMarginalizedParams *pp = (const PFMarginalizedParams *)p;
    Gaussian<vec3, mat33> noisePos(p->particleMuPos, p->particleSigmaPos);
    Gaussian<vec, mat> noiseRho(vec(&p->particleMuRho,1), mat(&p->particleSigmaRho,1,1));
    Gaussian<vec, mat> noiseVel(vec(&p->particleMuVel,1), mat(&p->particleSigmaVel,1,1));

    mat noiseP = noisePos.Sample(m_nParticles);
    mat noiseR = noiseRho.Sample(m_nParticles);
    mat noiseV = noiseVel.Sample(m_nParticles);

    TipState t;
    NSCommand uc; memcpy(&uc, u, sizeof(NSCommand));
    mat velocities = u->v*ones(1,m_nParticles)+noiseV;

    mat33 sigma1 = p->particleSigmaOr;
    mat33 sigma0;

    f64 phi;
    mat33 R10, R1, R0;
    //For each particle...
    for(s32 i=0; i<m_nParticles; i++) {
      Gaussian<vec3,mat33> noiseOr((vec3)zeros<vec>(3,1), m_R[i].sigma);
      //perturb orientation
      uc.v = velocities(0,i);
      t.pos = m_pos.col(i);
      t.R = (mat33)(m_R[i].mu*SO3Exp(noiseOr.Sample()));
      t.rho = m_rho(i);

      //Propagate
      t = t.Propagate(uc, dt, p);

      m_pos.col(i) = t.pos;
      m_rho(i) = t.rho;

      phi = uc.v*dt/m_rho(i);
      R0 = m_R[i].mu;
      R1 = (mat33)(Rz(uc.dtheta)*Rx(-phi));
      R10 = (mat33)(R0*R1);

      m_R[i] = OrientationKF(R10, sigma1+R1*m_R[i].sigma*R1.t());
    }

    //now add noise
    m_pos = m_pos+noiseP;
    m_rho = max(m_rho+noiseR,ones(1,m_nParticles)*PF_MIN_RHO);
  }

  void ParticleFilterMarginalized::ApplyMeasurement(const std::vector < Measurement > &m, const std::vector < NSCommand > &u, const vec &dts, const PFParams *p)
  {
    const PFMarginalizedParams *params = (const PFMarginalizedParams *)p;

    // project first particle state backward in time as template 
    // each particle will rotate and offset this history based on its orientation
    TipState t;
    t.pos = m_pos.col(0);
    t.R = m_R[0].mu;
    t.rho = m_rho(0, 0);
    std::vector < TipState > ts = t.PropagateBack(u, dts, p);
    

    mat33 A, invA;
    mat ds;

    // backward projected points from particle 0.
    // this will be adjusted for each particle
    mat modelHist = tipHistoryToPointMatrix(ts);
    mat cModelHist = zeros(modelHist.n_rows, modelHist.n_cols);
    //subtract off first point so we rotate about it
    mat zModelHist = modelHist-repmat(modelHist.col(0),1,modelHist.n_cols);
    
    mat33 Rdelta, frameMat, Rmeas, measSigma,K,sigmaC,Rc,Rprior;

    // For projecting particle position onto ultrasound frame
    A.col(0) = m[0].fbx; A.col(1) = m[0].fby; A.col(2) = cross(m[0].fbx, m[0].fby);
    invA = (mat33)inv(A);

    vec3 v;
    vec2 suv, duv, a, b;
    f64 p_uvxOffFrame, p_uvxOnFrame, p_dxOffFrame, p_dxOnFrame, p_offFrame, pin, dop;
    f64 minVal, maxVal, minAbsVal;
    uword minAbsIdx;

    vec usFrameParams;
    usFrameParams << params->usw << endr << params->ush << endr;

    mat pw = zeros(1,m_nParticles);
    mat meas = measurementToPointMat(m);

    dop = m[0].doppler(0,0);

    // For each particle...
    for(s32 i=0; i<m_nParticles; i++) {
      Rprior = m_R[i].mu;
      //delta rotation from particle 0 to particle i
      Rdelta = (mat33)(m_R[i].mu*t.R.t());

      // rotate history by Rdelta
      cModelHist = Rdelta*zModelHist;

      assert(m.size() >= params->minimumMeasurements);
      
      Rmeas = optimalRotationForModel(cModelHist, meas, m_pos.col(i), p);
      Rmeas = (mat33)(Rmeas*m_R[i].mu);

      if(det(params->measurementSigma) < 1e-6) {
        // zero measurement noise? just use measurement then
        m_R[i].mu = Rmeas;
        m_R[i].sigma.zeros();
      } else {
        measSigma = params->measurementSigma;
        K = (mat33)(m_R[i].sigma*inv(m_R[i].sigma+measSigma));
        sigmaC = (mat33)((eye(3,3)-K)*m_R[i].sigma);
        v = SO3Log(Rprior.t()*Rmeas);
        Rc = (mat33)(Rprior*SO3Exp(K*v));
        m_R[i] = OrientationKF(Rc, sigmaC);
      }

      Rdelta = (mat33)(m_R[i].mu*t.R.t());

      cModelHist = Rdelta*zModelHist;
      // offset so that first point is now particle i point
      cModelHist = cModelHist+repmat(m_pos.col(i),1,cModelHist.n_cols);

      // find flagella point with minimum distance projection onto ultrasound frame
      ds = zeros(3,cModelHist.n_cols);

      // for each history point...
      for(s32 j=0; j<cModelHist.n_cols; j++) {
        ds.col(j) = invA*(cModelHist.col(j)-m[0].ful);
      }


      // if point projections were outside frame, get rid of them
      s32 j = 0;
      while(j < ds.n_cols) {
        // did it not intersect the frame then set the projection distance to extremely high.
        if((sum(sum(zeros(2,1) <= ds.submat(span(0,1), span(j,j)))) == 2) && sum(sum(ds.submat(span(0,1), span(j,j)) <= usFrameParams)) == 2)
          j++;
        else
          ds.shed_col(j);
      }

      if(ds.n_cols == 0) {
        p_offFrame = 1.0; //no part of particle flagella intersects frame, so just set p(offFrame) = 1 and p_uvxOnFrame = 0
        p_uvxOnFrame = 0;
      } else {

        minVal = ds.row(2).min();
        maxVal = ds.row(2).max();
        minAbsVal = abs(ds.row(2)).min(minAbsIdx);

        // if projection value changes from positive to negative, then the frame intersects the particle flagella
        if(minVal < 0 && maxVal > 0)
          minAbsVal = 0;

        // p(off frame | distance of projection)
        p_offFrame = sigmoid(minAbsVal, params->offFrameB0, params->offFrameB1);

        // gaussian centered at measurement intersections
        suv = (vec2)(ds.submat(span(0,1), span(minAbsIdx,minAbsIdx)));

        // measurement.uv = measurement in image coordinaates in (mm)
        // p((u,v)|d,x) ~ truncated gaussian centered at shaft particle interesection
        // calculate limits for truncated gaussian
        a = suv-usFrameParams;  // if shaft (u,v) - (u,v) < shaft (u,v) - br, then  (u,v) > br
        b = suv;                                            // if shaft (u,v) - (u,v) > shaft (u,v), then (u,v) < (0,0) 

        duv = suv-m[0].uv.col(0);

        // calculate p(frame interesects with flagella|doppler)
        pin = sigmoid(m[0].doppler(0,0), params->sigB0, params->sigB1);
        p_uvxOnFrame = pin*TruncatedIndependentGaussianPDF2(duv, (vec2)zeros(2,1), params->measurementOffsetSigma, a, b)+
          (1-pin)*(1/(params->ush*params->usw));
      }

      p_uvxOffFrame = 1/(params->ush*params->usw);

      //p(doppler | over needle)
      p_dxOnFrame  = m_pDopOverNeedle->P(dop);

      //p(doppler | not over needle)
      p_dxOffFrame = m_pDopNotOverNeedle->P(dop);

      pw(0,i) = (p_uvxOnFrame*p_dxOnFrame)*(1-p_offFrame)+(p_uvxOffFrame*p_dxOffFrame*p_offFrame);
    }
    mat sm = sum(pw, 1);
    pw = pw/sm(0,0);
    ApplyWeights(pw);
  }

  mat ParticleFilterMarginalized::GetParticlePositions(const PFParams *p)
  {
    return m_pos;
  }

  std::vector < mat33 > ParticleFilterMarginalized::GetParticleOrientations(const PFParams *p)
  {
    std::vector < mat33 > res;
    for(s32 i=0; i<m_nParticles; i++) {
      res.push_back(m_R[i].mu);
    }
    return res;
  }

  std::vector < mat33 > ParticleFilterMarginalized::GetParticleOrientationSigmas(const PFParams *p)
  {
    std::vector < mat33 > res;
    for(s32 i=0; i<m_nParticles; i++) {
      res.push_back(m_R[i].sigma);
    }
    return res;
  }

  mat ParticleFilterMarginalized::GetParticleRhos(const PFParams *p)
  {
    return m_rho;
  }

  TipState ParticleFilterMarginalized::GetExpectedValue(const PFParams *p)
  {
    TipState res;
    res.pos = (vec3)((m_w*m_pos.t()).t());

    std::vector < mat33 > Rs(m_R.size());
    for(s32 i=0; i<m_R.size(); i++) 
      Rs[i] = m_R[i].mu;

    res.R = SO3Mean(Rs, m_w, 1e-2);
    res.rho = ((mat)(m_w*m_rho.t()))(0,0);

    return res;

    return TipState();
  }

  void ParticleFilterMarginalized::Resample(s32 nParticles, const PFParams *p)
  {
    uvec pids = Sample(m_w.t(), m_nParticles);
    m_pos = m_pos.cols(pids);
    m_rho = m_rho.cols(pids);

    std::vector <OrientationKF> newRs(m_nParticles);
    for(s32 i=0; i<m_nParticles; i++)
      newRs[i] = m_R[pids(i)];
    m_R = newRs;
    m_w = ones(1,m_nParticles)/(f64)m_nParticles;
  }

  void ParticleFilterMarginalized::SetOrientationKFs(const std::vector < OrientationKF > &ors)
  {
    m_R = ors;
  }

  void ParticleFilterMarginalized::SetPositions(const arma::mat &pos)
  {
    m_pos = pos;
  }

  void ParticleFilterMarginalized::SetRhos(const arma::mat &rhos)
  {
    m_rho = rhos;
  }
  //////////////////////////////////////////////////////////////////////////////////////////
  /// End Particle Filter With Kalman Filter for Orientation
  //////////////////////////////////////////////////////////////////////////////////////////

  
  // Assumes one point per measurement
  std::vector < Measurement > loadMeasurements(const char *basePath, bool reverse)
  {
    using ::s32;
    std::vector < Measurement > res;

    char path[150] = {0};

    sprintf(path, "%sMeasDoppler.dat", basePath);
    mat doppler; doppler.load(path);
    sprintf(path, "%sMeasPos.dat", basePath);
    mat pos; pos.load(path);

    sprintf(path, "%sMeasFul.dat", basePath);
    mat ful; ful.load(path);
    sprintf(path, "%sMeasFbl.dat", basePath);
    mat fbl; fbl.load(path);
    sprintf(path, "%sMeasFbr.dat", basePath);
    mat fbr; fbr.load(path);
    sprintf(path, "%sMeasFur.dat", basePath);
    mat fur; fur.load(path);

    sprintf(path, "%sMeasBx.dat", basePath);
    mat bx; bx.load(path);
    sprintf(path, "%sMeasBy.dat", basePath);
    mat by; by.load(path);

    sprintf(path, "%sMeasUv.dat", basePath);
    mat uv; uv.load(path);

    for(s32 i=0; i<doppler.n_cols; i++) {
      Measurement m;
      m.doppler = doppler.col(i);

      m.pos = (vec)pos.col(i);

      m.ful = (vec)ful.col(i);
      m.fbl = (vec)fbl.col(i);
      m.fbr = (vec)fbr.col(i);
      m.fur = (vec)fur.col(i);

      m.fbx = (vec)bx.col(i);
      m.fby = (vec)by.col(i);

      m.uv = (mat)uv.col(i);

      res.push_back(m);
    }
    if(reverse)
      std::reverse(res.begin(), res.end());
    return res;
  }

  
  void saveMeasurements(const char *basePath, const std::vector < Measurement > &measurements)
  {
    using ::s32;

    mat doppler, pos, ful, fbl, fbr, fur, fbx, fby, uv; 
    for(s32 i=0; i<measurements.size(); i++) {
      doppler = join_horiz(doppler, measurements[i].doppler);
      pos = join_horiz(pos, measurements[i].pos);
      ful = join_horiz(ful, measurements[i].ful);
      fbl = join_horiz(fbl, measurements[i].fbl);
      fbr = join_horiz(fbr, measurements[i].fbr);
      fur = join_horiz(fur, measurements[i].fur);
      
      fbx = join_horiz(fbx, measurements[i].fbx);
      fby = join_horiz(fby, measurements[i].fby);

      uv = join_horiz(uv, measurements[i].uv);
    }

    char path[150] = {0};

    sprintf(path, "%sMeasDoppler.dat", basePath);
    doppler.save(path, raw_ascii);
    sprintf(path, "%sMeasPos.dat", basePath);
    pos.save(path, raw_ascii);

    sprintf(path, "%sMeasFul.dat", basePath);
    ful.save(path, raw_ascii);
    sprintf(path, "%sMeasFbl.dat", basePath);
    fbl.save(path, raw_ascii);
    sprintf(path, "%sMeasFbr.dat", basePath);
    fbr.save(path, raw_ascii);
    sprintf(path, "%sMeasFur.dat", basePath);
    fur.save(path, raw_ascii);

    sprintf(path, "%sMeasBx.dat", basePath);
    fbx.save(path, raw_ascii);
    sprintf(path, "%sMeasBy.dat", basePath);
    fby.save(path, raw_ascii);

    sprintf(path, "%sMeasUv.dat", basePath);
    uv.save(path, raw_ascii);
  }

  mat loadTimes(const char *basePath)
  {
    char path[150] = {0};
    sprintf(path, "%sdts.dat", basePath);

    mat res; res.load(path);
    res = res.t();
    return res;
  }

  void saveTimes(const char *basePath, const arma::mat &ts)
  {
    char path[150] = {0};
    sprintf(path, "%sdts.dat", basePath);

    arma::mat temp = ts.t();
    temp.save(path, raw_ascii);
  }

  std::vector < NSCommand > loadCommands(const char *basePath)
  {
    using ::s32;
    std::vector < NSCommand > res;

    char path[150] = {0};

    sprintf(path, "%sV.dat", basePath);
    mat vs; vs.load(path);
    sprintf(path, "%sThetas.dat", basePath);
    mat dthetas; dthetas.load(path);

    for(s32 i=0; i<vs.n_cols; i++) {
      NSCommand u;
      u.dtheta = dthetas(0,i);
      u.v = vs(0,i);
      u.dutyCycle = 0;

      res.push_back(u);
    }
    return res;
  }

  void saveCommands(const char *basePath, const std::vector < NSCommand > &commands)
  {
    using ::s32;

    mat dthetas = zeros(1,commands.size()); mat vs = zeros(1, commands.size()); mat dutyCycles = zeros(1, commands.size());
    for(s32 i=0; i<commands.size(); i++) {
      dthetas(0,i) = commands[i].dtheta;
      vs(0,i) = commands[i].v;
      dutyCycles(0,i) = commands[i].dutyCycle;
    }

    char path[150] = {0};

    sprintf(path, "%sV.dat", basePath);
    vs.save(path, raw_ascii);
    sprintf(path, "%sThetas.dat", basePath);
    dthetas.save(path, raw_ascii);
    sprintf(path, "%sDutyCycles.dat", basePath);
    dutyCycles.save(path, raw_ascii);
  }

  PartMethod1 loadParticlesMethod1(const char *basePath)
  {
    using ::s32;

    char path[150] = {0};

    PartMethod1 res;

    sprintf(path, "%sPos.dat", basePath);
    res.pos.load(path);
    sprintf(path, "%sRho.dat", basePath);
    res.rhos.load(path);
    sprintf(path, "%sws.dat", basePath);
    res.ws.load(path);
    sprintf(path, "%sRs.dat", basePath);
    res.Rs = loadOrientations(path);
    return res;
  }

  void saveParticlesMethod1(const char *basePath, const PartMethod1 &particles)
  {
    using ::s32;

    char path[150] = {0};

    sprintf(path, "%sPos.dat", basePath);
    particles.pos.save(path, raw_ascii);
    sprintf(path, "%sRho.dat", basePath);
    particles.rhos.save(path, raw_ascii);
    sprintf(path, "%sws.dat", basePath);
    particles.ws.save(path, raw_ascii);
    sprintf(path, "%sRs.dat", basePath);
    saveOrientations(particles.Rs, path);
  }

  void saveTipHistory(const char *basePath, const std::vector < TipState > &hist)
  {
    mat pos = zeros(hist.size(), 3);
    std::vector < mat33 > Rs(hist.size());
    mat rhos = zeros(hist.size(), 1);
    for(s32 i=0; i<hist.size(); i++) {
      pos.row(i) = hist[i].pos.t();
      rhos(i,0) = hist[i].rho;
      Rs[i] = hist[i].R;
    }

    char path[200] = {0};
    sprintf(path, "%sPos.dat", basePath);
    pos.save(path, raw_ascii);
    sprintf(path, "%sRho.dat", basePath);
    rhos.save(path, raw_ascii);
    sprintf(path, "%sRs.dat", basePath);
    saveOrientations(Rs, path);
  }

  PartMethod3 loadParticlesMethod3(const char *basePath)
  {
    using ::s32;

    char path[150] = {0};

    PartMethod3 res;

    sprintf(path, "%sPos.dat", basePath);
    res.pos.load(path);
    sprintf(path, "%sRho.dat", basePath);
    res.rhos.load(path);
    sprintf(path, "%sws.dat", basePath);
    res.ws.load(path);
    sprintf(path, "%sRs.dat", basePath);
    std::vector < mat33 > Rs = loadOrientations(path);
    sprintf(path, "%ssigmas.dat", basePath);
    std::vector < mat33 > sigmas = loadOrientations(path);

    for(s32 i=0; i<Rs.size(); i++) {
      res.Rs.push_back(OrientationKF(Rs[i], sigmas[i]));
    }

    return res;
  }
  
  void saveParticlesMethod3(const char *basePath, const PartMethod3 &particles)
  {
    using ::s32;

    char path[150] = {0};

    sprintf(path, "%sPos.dat", basePath);
    particles.pos.save(path, raw_ascii);
    sprintf(path, "%sRho.dat", basePath);
    particles.rhos.save(path, raw_ascii);
    sprintf(path, "%sws.dat", basePath);
    particles.ws.save(path, raw_ascii);


    std::vector < mat33 > orientationMus;
    std::vector < mat33 > orientationSigmas;

    for(s32 i=0; i<particles.Rs.size(); i++) {
      orientationMus.push_back(particles.Rs[i].mu);
      orientationSigmas.push_back(particles.Rs[i].sigma);
    }
    sprintf(path, "%sRs.dat", basePath);
    saveOrientations(orientationMus, path);
    sprintf(path, "%ssigmas.dat", basePath);
    saveOrientations(orientationSigmas, path);
  }

  void saveOrientations(const std::vector < mat33 > &ors, const char *path)
  {
    using ::s32;
    mat res;    //res = [R1 R2 ... Rn] \in R^9xn, Ri = unrolled R \in R^9
    for(s32 i=0; i<ors.size(); i++) {
      mat curr = (mat)ors[i];
      res.insert_cols(i, reshape(curr, 9, 1));
    }
    res.save(path, raw_ascii);
  }

  std::vector < mat33 > loadOrientations(const char *path)
  {
    using ::s32;
    std::vector < mat33 > res;
    mat l;
    bool rv = l.load(path);
    assert(rv);
    for(s32 i=0; i<l.n_cols; i++) {
      res.push_back(reshape(l.col(i), 3, 3));
    }
    return res;
  }
}