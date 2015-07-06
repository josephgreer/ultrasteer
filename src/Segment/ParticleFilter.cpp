#pragma once

#include "ParticleFilter.h"

using namespace arma;
namespace Nf
{
  using ::s32;

  PFParams::PFParams()
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
    particleSigmaPos = (mat33)diagmat(dd/3.0);

    particleMuOr = zeros<vec>(3);
    dd.clear(); dd << PI << PI << PI << endr;
    particleSigmaOr = (mat33)diagmat(1.0/5000.0*dd);

    particleSigmaVel = 5;
    particleMuVel = 0;

    particleSigmaRho = 3;
    particleMuRho = 0;

    mpp = 83;
    measurementOffsetSigma = (vec2)(mpp*5*1e-3*ones(2,1));

    offNeedleDopplerMu = 0.56;                                           
    offNeedleDopplerSigma = 0.75;                                        
    onNeedleDopplerMu = 2.33;                                            
    onNeedleDopplerSigma = 0.098;  

    minimumMeasurements = 1;

    usw = mpp*630*1e-3;
    ush = mpp*480*1e-3;

    sigA = 1;
    sigC = 6;

    n = 50;

  }

  PFFullStateParams::PFFullStateParams()
    : PFParams()
  {
  }

  PFMarginalizedParams::PFMarginalizedParams()
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
      uc = u[i-1];
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

  static f64 sigmoid(f64 x, f64 a, f64 c)
  {
    return 1/(1+exp(-a*(x-c)));
  }

  //X = [x1 x2 ... xn] x_i \in R^3
  //Y = [y1 y2 ... ym] y_i \in R^3
  //D = D_ij = |(x_i-y_j)|^2
  static mat distanceMatrix(const mat &X, const mat &Y)
  {
    using ::s32;

    s32 m = X.n_cols;
    s32 n = Y.n_cols;

    mat D = repmat(((mat)(X.t()*X)).diag(), 1, n)-2*X.t()*Y+repmat(((mat)(Y.t()*Y)).diag().t(), m, 1);

    return D;
  }

  // sample without replacmenet
  static uvec sample(uvec x, s32 n)
  {
    using ::s32;

    uvec seq(x.n_rows, 1);
    for(s32 i=0; i<x.n_rows; i++) {
      seq(i) = i;
    }
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
    
    uvec minTemplate, goodDs, shuf;
    mat D,U,V,X,Y,dR;
    vec S, minD;
    for(s32 i=0; i<pfm->procrustesIt; i++) {
      //D_ij = distanceSq(measurements(i), cTemplate(j))
      D = distanceMatrix(measurements,cTemplate);

      minD = min(D, 1);
      for(s32 r=0; r<D.n_rows; r++) {
        minTemplate = join_vert(minTemplate,find(D.row(r) == minD(r)));
      }
      goodDs = find(minD < pfm->distanceThreshSq);
      goodDs = sample(goodDs, MIN(pfm->subsetSize, goodDs.n_rows));

      minTemplate = minTemplate.rows(goodDs);

      X = cTemplate.cols(minTemplate);
      Y = measurements.cols(goodDs);

      svd(U,S,V,X*Y.t());
      dR = V*U.t();
      if(det(dR) < 0) {
        vec rotZ; rotZ << 1 << 1 << -1 << endr;
        dR = V*diagmat(rotZ)*U.t();
      }
      R = (mat33)(dR*R);
      cTemplate = dR*cTemplate;
    }
    return R;
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  /// End Helper Functions
  //////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////////////////
  /// Begin Basic Particle Filter
  //////////////////////////////////////////////////////////////////////////////////////////
  ParticleFilter::ParticleFilter(s32 nParticles, const char *name, const PFParams *p)
    : m_nParticles(nParticles)
  {
    m_w = ones(m_nParticles)/m_nParticles;
  }

  ParticleFilter::~ParticleFilter()
  {
  }

  void ParticleFilter::SetNumberOfParticles(s32 nParticles)
  {
    Resample(nParticles);
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
  ParticleFilterFullState::ParticleFilterFullState(s32 nParticles, const PFParams *p)
    : ParticleFilter(nParticles, "ParticleFilterFullState", p)
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
    vec3 tt;
    vec2 suv, duv, a, b;
    vec3 dr;
    f64 proj, p_uvx, p_dx, pin, dop;

    vec usFrameParams;
    usFrameParams << 1 << endr << params->usw << endr << params->ush << endr;

    mat pw = zeros(1,m_nParticles);

    bool offFrame;
    // For each particle...
    for(s32 i=0; i<m_nParticles; i++) {
      //delta rotation from particle 0 to particle i
      Rdelta = (mat33)(m_R[i]*m_R[0].t());

      //points from measurement to particle loc
      dr = m[0].pos.col(0)-m_pos.col(i);

      // look at projection of dr onto particle tip frame.  if it's positive
      // then we believe it's in front of this particle (in other words, the
      // ultrasound frame is off the needle in this hypothetical position)
      proj = ((mat)(dr.t()*m_R[i].col(2)))(0,0);

      p_uvx = p_dx = 0;

      //does the needle flagella of the particle intersect the frame?
      offFrame = true;

      dop = m[0].doppler(0,0);

      if(proj <= 0) {
        // ultrasound frame behind the needle tip
        //subtract off first point so we rotate about it
        cModelHist = modelHist-repmat(modelHist.col(0),1,modelHist.n_cols);
        // rotate history by Rdelta
        cModelHist = Rdelta*cModelHist;
        // offset so that first point is now particle i point
        cModelHist = cModelHist+repmat(m_pos.col(i),1,cModelHist.n_cols);

        // for each history point...
        for(s32 j=0; j<cModelHist.n_cols-1; j++) {
          dr = cModelHist.col(j+1)-cModelHist.col(j);

          // find the point of intersection between ultrasound frame and "flagella"
          frameMat = (mat33)(join_horiz(join_horiz(-dr, m[0].fbx), m[0].fby));

          tt = solve(frameMat,cModelHist.col(j)-m[0].ful);
          if(sum(zeros(3,1) <= tt)==3 && sum(tt <= usFrameParams)==3) {
            // it intersects so look up location in truncated 
            // gaussian centered at measurement intersections
            suv = tt.submat(span(1,2), span(0,0));

            // measurement.uv = measurement in image coordinaates in (mm)
            // p((u,v)|d,x) ~ truncated gaussian centered at shaft particle interesection
            // calculate limits for truncated gaussian
            a = suv-usFrameParams.submat(span(1,2),span(0,0));  // if shaft (u,v) - (u,v) < shaft (u,v) - br, then  (u,v) > br
            b = suv;                                            // if shaft (u,v) - (u,v) > shaft (u,v), then (u,v) < (0,0) 

            duv = suv-m[0].uv.col(0);

            // calculate p(frame interesects with flagella|doppler)
            pin = sigmoid(m[0].doppler(0,0), params->sigA, params->sigC);
            p_uvx = pin*TruncatedIndependentGaussianPDF2(duv, (vec2)zeros(2,1), params->measurementOffsetSigma, a, b)+
              (1-pin)*(1/(params->ush*params->usw));

            //TODO change this to sigmoid
            p_dx = lognpdf(dop, params->onNeedleDopplerMu, params->onNeedleDopplerSigma);
            offFrame = 0;
            break;
          }
        }
      }

      // particle doesn't intersect image frame
      if(offFrame) {
        p_uvx = 1/(params->ush*params->usw);
        p_dx = lognpdf(dop, params->offNeedleDopplerMu, params->offNeedleDopplerSigma);
      }

      pw(0,i) = p_uvx*p_dx;
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
    return TipState();
  }

  void ParticleFilterFullState::Resample(s32 nParticles)
  {
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
  ParticleFilterMarginalized::ParticleFilterMarginalized(s32 nParticles, const PFParams *p)
    : ParticleFilter(nParticles, "MarginalizedParticleFilter", p)
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
    
    // backward projected points from particle 0.
    // this will be adjusted for each particle
    mat modelHist = tipHistoryToPointMatrix(ts);
    mat cModelHist = zeros(modelHist.n_rows, modelHist.n_cols);
    
    mat33 Rdelta, frameMat, Rmeas, measSigma,K,sigmaC,Rc,Rprior;

    vec3 tt, errorVec,v;
    vec2 suv, duv, a, b;
    vec3 dr;
    f64 proj, p_uvx, p_dx, pin, dop;

    vec usFrameParams;
    usFrameParams << 1 << endr << params->usw << endr << params->ush << endr;

    mat pw = zeros(1,m_nParticles);

    bool offFrame;
    // For each particle...
    for(s32 i=0; i<m_nParticles; i++) {
      Rprior = m_R[i].mu;
      //delta rotation from particle 0 to particle i
      Rdelta = (mat33)(m_R[i].mu*m_R[0].mu.t());

      // ultrasound frame behind the needle tip
      //subtract off first point so we rotate about it
      cModelHist = modelHist-repmat(modelHist.col(0),1,modelHist.n_cols);
      // rotate history by Rdelta
      cModelHist = Rdelta*cModelHist;

      assert(m.size() >= params->minimumMeasurements);

      Rmeas = optimalRotationForModel(cModelHist, measurementToPointMat(m), m_pos.col(i), p);
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

      //points from measurement to particle loc
      dr = m[0].pos.col(0)-m_pos.col(i);

      // look at projection of dr onto particle tip frame.  if it's positive
      // then we believe it's in front of this particle (in other words, the
      // ultrasound frame is off the needle in this hypothetical position)
      proj = ((mat)(dr.t()*m_R[i].mu.col(2)))(0,0);

      p_uvx = p_dx = 0;

      //does the needle flagella of the particle intersect the frame?
      offFrame = true;

      dop = m[0].doppler(0,0);

      if(proj <= 0) {
        // offset so that first point is now particle i point
        cModelHist = cModelHist+repmat(m_pos.col(i),1,cModelHist.n_cols);

        // for each history point...
        for(s32 j=0; j<cModelHist.n_cols-1; j++) {
          dr = cModelHist.col(j+1)-cModelHist.col(j);

          // find the point of intersection between ultrasound frame and "flagella"
          frameMat = (mat33)(join_horiz(join_horiz(-dr, m[0].fbx), m[0].fby));

          tt = solve(frameMat,cModelHist.col(j)-m[0].ful);
          if(sum(zeros(3,1) <= tt)==3 && sum(tt <= usFrameParams)==3) {
            // it intersects so look up location in truncated 
            // gaussian centered at measurement intersections
            suv = tt.submat(span(1,2), span(0,0));

            // measurement.uv = measurement in image coordinaates in (mm)
            // p((u,v)|d,x) ~ truncated gaussian centered at shaft particle interesection
            // calculate limits for truncated gaussian
            a = suv-usFrameParams.submat(span(1,2),span(0,0));  // if shaft (u,v) - (u,v) < shaft (u,v) - br, then  (u,v) > br
            b = suv;                                            // if shaft (u,v) - (u,v) > shaft (u,v), then (u,v) < (0,0) 

            duv = suv-m[0].uv.col(0);

            // calculate p(frame interesects with flagella|doppler)
            pin = sigmoid(m[0].doppler(0,0), params->sigA, params->sigC);
            p_uvx = pin*TruncatedIndependentGaussianPDF2(duv, (vec2)zeros(2,1), params->measurementOffsetSigma, a, b)+
              (1-pin)*(1/(params->ush*params->usw));

            //TODO change this to sigmoid
            p_dx = lognpdf(dop, params->onNeedleDopplerMu, params->onNeedleDopplerSigma);
            offFrame = 0;
            break;
          }
        }
      }

      // particle doesn't intersect image frame
      if(offFrame) {
        p_uvx = 1/(params->ush*params->usw);
        p_dx = lognpdf(dop, params->offNeedleDopplerMu, params->offNeedleDopplerSigma);
      }

      pw(0,i) = p_uvx*p_dx;
    }

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
    return TipState();
  }

  void ParticleFilterMarginalized::Resample(s32 nParticles)
  {
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
}