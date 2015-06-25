#pragma once

#include <vector>
#include <deque>
#include "RTCore.h"
#include "UICore.h"

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
    arma::vec doppler;        // represents corresponding doppler response values \in R^3 [d1; d2; ... dn] \in R^n
  };

  class ParticleFilterParameters
  {
  public:

  };

  struct TipState
  {
    arma::mat33 R;             // R = [x y z] x,y,z \in R^3, unit vectors representing tip frame
    arma::vec3 pos;            // represents position
    f64 rho;                   // represents curvature

    TipState()
    {
      R.eye();
      pos.zeros();
      rho = 0;
    }

    //Propagate by a timestep according to unicycle model
    TipState Propagate(const NSCommand &u, f64 dt, const ParticleFilterParameters *p);

    //Propagate needle by path dl according to unicycle model
    TipState PropagateLength(const NSCommand &u, f64 dl, const ParticleFilterParameters *p);
  };

  class ParticleFilter : public ParameterCollection
  {
  protected:
    s32 m_nParticles;

  public:
    //Constructor
    //  nParticles: number of particles in particle filter
    ParticleFilter(s32 nParticles, const char *name, const ParticleFilterParameters *p);

    virtual ~ParticleFilter();

    //Initialize the particle filter based on state history
    //    hist: hist[0] = current state
    //          hist[1] = state 1 timestep back
    //          ...
    //          hist[n] = state n timesteps back
    virtual void InitializeParticleFilter(const std::vector < TipState > &hist, const ParticleFilterParameters *p) = 0;

    //Propagate particle with time update
    virtual void Propagate(const NSCommand *u, f64 dt, const ParticleFilterParameters *p) = 0;

    //Apply measurement to particle filter from ultrasound
    virtual void ApplyMeasurement(const Measurement *m, const ParticleFilterParameters *p) = 0;

    // Get all the particle positions
    virtual arma::mat GetParticlePositions(const ParticleFilterParameters *p) = 0;

    // Get all the particle positions
    //    returns:
    //      vector of tip frame orientations
    virtual std::vector < arma::mat33 > GetParticleOrientations(const ParticleFilterParameters *p) = 0;


    // Returns curvatures of each particle
    //   returns:
    //   [rho1; ...; rhon] \in R^n, each row represents a curvature value
    virtual arma::vec GetParticleRhos(const ParticleFilterParameters *p) = 0;

    // Returns expected value of particle filter posterior distribution
    // in other words, the estimate of the filer.
    virtual TipState GetExpectedValue(const ParticleFilterParameters *p) = 0;
  };

  // Implements full state particle filter.
  // Estimate joint distribution p(P_{t:1},R_{t:1}|Z_{t:1})
  class ParticleFilterFullState : public ParticleFilter 
  {

    ParticleFilterFullState(s32 nParticles, const ParticleFilterParameters *p);

    virtual ~ParticleFilterFullState();

    //Initialize the particle filter based on state history
    //    hist: hist[0] = current state
    //          hist[1] = state 1 timestep back
    //          ...
    //          hist[n] = state n timesteps back
    virtual void InitializeParticleFilter(const std::vector < TipState > &hist, const ParticleFilterParameters *p);

    //Propagate particle with time update
    virtual void Propagate(const NSCommand *u, f64 dt, const ParticleFilterParameters *p);

    //Apply measurement to particle filter from ultrasound
    virtual void ApplyMeasurement(const Measurement *m, const ParticleFilterParameters *p);

    // Get all the particle positions
    // return
    //  [x_1^T; ... ; x_n^T] return \in R^(nx3)
    virtual arma::mat GetParticlePositions(const ParticleFilterParameters *p);

    // Get all the particle positions
    //    returns:
    //      vector of tip frame orientations
    virtual std::vector < arma::mat33 > GetParticleOrientations(const ParticleFilterParameters *p) = 0;


    // Returns curvatures of each particle
    //   returns:
    //   [rho1; ...; rhon] \in R^n, each row represents a curvature value
    virtual arma::vec GetParticleRhos(const ParticleFilterParameters *p);

    // Returns expected value of particle filter posterior distribution
    // in other words, the estimate of the filer.
    virtual TipState GetExpectedValue(const ParticleFilterParameters *p);
  };

  // Implements full state particle filter.
  // Estimate joint distribution p(P_{t:1},R_{t:1}|Z_{t:1})
  class ParticleFilterMarginalized : public ParticleFilter 
  {

    ParticleFilterMarginalized(s32 nParticles, const ParticleFilterParameters *p);

    virtual ~ParticleFilterMarginalized();

    //Initialize the particle filter based on state history
    //    hist: hist[0] = current state
    //          hist[1] = state 1 timestep back
    //          ...
    //          hist[n] = state n timesteps back
    virtual void InitializeParticleFilter(const std::vector < TipState > &hist, const ParticleFilterParameters *p);

    //Propagate particle with time update
    virtual void Propagate(const NSCommand *u, f64 dt, const ParticleFilterParameters *p);

    //Apply measurement to particle filter from ultrasound
    virtual void ApplyMeasurement(const Measurement *m, const ParticleFilterParameters *p);

    // Get all the particle positions
    // return
    //  [x_1^T; ... ; x_n^T] return \in R^(nx3)
    virtual arma::mat GetParticlePositions(const ParticleFilterParameters *p);

    // Get all the particle positions
    //    returns:
    //      vector of tip frame orientations
    virtual std::vector < arma::mat33 > GetParticleOrientations(const ParticleFilterParameters *p) = 0;


    // Returns curvatures of each particle
    //   returns:
    //   [rho1; ...; rhon] \in R^n, each row represents a curvature value
    virtual arma::vec GetParticleRhos(const ParticleFilterParameters *p);

    // Returns expected value of particle filter posterior distribution
    // in other words, the estimate of the filer.
    virtual TipState GetExpectedValue(const ParticleFilterParameters *p);
  };
}