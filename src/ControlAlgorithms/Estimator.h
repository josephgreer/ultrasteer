#ifndef ESTIMATOR_H
#define ESTIMATOR_H

#include "RTCore.h"
#include <vector>
#include<stdio.h>

#define TIP_LENGTH 15.3

 
//#pragma comment(lib,"ws2_32.lib") //Winsock Library
 
#define BUFLEN 1  //Max length of buffer
#define PORT 9854   //The port on which to listen for incoming data


/// class:		UnscentedKalmanFilter
/// brief:		Filters ultrasound segmentation data based on a stochastic model of needle steering
///
///	author:		Alessandro Diodato
///	date:		  May 14, 2015

namespace Nf
{
  class Estimator
  {
   
    bool Init;
    vector<Vec3d> OBS;
    vector<Vec3d> VER;

    vector<double> L;
    vector<unsigned int> ART;
    vector<int> IND;
    vector<double> THETA;
    double StartArt;
    double ART_ANG;

    
    double rho_n,rho_a,yaw_art;

    Matrix33d ROT_base;
    Vec3d off;
    
    bool firstAdd; 
    

    SOCKET s;
    struct sockaddr_in server, si_other;
    int slen , recv_len;
    char buf[BUFLEN];
    WSADATA wsa;


  private:
    Matrix33d rotx(double roll);
    Matrix33d roty(double pitch);
    Matrix33d rotz(double yaw);
    Matrix33d AxisAngle(Vec3d axis,double angle);
    Matrix44d simulate1Step(double diff_l,double diff_th,unsigned int a,unsigned int prev_a,Matrix44d prevTIP,double L_n);
    

  public: //Methods

    vector<Matrix44d> TIP_t;
    Estimator(void);
    ~Estimator(void);
    void setEstimator(float ins,float appoff,float yaw,unsigned int art);
    void resetEstimator(void);
    bool isInitialized();
    Matrix44d getCurrentEstimateTIP();
    void addPOINT(Vec3d p,Vec3d v);
    void updateInput(double m_l, double m_th,unsigned int a);
    void Estimator::saveDataOpt();
    bool WaitAndCorrect();
    void addTIP();
    void resetaddTIP();
    Matrix44d getCurrentEstimateWRIST();
    double lastTHETA();
    bool testArt(double L_n);
    Matrix44d getCurrentEstimateTIP_trans(float horizon);
    /*void fullUpdateUKF(Vec3d u, Matrix44d z);
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
    Matrix66d m_K;*/
  };
}

#endif