#ifndef UNSCENTEDKALMANFILTER_H
#define UNSCENTEDKALMANFILTER_H

#include "RTCore.h"
#include <vector>


/// class:		UnscentedKalmanFilter
/// brief:		Filters ultrasound segmentation data based on a stochastic model of needle steering
///
///	author:		Troy Adebar
///	date:		  May 14, 2015

namespace Nf
{
  class UnscentedKalmanFilter
  {
  public: //Methods
    UnscentedKalmanFilter(void);
    ~UnscentedKalmanFilter(void);
    void fullUpdateUKF(Vec3d u, Matrix44d z);
    void processUpdateUKF(Vec3d u);
    void getCurrentStateEstimate(Matrix44d &x_out);
    void resetEstimate();    
    void zeroRotationEstimate();
    Matrix66d getCurrentCovariance();
    Matrix66d getCurrentGain();
    Matrix44d getCurrentEstimate();
    Vec3d getCurrentXYZVariance();
    Matrix33d vec2mat(Vec3d r);
    Vec3d mat2vec(Matrix33d R);
    bool isInitialized();
    void initialize(f32 l, f32 theta);
    void sigmas(Matrix44d x, Matrix66d P, std::vector<Matrix44d> &X);
    void findMeanAndCov( std::vector<Matrix44d> X, Matrix44d &x, Matrix66d &P, std::vector<Vec6d> &E);

  private:	//Methods
    void utf(std::vector<Matrix44d> X, Vec3d u, Matrix44d &x_, Matrix66d &P_, std::vector<Vec6d> &Ex_);
    void utg(std::vector<Matrix44d> X, Matrix44d &z, Matrix66d &Pzz, std::vector<Vec6d> &Ezz);
    Matrix44d f(Matrix44d x1, Vec3d u);
    Matrix44d g(Matrix44d x);
    Vec6d differentialPose( Matrix44d a, Matrix44d b );
    Matrix44d addDifferentialPose( Matrix44d a, Vec6d d );
    Matrix33d Rz(double d_th);
    Matrix33d Rx(double d_th);
    double safeAcos(double x);
    

  private:	//Attributes
    Matrix44d x_hat;
    Matrix66d P_hat, Q, R;
    bool m_initialized;
    Matrix66d m_K;
  };
}

#endif