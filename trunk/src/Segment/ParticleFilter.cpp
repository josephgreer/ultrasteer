#pragma once

#include "ParticleFilter.h"

using namespace arma;
namespace Nf
{
  using ::s32;

  
  TipState TipState::PropagateLength(const NSCommand &u, f64 dl, const ParticleFilterParameters *p)
  {
    TipState res;

#if 0
 //k' coordinate system expressed in global coordinates.
// note rotation order is flipped because using intrinsic rotations with
// extrinisc rotation matrices
   vec3 zhat;
   zhat << 0 << endr << 0 << endr << 1 << endr;
   mat33 kp = this->R*SO3Exp(u.dtheta*zhat);

% location of tip expressed in Rk' coordinates
kp_x = [0; x.rho*(1-cos(dl/x.rho)); x.rho*sin(dl/x.rho)];


% new coordinate frame expressed in global coordinates.
% new coordinate frame is old coordinate frame rhoated by u.dtheta around
% needle tip's z-axis then rotated about the needle tip's x axis due to
% following the curved arc.  Note taht rotation order is flipped due to
% intrinsic axis rotations used rather than extrinsic.
x1.q = quatmult(kp,AxisAngleToQuat(-dl/x.rho*[1; 0; 0]));

% convert needle tip location into global coordinates
x1.pos = quatrot(kp,kp_x)+x.pos;
% pass through rho
x1.rho = x.rho;
#endif

    return res;
  }

  TipState TipState::Propagate(const NSCommand &u, f64 dt, const ParticleFilterParameters *p)
  {
    TipState res;



    return res;
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