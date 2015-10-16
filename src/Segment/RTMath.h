#pragma once
#pragma warning(push,0)

#include "RTCore.h"

namespace Nf
{

  ///////////////////////////////////////////////////////////////
  // Begin SO(3) Math
  ///////////////////////////////////////////////////////////////
  arma::mat33 SO3Hat(const arma::vec3 &x);
  arma::vec3 SO3HatInverse(const arma::mat33 &x);
  arma::mat33 SO3Exp(const arma::vec3 &x);
  arma::vec3 SO3Log(const arma::mat33 &x);
  arma::mat33 Rx(f64 theta);
  arma::mat33 Ry(f64 theta);
  arma::mat33 Rz(f64 theta);

  ///////////////////////////////////////////////////////////////
  // End SO(3) Math
  ///////////////////////////////////////////////////////////////


  //implements multivariate gaussians sampling
  template < class V, class M >
  class Gaussian
  {
  protected:
    bool m_sampleInit;
    V m_mu;           //mean
    M m_E;            //covariance matrix
    M m_L;            //factor covariance matrix using cholesky E = LL^T to generate random samples
    M m_Ei;           //inverse of covariance matrix (used for evaluating pdf)
    f64 m_norm;       //normalizing constant for evaluating pdf

  public:
    Gaussian(const V &mu, const M &E)
      : m_mu(mu)
      , m_E(E)
      , m_norm(-1)
      , m_sampleInit(false)
    {
    }

    void InitForSampling()
    {
      arma::vec m_lambda;
      arma::mat m_w;
      bool rv = arma::eig_sym(m_lambda, m_w, m_E);
      assert(rv);
      m_L = (M)(m_w*arma::diagmat(arma::sqrt(m_lambda)));
      m_sampleInit = true;
    }

    arma::mat Sample(s32 n)
    {
      if(!m_sampleInit)
        InitForSampling();

      //res \in R^(dxn) where d is dimensionality of data and n is number of samples
      //res = [s1 s2 ... sn] s_i \in R^d
      arma::mat res(m_mu.n_rows, n);
      res.randn();
      res = m_L*res;
      res = res+arma::repmat(m_mu,1,n);

      return res;
    }

    V Sample()
    {
      if(!m_sampleInit)
        InitForSampling();

      //res \in R^d where d is dimensionality of m_mu
      V res = arma::randn(m_mu.n_rows,1);

      res = m_L*res+m_mu;

      return res;
    }

    f64 Eval(V v)
    {
      //we haven't calculated sigma inverse or normalizing constant yet
      if(m_norm < 0) {
        M li = inv(m_L);
        m_Ei = (M)(li.t()*li);
        m_norm = 1/sqrt(pow((f64)(2*PI),(f64)v.n_rows)*det(m_E));
      }

      V delta = (v-m_mu);
      arma::mat inner = -0.5*delta.t()*m_Ei*delta;
      return (f64)(m_norm*exp(inner(0,0)));
    }
    
  };

  //
  arma::vec2 normcdf2(arma::vec2 x, arma::vec2 mu, arma::vec2 sigma);

  //  ////////////////////////////////////////////////////
  //    This function returns the pdf of a truncated gaussian
  //    x = nx1 vector to look up
  //    mu = nx1 vector representing the mean of the truncated gaussian
  //    sigma = nx1 standard deviation vector. (all cross-correlations zero)
  //    a = nx1 to truncate distribution to (lower limit)
  //    b = nx1 to truncate distribution to (upper limit)
  //    return values
  //    density value 
  f64 TruncatedIndependentGaussianPDF2(arma::vec2 x, arma::vec2 mu, arma::vec2 sigma, arma::vec2 a, arma::vec2 b);
  
  //log-normal pdf with associated normal distrubtion with mu and sigma
  f64 lognpdf(f64 x, f64 mu, f64 sigma);

  f64 erf(f64 x);

  // Return the Karcher mean of rotations in Rs weighted by weights in ws
  // logically w[0]*R[0]+...+w[n]*R[n] 
  // ws = [w1 ... wn] \in R^(1xn)
  arma::mat33 SO3Mean(const std::vector < arma::mat33 > & Rs, const arma::mat & ws, f64 eps);
  
  //computes the distance between to orientations using Riemannian metric
  //on SO(3)
  f64 SO3Distance(const arma::mat33 &R1, const arma::mat33 &R2);

  arma::uvec Sample(const arma::vec &dist, s32 n);

  class Polynomial
  {
  protected:
    arma::mat m_coeffs;

  public:
    // Assume coeffs \in R^(dxn)
    // n is the dimensionality of the polynomial curve
    // d is the degree of the polynomial
    Polynomial(const arma::mat &coeffs);

    // Generate least squares fit polynomial for points
    // pts \in R^(mxn) m = number of samples, n = dimensionality of points
    // degree = degree of polynomial to fit
    // coord = coordinate to use for parameter (i.e. coord == 0 is x, coord == 1 is y, etc.)
    Polynomial(const arma::mat &pts, s32 degree, s32 coord);

    //Evaluate polynomial at point t
    arma::vec operator()(f64 t);

    //Print coefficients of polynomial with preamble
    void Print(std::string preamble);

    // Generate uniformly spaced points from a to b separated by ds
    arma::mat UniformlySpacedPoints(f64 a, f64 b, f64 ds, s32 nTries);
  };
}
