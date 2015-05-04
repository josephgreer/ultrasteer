#include <vector>
#include <vnl/vnl_cross.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_quaternion.h>
#include <QDebug>

/// class:		UnscentedKalmanFilter
/// brief:		Filters ultrasound segmentation data based on 
///				a statistical model of needle steering
///
///	author:		Troy Adebar
///	date:		January 15, 2014

#pragma once

class UnscentedKalmanFilter
{
public: //Methods
	UnscentedKalmanFilter(void);
	~UnscentedKalmanFilter(void);
	void UnscentedKalmanFilter::updateUKF(vnl_vector<double> u, vnl_vector<double> z);
	void UnscentedKalmanFilter::getCurrentStateEstimate(vnl_vector<double> &x_out);
	
public:	//Methods
	void utf(vnl_matrix<double> X, vnl_vector<double> u, vnl_vector<double> &y, 
		vnl_matrix<double> &Y, vnl_matrix<double> &P, vnl_matrix<double> &Y1);
	void utg(vnl_matrix<double> X, vnl_vector<double> &y, 
		vnl_matrix<double> &Y, vnl_matrix<double> &P, vnl_matrix<double> &Y1);
	void sigmas(vnl_vector<double> x, vnl_matrix<double> P, double c, vnl_matrix<double> &X);
	void f(vnl_vector<double> x1, vnl_vector<double> u, vnl_vector<double> &x2);
	void g(vnl_vector<double> x, vnl_vector<double> &z);
	void resetEstimate();
	void zeroRotationEstimate();
	

private:	//Attributes
	vnl_vector<double> x_hat;
	vnl_matrix<double> P_hat,K,Q,R,Wm,Wc;
};
