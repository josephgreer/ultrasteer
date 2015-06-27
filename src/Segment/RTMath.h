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

  ///////////////////////////////////////////////////////////////
  // End SO(3) Math
  ///////////////////////////////////////////////////////////////


  //implements multivariate gaussians sampling
  template < class V, class M >
  class Gaussian
  {
  protected:
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
    {
      assert(arma::chol(m_L, m_E, "lower"));
    }

    arma::mat Sample(s32 n)
    {
      //res \in R^(dxn) where d is dimensionality of data and n is number of samples
      //res = [s1 s2 ... sn] s_i \in R^d
      arma::mat res(m_mu.n_rows, n);
      res.randn();

      res = m_L*res+arma::repmat(m_mu,1,n);

      return res;
    }

    V Sample()
    {
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
}
