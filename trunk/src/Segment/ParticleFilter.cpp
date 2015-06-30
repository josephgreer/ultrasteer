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

    minimumMeasurements = 1;
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
  /// Begin Basic Particle Filter
  //////////////////////////////////////////////////////////////////////////////////////////
  ParticleFilter::ParticleFilter(s32 nParticles, const char *name, const PFParams *p)
    : m_nParticles(nParticles)
  {
  }

  ParticleFilter::~ParticleFilter()
  {
  }

  void ParticleFilter::SetNumberOfParticles(s32 nParticles)
  {
    Resample(nParticles);
    m_nParticles = nParticles;
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

  void ParticleFilterFullState::ApplyMeasurement(const Measurement *m, const PFParams *p)
  {

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

  void ParticleFilterMarginalized::ApplyMeasurement(const Measurement *m, const PFParams *p)
  {

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
  //////////////////////////////////////////////////////////////////////////////////////////
  /// End Particle Filter With Kalman Filter for Orientation
  //////////////////////////////////////////////////////////////////////////////////////////
}