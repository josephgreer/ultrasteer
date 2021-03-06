#pragma once

#include <vector>
#include <deque>
#include "RTCore.h"
#include "UICore.h"
#include "RPFileReader.h"

#define PF_MIN_RHO 10

namespace Nf
{
  struct Measurement
  {
    arma::mat pos;             // represents positions of points x1,x2,...,xn \in R^3. [x1 x2 ... xn] \in R^(3xn)
    arma::mat uv;              // represents pixel coordinates of measurement x1,x2,..,xn \in R^2. [x1 x2 ... xn] \in R^(2xn)
    arma::mat doppler;         // represents corresponding doppler response values \in R [d1 d2 ... dn] \in R^n
    arma::vec ful;             // represents upper left point of frame ful \in R^3
    arma::vec fbl;             // represents bottom left point of frame fbl \in R^3
    arma::vec fbr;             // represents bottom right point of frame fbr \in R^3
    arma::vec fur;             // represents upper right point of frame fur \in R^3
    arma::vec fbx;             // represents x-basis vector of frame \in R^3
    arma::vec fby;             // represents y-basis vector of frame\in R^3
  };

  class PFParams : public ParameterCollection
  {
  public:
    std::tr1::shared_ptr < Vec3dParameter > tipOffset;                          //  offset of physical tip from "logical" tip (in calibrated tip frame) tracked by particle filter
    std::tr1::shared_ptr < IntParameter > minimumMeasurements;                  //  minimum number of measurements needed to measure particles
    std::tr1::shared_ptr < FloatParameter > minLength;                          //  Minimum length for tip history to be used in measurement model
    std::tr1::shared_ptr < Vec3dParameter > initPosMu;                          //  pos mu for initial distribution of particles
    std::tr1::shared_ptr < Vec3dParameter > initPosSigma;                       //  pos sigma for initial distribution of particles
    std::tr1::shared_ptr < FloatParameter > initRhoMu;                          //  rho mu for initial distribution of particles
    std::tr1::shared_ptr < FloatParameter > initRhoSigma;                       //  rho sigma for initial distribution of particles
    std::tr1::shared_ptr < Vec3dParameter> initOrientationMu;                   //  orientation mu for initial distribution of particles
    std::tr1::shared_ptr < Vec3dParameter > initOrientationSigma;               //  orientation sigma for initial distribution of particles
    std::tr1::shared_ptr < Vec3dParameter > particleSigmaPos;                   //  sigma pos for particle propagation (as opposed to pos sigma for simulation)
    std::tr1::shared_ptr < Vec3dParameter > particleMuPos;                      //  mu pos for particle propagation (as opposed to pos mu for simulation)
    std::tr1::shared_ptr < Vec3dParameter > particleSigmaOr;                    //  sigma orientation for particle propagation
    std::tr1::shared_ptr < Vec3dParameter > particleMuOr;                       //  mu orientation for particle propagation (as opposed to orientation mu for simulation)
    Vec2d mpp;                                                                  //  microns per pixel
    Vec2d measurementOffsetSigma;                                               //  measurement noise of needle section within US frame (in mm)
    std::tr1::shared_ptr < Vec2dParameter > measurementOffsetSigmaPx;           //  measurement offset sigma within US frame (pixels) converted automatically into mm
    std::tr1::shared_ptr < FloatParameter > offNeedleDopplerMu;                 //  Doppler strength of measurement off needle distributed according to lognormal dist
    std::tr1::shared_ptr < FloatParameter > offNeedleDopplerSigma;              //  Doppler strength of measurement off needle distributed according to lognormal dist
    std::tr1::shared_ptr < FloatParameter > onNeedleDopplerMu;                  //  Doppler strength of measurement on needle distributed according to lognormal dist
    std::tr1::shared_ptr < FloatParameter > onNeedleDopplerSigma;               //  Doppler strength of measurement on needle distributed according to lognormal dist
    std::tr1::shared_ptr < FloatParameter > particleSigmaVel;                   //  sigma velocity for particle propagation
    std::tr1::shared_ptr < FloatParameter > particleMuVel;                      //  mu velocity for particle propagation (as opposed to vel mu for simulation)
    std::tr1::shared_ptr < FloatParameter > particleSigmaRho;                   //  sigma rho for particle propagation
    std::tr1::shared_ptr < FloatParameter > particleMuRho;                      //  mu rho for particle propagation
    f64 usw;                                                                    //  width of ultrasound frame in mm
    f64 ush;                                                                    //  height of ultraosund frame in mm
    std::tr1::shared_ptr < FloatParameter > sigB0;                              //  for outlier smooth step sigmoid 1/(1+exp(-B0 - B1*x)
    std::tr1::shared_ptr < FloatParameter > sigB1;                              //  for outlier smooth step sigmoid 1/(1+exp(-B0 - B1*x)
    std::tr1::shared_ptr < FloatParameter > lambdaDop;                          //  Regularization for p(D | x)
    std::tr1::shared_ptr < FloatParameter > offFrameB0;                         //  p(off frame | distance projection) 1/(1+exp(-B0-B1*x))
    std::tr1::shared_ptr < FloatParameter > offFrameB1;                         //  p(off frame | distance projection) 1/(1+exp(-B0-B1*x))
    std::tr1::shared_ptr < IntParameter > n;                                    //  how many points do we use for creating needle "flagella"
    std::tr1::shared_ptr < FloatParameter > neff;                               //  resample if the number of effective particles drops below this fraction
    std::tr1::shared_ptr < BoolParameter > useLut;                              //  use LUT for p(D | x) or sigmoid
    std::tr1::shared_ptr < FileParameter > onNeedleDopplerLUTPath;              //  Path for on needle doppler distribution look up table generated by kernel density estimation
    std::tr1::shared_ptr < FileParameter > offNeedleDopplerLUTPath;             //  Path for off needle doppler distribution look up table generated by kernel density estimation
 
    PFParams(Vec2d mpp = Vec2d(83,83), const char *name = "Particle Filter Params");
  };

  class Distribution
  {
  public:
    virtual f64 P(f64 x) const = 0;
  };

  // Implements distribution from lookup table specified by path
  class LUTDist : public Distribution
  {
  protected:
    arma::vec m_p;
    arma::vec m_x;
    f64 max_x;
    f64 min_x;
    bool m_init;

  public:
    LUTDist(const char *path);
    virtual f64 P(f64 x) const;
  };

  class LogNormalDist : public Distribution
  {
  protected:
    f64 m_mu;
    f64 m_sigma;

  public:
    LogNormalDist(f64 mu, f64 sigma) 
      : m_mu(mu)
      , m_sigma(sigma)
    {
    }

    virtual f64 P(f64 x) const
    {
      return lognpdf(x, m_mu, m_sigma);
    }
  };

  class PFFullStateParams : public PFParams
  {
  public:
    PFFullStateParams(Vec2d mpp = Vec2d(83,83), const char *name = "Particle Filter Full State Params");
  };

  class PFMarginalizedParams : public PFParams
  {
  public:
    std::tr1::shared_ptr < Vec3dParameter > measurementSigma;                      //  measurement sigma for orientation
    std::tr1::shared_ptr < FloatParameter > distanceThreshSq;                      //  minimum distance squared between measurements and template for optimal rotation calculation
    std::tr1::shared_ptr < IntParameter > subsetSize;                              //  subset size for rotation measurement
    std::tr1::shared_ptr < IntParameter > procrustesIt;                            //  number of iterations to run procrustes
    std::tr1::shared_ptr < FloatParameter > minTotalLength;                        //  minimum shaft length before starting to compute rotations
    std::tr1::shared_ptr < IntParameter > nPoints;                                 //  number of points to average together for distance calculation

    PFMarginalizedParams(Vec2d mpp = Vec2d(83,83), const char *name = "Particle Filter Marginalized Params");
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
    arma::mat m_w;                          //weights [w1 ... wn] wi \in R, m_w \in R^(1xn)
    std::tr1::shared_ptr < Distribution > m_pDopOverNeedle;
    std::tr1::shared_ptr < Distribution > m_pDopNotOverNeedle;
    Vec2d m_overNeedle;                       //does US frame intersect each particle's flagella?

  public:
    //Constructor
    //  nParticles: number of particles in particle filter
    ParticleFilter(s32 nParticles, const char *name, 
      std::tr1::shared_ptr < Distribution > pDopOverNeedle, std::tr1::shared_ptr < Distribution > pDopNotOverNeedle, const PFParams *p);

    virtual ~ParticleFilter();

    virtual s32 GetNumberOfParticles() { return m_nParticles; }

    //Initialize the particle filter based on state history
    //    hist: hist[0] = current state
    //          hist[1] = state 1 timestep back
    //          ...
    //          hist[n] = state n timesteps back
    virtual void InitializeParticleFilter(const std::vector < TipState > &hist, const PFParams *p) = 0;

    //Propagate particle with time update
    virtual void Propagate(const NSCommand *u, f64 dt, const PFParams *p) = 0;
    
    //Used for testing only
    virtual void SetWeights(const arma::mat &w);

    virtual arma::mat GetWeights();

    //Apply measurement to particle filter from ultrasound
    // m[0] = current measurement
    // m[1] = measurement 1 timestep back
    // ...
    // m[n] = measurement n timesteps back
    // u[0] = command from t-1 to t
    // ...
    // u[n] = command from t-n-1 to t-n
    // dts[0] = time elapsed from timestep t-1 to t
    // ...
    // dts[n] = time elapsed from t-n-1 to t-n
    virtual void ApplyMeasurement(const std::vector < Measurement > &m, const std::vector < NSCommand > &u, const arma::vec &dts, const PFParams *p, f64 shaftLength, arma::mat curvePoints = arma::mat()) = 0;

    // Get all the particle positions
    virtual arma::mat GetParticlePositions(const PFParams *p) = 0;

    // Get all the particle positions
    //    returns:
    //      vector of tip frame orientations
    virtual std::vector < arma::mat33 > GetParticleOrientations(const PFParams *p) = 0;
    
    //Set the number of particles. Causes a resample
    virtual void SetNumberOfParticles(s32 nParticles, const PFParams *p);

    // Returns the effective number of samples in the particle filter. 
    // This metric is used for resampling purposes
    virtual f64 EffectiveSampleSize();

    // Resample particles according to particle weight distribution;
    virtual void Resample(s32 nParticles, const PFParams *p) = 0;

    // Returns curvatures of each particle
    //   returns:
    //   [rho1; ...; rhon] \in R^n, each row represents a curvature value
    virtual arma::mat GetParticleRhos(const PFParams *p) = 0;

    // Returns expected value of particle filter posterior distribution
    // in other words, the estimate of the filer.
    virtual TipState GetExpectedValue(const PFParams *p) = 0;

    virtual void ApplyWeights(const arma::mat &pw);

    // Get Probability Ultrasound Frame is over Needle Shaft
    virtual Vec2d GetProbOverNeedle();
  };

  // Implements full state particle filter.
  // Estimate joint distribution p(P_{t:1},R_{t:1}|Z_{t:1})
  class ParticleFilterFullState : public ParticleFilter 
  {
  protected:
    arma::mat m_pos;                        //position matrix for particles [x1 x2 ... xn] x_i \in R^3 m_pos \in R^(3xn)
    std::vector < arma::mat33 > m_R;        //vector of rotation matrices for particles
    arma::mat m_rho;                        //rhos [rho1 ... rhon] rhoi \in R, m_rhos \in R^(1xn)

  public:
    ParticleFilterFullState(s32 nParticles, std::tr1::shared_ptr < Distribution > pDopOverNeedle, std::tr1::shared_ptr < Distribution > pDopNotOverNeedle, const PFParams *p);

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
    // m[0] = current measurement
    // m[1] = measurement 1 timestep back
    // ...
    // m[n] = measurement n timesteps back
    // u[0] = command from t-1 to t
    // ...
    // u[n] = command from t-n-1 to t-n
    // dts(0) = time elapsed from timestep t-1 to t
    // ...
    // dts(n) = time elapsed from t-n-1 to t-n
    virtual void ApplyMeasurement(const std::vector < Measurement > &m, const std::vector < NSCommand > &u, const arma::vec &dts, const PFParams *p, f64 shaftLength, arma::mat curvePoints = arma::mat());

    // Get all the particle positions
    // return
    //  [x_1 ... x_n] return \in R^(nx3)
    virtual arma::mat GetParticlePositions(const PFParams *p);

    // Get all the particle positions
    //    returns:
    //      vector of tip frame orientations
    virtual std::vector < arma::mat33 > GetParticleOrientations(const PFParams *p);

    // Resample particles according to particle weight distribution;
    virtual void Resample(s32 nParticles, const PFParams *p);


    // Returns curvatures of each particle
    //   returns:
    //   [rho1; ...; rhon] \in R^n, each row represents a curvature value
    virtual arma::mat GetParticleRhos(const PFParams *p);

    // Returns expected value of particle filter posterior distribution
    // in other words, the estimate of the filer.
    virtual TipState GetExpectedValue(const PFParams *p);

    //Used for testing only
    virtual void SetOrientations(const std::vector < arma::mat33 > &ors);
    
    //Used for testing only
    virtual void SetPositions(const arma::mat &pos);
    
    //Used for testing only
    virtual void SetRhos(const arma::mat &rhos);
  };

  struct OrientationKF
  {
    public:
    arma::mat33 mu;                 //represents mean of gaussian distribution
    arma::mat33 sigma;              //covariance of gaussian distribution

    OrientationKF()
    {
      this->mu = (arma::mat33)arma::eye(3,3);
      this->sigma = (arma::mat33)arma::eye(3,3);
    }

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
    ParticleFilterMarginalized(s32 nParticles, std::tr1::shared_ptr < Distribution > pDopOverNeedle, std::tr1::shared_ptr < Distribution > pDopNotOverNeedle, const PFParams *p);

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
    // m[0] = current measurement
    // m[1] = measurement 1 timestep back
    // ...
    // m[n] = measurement n timesteps back
    // u[0] = command from t-1 to t
    // ...
    // u[n] = command from t-n-1 to t-n
    // dts[0] = time elapsed from timestep t-1 to t
    // ...
    // dts[n] = time elapsed from t-n-1 to t-n
    virtual void ApplyMeasurement(const std::vector < Measurement > &m, const std::vector < NSCommand > &u, const arma::vec &dts, const PFParams *p, f64 shaftLength, arma::mat curvePoints = arma::mat());

    // Get all the particle positions
    // return
    //  [x_1 ... x_n] return \in R^(3xn)
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
    virtual void Resample(s32 nParticles, const PFParams *p);


    // Returns curvatures of each particle
    //   returns:
    //   [rho1; ...; rhon] \in R^n, each row represents a curvature value
    virtual arma::mat GetParticleRhos(const PFParams *p);

    // Returns expected value of particle filter posterior distribution
    // in other words, the estimate of the filer.
    virtual TipState GetExpectedValue(const PFParams *p);

    //Used for testing only
    virtual void SetOrientationKFs(const std::vector < OrientationKF > &ors);
    
    //Used for testing only
    virtual void SetPositions(const arma::mat &pos);
    
    //Used for testing only
    virtual void SetRhos(const arma::mat &rhos);
  };

  struct PartMethod1
  {
    arma::mat pos;
    arma::mat rhos;
    std::vector < arma::mat33 > Rs;
    arma::mat ws;
  };

  struct PartMethod3
  {
    arma::mat pos;
    arma::mat rhos;
    std::vector < OrientationKF > Rs;
    arma::mat ws;
  };

  std::vector < Measurement > loadMeasurements(const char *basePath, bool reverse=true);
  void saveMeasurements(const char *basePath, const std::vector < Measurement > &measurements);
  arma::mat loadTimes(const char *basePath);
  void saveTimes(const char *basePath, const arma::mat &ts);
  void saveCurvePoints(const char *basePath, const arma::mat &curvePoints);
  std::vector < NSCommand > loadCommands(const char *basePath);
  void saveCommands(const char *basePath, const std::vector < NSCommand > &commands);
  PartMethod1 loadParticlesMethod1(const char *basePath);
  void saveParticlesMethod1(const char *basePath, const PartMethod1 &particles);
  PartMethod3 loadParticlesMethod3(const char *basePath);
  void saveParticlesMethod3(const char *basePath, const PartMethod3 &particles); 
  void saveOrientations(const std::vector < arma::mat33 > &ors, const char *path);
  std::vector < arma::mat33 > loadOrientations(const char *path);
  void saveTipHistory(const char *basePath, const std::vector < TipState > &hist);
  f64 totalLength(const std::vector < NSCommand > &u, const arma::vec &dts);
}