#pragma once

#include "ParticleFilter.h"

using namespace arma;
namespace Nf
{
  using ::s32;


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

  mat ParticleFilterFullState::GetParticleOrientations(const ParticleFilterParameters *p)
  {
    return zeros<mat>(m_nParticles, 4);
  }

  colvec ParticleFilterFullState::GetParticleRhos(const ParticleFilterParameters *p)
  {
    return zeros<colvec>(m_nParticles);
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

  mat ParticleFilterMarginalized::GetParticleOrientations(const ParticleFilterParameters *p)
  {
    return zeros<mat>(m_nParticles, 4);
  }

  colvec ParticleFilterMarginalized::GetParticleRhos(const ParticleFilterParameters *p)
  {
    return zeros<colvec>(m_nParticles);
  }

  TipState ParticleFilterMarginalized::GetExpectedValue(const ParticleFilterParameters *p)
  {
    return TipState();
  }
  //////////////////////////////////////////////////////////////////////////////////////////
  /// End Particle Filter With Kalman Filter for Orientation
  //////////////////////////////////////////////////////////////////////////////////////////
}