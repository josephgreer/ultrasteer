#pragma once

#include <vector>
#include <deque>

//alglib
#include "SegmentCore.h"
#include <armadillo>

namespace Nf
{
  struct NSCommand
  {
    f64 dutyCycle;            // duty cycle fraction \in [0,1]
    f64 v;                    // velocity in mm/s
    f64 dtheta;               // rotation about needle axis \in [0, 2*pi]
  };

  struct Measurement
  {
    arma::mat points;         // represents positions of points x1,x2,...,xn \in R^3. [x1^T; x2^T;... xn^T] \in R^(nx3)
    arma::colvec doppler;     // represents corresponding doppler response values \in R^3 [d1; d2; ... dn] \in R^n
  };

  struct TipState
  {
    arma::colvec4 q;           // represents orientation
    arma::colvec3 pos;         // represents position
    f64 rho;                   // represents curvature
  };

  class ParticleFilterParameters
  {
  public:

  };

  class ParticleFilter
  {
  public:
    //Constructor
    //  nParticles: number of particles in particle filter
    ParticleFilter(s32 nParticles, const ParticleFilterParameters *p);

    //Propagate particle with time update
    virtual void Propagate(const NSCommand *u, f64 dt, const ParticleFilterParameters *p) = 0;

    //Apply measurement to particle filter from ultrasound
    virtual void ApplyMeasurement(const Measurement *m, const ParticleFilterParameters *p) = 0;

    //Initialize the particle filter based on state history
    //    hist: hist[0] = current state
    //          hist[1] = state 1 timestep back
    //          ...
    //          hist[n] = state n timesteps back
    virtual void InitializeParticleFilter(const std::vector < TipState > &hist, const ParticleFilterParameters *p) = 0;

    // Get all the particle positions
    virtual arma::mat GetParticlePositions(const ParticleFilterParameters *p) = 0;

    // Get all the particle positions
    //    returns:
    //      [q1^T ; ... ; qn^T] \in R^(nx4) each row represents an orientation in quaternions
    virtual arma::mat GetParticleOrientations(const ParticleFilterParameters *p) = 0;


    // Returns curvatures of each particle
    //   returns:
    //   [rho1; ...; rhon] \in R^n, each row represents a curvature value
    virtual arma::colvec GetParticleRhos(const ParticleFilterParameters *p) = 0;

    // Returns expected value of particle filter posterior distribution
    // in other words, the estimate of the filer.
    virtual TipState GetExpectedValue(const ParticleFilterParameters *p) = 0;
  };
}