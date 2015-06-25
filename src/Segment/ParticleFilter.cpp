#pragma once

#include "ParticleFilter.h"

using namespace arma;
namespace Nf
{
  using ::s32;


  TipState TipState::PropagateLength(const NSCommand &u, f64 dl, const ParticleFilterParameters *p)
  {
    TipState res;

    //k' coordinate system expressed in global coordinates.
    // note rotation order is flipped because using intrinsic rotations with
    // extrinisc rotation matrices
    vec3 zhat;
    zhat << 0 << endr << 0 << endr << 1 << endr;
    vec3 xhat;
    xhat << 1 << endr << 0 << endr << 0 << endr;

    mat33 kp = this->R*SO3Exp((vec3 &)(u.dtheta*zhat));

    // location of tip expressed in Rk' coordinates
    f64 phi = dl/this->rho;
    vec3 kp_x; 
    kp_x << 0 << endr << this->rho*(1-cos(phi)) << endr << this->rho*sin(phi) << endr;


    // new coordinate frame expressed in global coordinates.
    // new coordinate frame is old coordinate frame rhoated by u.dtheta around
    // needle tip's z-axis then rotated about the needle tip's x axis due to
    // following the curved arc.  Note taht rotation order is flipped due to
    //  intrinsic axis rotations used rather than extrinsic.
    res.R = (mat33 &)(kp*SO3Exp((vec3 &)(-phi*xhat)));

    // convert needle tip location into global coordinates
    res.pos = kp*kp_x+this->pos;
    // pass through rho
    res.rho = this->rho;

    return res;
  }

  TipState TipState::Propagate(const NSCommand &u, f64 dt, const ParticleFilterParameters *p)
  {
    return PropagateLength(u, u.v*dt, p);
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  /// Begin Basic Particle Filter
  //////////////////////////////////////////////////////////////////////////////////////////
  ParticleFilter::ParticleFilter(s32 nParticles, const char *name, const ParticleFilterParameters *p)
    : m_nParticles(nParticles)
    , ParameterCollection(name)
  {
  }

  ParticleFilter::~ParticleFilter()
  {
  }
  //////////////////////////////////////////////////////////////////////////////////////////
  /// End Basic Particle Filter
  //////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////////////////
  /// Begin Particle Filter Full State
  //////////////////////////////////////////////////////////////////////////////////////////
  ParticleFilterFullState::ParticleFilterFullState(s32 nParticles, const ParticleFilterParameters *p)
    : ParticleFilter(nParticles, "ParticleFilterFullState", p)
  {
  }

  ParticleFilterFullState::~ParticleFilterFullState()
  {
  }

  void ParticleFilterFullState::InitializeParticleFilter(const std::vector < TipState > &hist, const ParticleFilterParameters *p)
  {
  }

  void ParticleFilterFullState::Propagate(const NSCommand *u, f64 dt, const ParticleFilterParameters *p)
  {
  }

  void ParticleFilterFullState::ApplyMeasurement(const Measurement *m, const ParticleFilterParameters *p)
  {
  }

  mat ParticleFilterFullState::GetParticlePositions(const ParticleFilterParameters *p)
  {
    return zeros<mat>(m_nParticles, 3);
  }

  std::vector < mat33 > ParticleFilterFullState::GetParticleOrientations(const ParticleFilterParameters *p)
  {
    return std::vector < mat33 >();
  }

  vec ParticleFilterFullState::GetParticleRhos(const ParticleFilterParameters *p)
  {
    return zeros<vec>(m_nParticles);
  }

  TipState ParticleFilterFullState::GetExpectedValue(const ParticleFilterParameters *p)
  {
    return TipState();
  }
  //////////////////////////////////////////////////////////////////////////////////////////
  /// End Particle Filter Full State
  //////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////////////////
  /// Begin Particle Filter With Kalman Filter for Orientation
  //////////////////////////////////////////////////////////////////////////////////////////
  ParticleFilterMarginalized::ParticleFilterMarginalized(s32 nParticles, const ParticleFilterParameters *p)
    : ParticleFilter(nParticles, "MarginalizedParticleFilter", p)
  {
  }

  ParticleFilterMarginalized::~ParticleFilterMarginalized()
  {
  }

  void ParticleFilterMarginalized::InitializeParticleFilter(const std::vector < TipState > &hist, const ParticleFilterParameters *p)
  {
  }

  void ParticleFilterMarginalized::Propagate(const NSCommand *u, f64 dt, const ParticleFilterParameters *p)
  {
  }

  void ParticleFilterMarginalized::ApplyMeasurement(const Measurement *m, const ParticleFilterParameters *p)
  {
  }

  mat ParticleFilterMarginalized::GetParticlePositions(const ParticleFilterParameters *p)
  {
    return zeros<mat>(m_nParticles, 3);
  }

  std::vector < mat33 > ParticleFilterMarginalized::GetParticleOrientations(const ParticleFilterParameters *p)
  {
    return std::vector < mat33 >();
  }

  vec ParticleFilterMarginalized::GetParticleRhos(const ParticleFilterParameters *p)
  {
    return zeros<vec>(m_nParticles);
  }

  TipState ParticleFilterMarginalized::GetExpectedValue(const ParticleFilterParameters *p)
  {
    return TipState();
  }
  //////////////////////////////////////////////////////////////////////////////////////////
  /// End Particle Filter With Kalman Filter for Orientation
  //////////////////////////////////////////////////////////////////////////////////////////
}