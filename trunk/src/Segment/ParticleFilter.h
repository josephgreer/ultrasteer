#pragma once

#include <vector>
#include <deque>
#include "RTCore.h"
#include "UICore.h"

#define PF_MIN_RHO 10

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

  class PFParams
  {
  public:
    s32 minimumMeasurements;                  //  minimum number of measurements needed to measure particles
    arma::vec3 initPosMu;                     //  pos mu for initial distribution of particles
    arma::mat33 initPosSigma;                 //  pos sigma for initial distribution of particles
    f64 initRhoMu;                            //  rho mu for initial distribution of particles
    f64 initRhoSigma;                         //  rho sigma for initial distribution of particles
    arma::vec3 initOrientationMu;             //  orientation mu for initial distribution of particles
    arma::mat33 initOrientationSigma;         //  orientation sigma for initial distribution of particles
    arma::mat33 particleSigmaPos;             //  sigma pos for particle propagation (as opposed to pos sigma for simulation)
    arma::vec3 particleMuPos;                 //  mu pos for particle propagation (as opposed to pos mu for simulation)
    arma::mat33 particleSigmaOr;              //  sigma orientation for particle propagation
    arma::vec3 particleMuOr;                  //  mu orientation for particle propagation (as opposed to orientation mu for simulation)
    f64 particleSigmaVel;                     //  sigma velocity for particle propagation
    f64 particleMuVel;                        //  mu velocity for particle propagation (as opposed to vel mu for simulation)
    f64 particleSigmaRho;                     //  sigma rho for particle propagation
    f64 particleMuRho;                        //  mu rho for particle propagation
 
    PFParams();
  };

  class PFFullStateParams : public PFParams
  {
  public:
    PFFullStateParams();
  };

  class PFMarginalizedParams : public PFParams
  {
  public:
    arma::mat33 measurementSigma;             //  measurement sigma for orientation
    f64 distanceThreshSq;                     //  minimum distance squared between measurements and template for optimal rotation calculation
    s32 subsetSize;                           //  subset size for rotation measurement
    s32 procrustesIt;                         //  number of iterations to run procrustes

    PFMarginalizedParams();
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

    TipState & operator=(TipState o)
    {
      this->R = o.R;
      this->pos = o.pos;
      this->rho = o.rho;
      return *this;
    }

    //Propagate by a timestep according to unicycle model
    TipState Propagate(const NSCommand &u, f64 dt, const PFParams *p);

    //Propagate needle by path dl according to unicycle model
    TipState PropagateLength(const NSCommand &u, f64 dl, const PFParams *p);

    //Propagate needle tip back based on command history
    // u[0] = command from timestep t-1 to t
    // u[1] = command from timestep t-2 to t-1
    // u[n] = command from timestep n-1 to n
    // dts(0) = time elapsed from timestep t-1 to t
    // dts(1) = time elapsed from timestep t-2 to t-1
    // ...
    // dts(n) = time elapsed from timestep t-n-1 to t-n
    //return
    //   return[0] = current needle tip state
    //   return[1] = needle tip state 1 timestep back
    //     ...
    //   return[n] = needle tip state n timesteps back
    std::vector < TipState > PropagateBack(const std::vector < NSCommand > &u, const arma::vec &dts, const PFParams *p);
  };

  class ParticleFilter
  {
  protected:
    s32 m_nParticles;

  public:
    //Constructor
    //  nParticles: number of particles in particle filter
    ParticleFilter(s32 nParticles, const char *name, const PFParams *p);

    virtual ~ParticleFilter();

    //Initialize the particle filter based on state history
    //    hist: hist[0] = current state
    //          hist[1] = state 1 timestep back
    //          ...
    //          hist[n] = state n timesteps back
    virtual void InitializeParticleFilter(const std::vector < TipState > &hist, const PFParams *p) = 0;

    //Propagate particle with time update
    virtual void Propagate(const NSCommand *u, f64 dt, const PFParams *p) = 0;

    //Apply measurement to particle filter from ultrasound
    virtual void ApplyMeasurement(const Measurement *m, const PFParams *p) = 0;

    // Get all the particle positions
    virtual arma::mat GetParticlePositions(const PFParams *p) = 0;

    // Get all the particle positions
    //    returns:
    //      vector of tip frame orientations
    virtual std::vector < arma::mat33 > GetParticleOrientations(const PFParams *p) = 0;
    
    //Set the number of particles. Causes a resample
    virtual void SetNumberOfParticles(s32 nParticles);

    // Resample particles according to particle weight distribution;
    virtual void Resample(s32 nParticles) = 0;

    // Returns curvatures of each particle
    //   returns:
    //   [rho1; ...; rhon] \in R^n, each row represents a curvature value
    virtual arma::mat GetParticleRhos(const PFParams *p) = 0;

    // Returns expected value of particle filter posterior distribution
    // in other words, the estimate of the filer.
    virtual TipState GetExpectedValue(const PFParams *p) = 0;
  };

  // Implements full state particle filter.
  // Estimate joint distribution p(P_{t:1},R_{t:1}|Z_{t:1})
  class ParticleFilterFullState : public ParticleFilter 
  {
  protected:
    arma::mat m_pos;                        //position matrix for particles [x1 x2 ... xn] x_i \in R^3 m_pos \in R^(3xn)
    std::vector < arma::mat33 > m_R;        //vector of rotation matrices for particles
    arma::mat m_rho;                        //rhos [rho1 ... rhon] rhoi \in R, m_rhos \in R^n

  public:
    ParticleFilterFullState(s32 nParticles, const PFParams *p);

    virtual ~ParticleFilterFullState();

    //Initialize the particle filter based on state history
    //    hist: hist[0] = current state
    //          hist[1] = state 1 timestep back
    //          ...
    //          hist[n] = state n timesteps back
    virtual void InitializeParticleFilter(const std::vector < TipState > &hist, const PFParams *p);

    //Propagate particle with time update
    virtual void Propagate(const NSCommand *u, f64 dt, const PFParams *p);

    //Apply measurement to particle filter from ultrasound
    virtual void ApplyMeasurement(const Measurement *m, const PFParams *p);

    // Get all the particle positions
    // return
    //  [x_1^T; ... ; x_n^T] return \in R^(nx3)
    virtual arma::mat GetParticlePositions(const PFParams *p);

    // Get all the particle positions
    //    returns:
    //      vector of tip frame orientations
    virtual std::vector < arma::mat33 > GetParticleOrientations(const PFParams *p);

    // Resample particles according to particle weight distribution;
    virtual void Resample(s32 nParticles);


    // Returns curvatures of each particle
    //   returns:
    //   [rho1; ...; rhon] \in R^n, each row represents a curvature value
    virtual arma::mat GetParticleRhos(const PFParams *p);

    // Returns expected value of particle filter posterior distribution
    // in other words, the estimate of the filer.
    virtual TipState GetExpectedValue(const PFParams *p);
  };

  struct OrientationKF
  {
    public:
    arma::mat33 mu;                 //represents mean of gaussian distribution
    arma::mat33 sigma;              //covariance of gaussian distribution

   OrientationKF(arma::mat33 mu, arma::mat33 sigma)
   {
     this->mu = mu;
     this->sigma = sigma;
   }
  };

  // Implements full state particle filter.
  // Estimate joint distribution p(P_{t:1},R_{t:1}|Z_{t:1})
  class ParticleFilterMarginalized : public ParticleFilter 
  {    
  protected:
    arma::mat m_pos;                          //position matrix for particles [x1 x2 ... xn] x_i \in R^3 m_pos \in R^(3xn)
    std::vector < OrientationKF > m_R;        //vector of orientation Kalman Filters p(R_t|p_t,Y_{1:t})
    arma::mat m_rho;                          //rhos [rho1 ... rhon] rhoi \in R, m_rhos \in R^n


  public:
    ParticleFilterMarginalized(s32 nParticles, const PFParams *p);

    virtual ~ParticleFilterMarginalized();

    //Initialize the particle filter based on state history
    //    hist: hist[0] = current state
    //          hist[1] = state 1 timestep back
    //          ...
    //          hist[n] = state n timesteps back
    virtual void InitializeParticleFilter(const std::vector < TipState > &hist, const PFParams *p);

    //Propagate particle with time update
    virtual void Propagate(const NSCommand *u, f64 dt, const PFParams *p);

    //Apply measurement to particle filter from ultrasound
    virtual void ApplyMeasurement(const Measurement *m, const PFParams *p);

    // Get all the particle positions
    // return
    //  [x_1^T; ... ; x_n^T] return \in R^(nx3)
    virtual arma::mat GetParticlePositions(const PFParams *p);

    // Get all the particle orientations
    //    returns:
    //      vector of tip frame orientations
    virtual std::vector < arma::mat33 > GetParticleOrientations(const PFParams *p);
    
    // Get all the particle kalman filter orientations
    //    returns:
    //      vector of tip frame sigmas
    virtual std::vector < arma::mat33 > GetParticleOrientationSigmas(const PFParams *p);

    // Resample particles according to particle weight distribution;
    virtual void Resample(s32 nParticles);


    // Returns curvatures of each particle
    //   returns:
    //   [rho1; ...; rhon] \in R^n, each row represents a curvature value
    virtual arma::mat GetParticleRhos(const PFParams *p);

    // Returns expected value of particle filter posterior distribution
    // in other words, the estimate of the filer.
    virtual TipState GetExpectedValue(const PFParams *p);
  };
}