#pragma once

#include "ParticleFilter.h"

using namespace arma;
namespace Nf
{
  using ::s32;

  ParticleFilter::ParticleFilter(s32 nParticles, const ParticleFilterParameters *p)
    : m_nParticles(nParticles)
    , ParameterCollection("Particle Fiter")
  {
  }

  ParticleFilter::~ParticleFilter()
  {
  }

  ParticleFilterFullState::ParticleFilterFullState(s32 nParticles, const ParticleFilterParameters *p)
    : ParticleFilter(nParticles, p)
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
}