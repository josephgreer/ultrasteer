#include "UnscentedKalmanFilter.h"
#include <vector>
#include <vnl/vnl_cross.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_rotation_matrix.h>
#include <vnl/algo/vnl_cholesky.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <math.h>

#define		N					6						// length of state vector
#define		M					6						// length of measurement vector

#define		P_POS_INIT			3.0						// prior covariance of position
#define		P_ROT_INIT			1e-2					// prior covariance of orientation

#define		Q_POS				0.2						// process covariance of position
#define		Q_ROT				1e-3					// process covariance of orientation

#define		R_POS				1.0						// measurement covariance of position
#define		R_ROT				1.0						// measurement covariance of orientation

#define		ALPHA				1e-3					// Sigma parameter for UKF
#define		KI					0.0						// Sigma parameter for UKF
#define		BETA				2.0						// Sigma parameter for UKF
#define		LAMBDA				((pow(ALPHA,2.0))*(N+KI)-N)		// Sigma parameter for UKF
#define		C					(N+LAMBDA)				// Sigma parameter for UKF

/// \brief		Constructor: Initializes filter	
UnscentedKalmanFilter::UnscentedKalmanFilter(void):
x_hat(N,0.0),P_hat(N,N,0.0),K(N,M,0.0),Q(N,N,0.0),R(M,M,0.0),Wm(1,2*N+1,0.5/C),Wc(1,2*N+1,0.5/C)
{
	// Set mean of initial position prior (orientation already 0)
	x_hat[0] = 0.0; x_hat[1] = 40.0; x_hat[2] = -15.0;
	x_hat[3] = 0.0; x_hat[4] = 0.0; x_hat[5] = 0.0;
	// Set diagonal covariance entries for prior
	P_hat.fill_diagonal(P_POS_INIT);
	P_hat(3,3) = P_ROT_INIT; P_hat(4,4) = P_ROT_INIT; P_hat(5,5) = P_ROT_INIT;
	// Set diagonal covariance entries for process noise
	Q.fill_diagonal(Q_POS);
	Q(3,3) = Q_ROT; Q(4,4) = Q_ROT; Q(5,5) = Q_ROT;
	// Set diagonal covariance entries for measurement noise
	R.fill_diagonal(R_POS);
	R(3,3) = R_ROT; R(4,4) = R_ROT; R(5,5) = R_ROT;
	// Set weighting for UKF sigma points
	Wm(0,0) = (LAMBDA)/(C);	
	Wc = Wm;
	double temp = Wc(0,0);
	Wc(0,0) = temp + 1-pow(ALPHA,2)+BETA;
}

/// \brief		Destructor
UnscentedKalmanFilter::~UnscentedKalmanFilter(void)
{

}

/// \brief		Update Kalman filter
void UnscentedKalmanFilter::updateUKF(vnl_vector<double> u, vnl_vector<double> z)
{
	// create Sigma points X for the current estimate distribution
	vnl_matrix<double> X(N,2*N+1);
	sigmas(x_hat, P_hat+Q, sqrt(C), X);
	
	vnl_matrix<double> Dbg;

	qDebug() << "X: "; 
	for( int i = 0; i<6; i++)
	{
		qDebug() << X(i,0) << " " << X(i,1) << " " << X(i,2) << " " << X(i,3) << " " << X(i,4) << " " << X(i,5) << " "
			<< X(i,6) << " " << X(i,7) << " " << X(i,8) << " " << X(i,9) << " " << X(i,10) << " " << X(i,11)
			<< " " << X(i,12);
	}

	// apply unscented transformation for process model 
	vnl_vector<double> x1(6);
	vnl_matrix<double> X1(N,2*N+1), P1(N,N), X2(N,2*N+1);
	utf(X, u, x1, X1, P1, X2);

	// apply unscented transformation for observation model
	vnl_vector<double> z1(M);
	vnl_matrix<double> Z1(M,2*M+1), P2(M,M), Z2(M,2*M+1);
	utg(X1, z1, Z1, P2, Z2);
	// define transformated cross-covariance
	vnl_matrix<double> WC(2*N+1,2*N+1,0.0);
	WC.set_diagonal(Wc.get_row(0));
	vnl_matrix<double> P12 = X2*WC*Z2.transpose();
	// perform state update
	K = P12*vnl_matrix_inverse<double>(P2);
	x_hat = x1 + K*(z-z1);
	// perform covariance update
	P_hat = P1 - K*P12.transpose();
}


/// \brief		Unscented transform of process Sigma points
void UnscentedKalmanFilter::utf(vnl_matrix<double> X, vnl_vector<double> u,
	vnl_vector<double> &y, vnl_matrix<double> &Y, vnl_matrix<double> &P, vnl_matrix<double> &Y1)
{
	// determine number of sigma points
	unsigned int L = X.cols();
	// zero output matrices
	y.fill(0.0); Y.fill(0.0);

	// transform the sigma points and put them as columns in a matrix Y
	for( int k = 0; k < L; k++ )
	{
		vnl_vector<double> xk = X.get_column(k);
		vnl_vector<double> yk(N);
		f(xk,u,yk);
		Y.set_column(k,yk);
		// add each transformed point to the weighted mean
		y = y + Wm.get(0,k)*yk;
	}

	// create a matrix with each column being the weighted mean
	vnl_matrix<double> Ymean(N,L);
	for( int k = 0; k < L; k++ )
		Ymean.set_column(k,y);
	// set the matrix of difference vectors
	Y1 = Y-Ymean;
	// calculate the covariance matrix output
	vnl_matrix<double> WC(L,L,0.0);
	WC.set_diagonal(Wc.get_row(0));
	P = Y1*WC*Y1.transpose();
}


/// \brief		Unscented transform of process Sigma points
void UnscentedKalmanFilter::utg(vnl_matrix<double> X, 
	vnl_vector<double> &y, vnl_matrix<double> &Y, vnl_matrix<double> &P, vnl_matrix<double> &Y1)
{
	// determine number of sigma points
	unsigned int L = X.cols();
	// zero output matrices
	y.fill(0.0); Y.fill(0.0);
	// transform the sigma points and put them as columns in a matrix Y
	for( int k = 0; k < L; k++ )
	{
		vnl_vector<double> xk = X.get_column(k);
		vnl_vector<double> yk(M);
		g(xk,yk);
		Y.set_column(k,yk);
		// add each transformed point to the weighted mean
		y = y + Wm.get(0,k)*yk;
	}
	// create a matrix with each column being the weighted mean
	vnl_matrix<double> Ymean(N,L);
	for( int k = 0; k < L; k++ )
		Ymean.set_column(k,y);
	// set the matrix of difference vectors
	Y1 = Y-Ymean;
	// calculate the covariance matrix output
	vnl_matrix<double> WC(L,L,0.0);
	WC.set_diagonal(Wc.get_row(0));
	P = Y1*WC*Y1.transpose() + R;
}


/// \brief		Create Sigma point to represent a normal distribution
void UnscentedKalmanFilter::sigmas(vnl_vector<double> x, vnl_matrix<double> P, double c, vnl_matrix<double> &X)
{
	// Copy matrix P to A
	vnl_matrix<double> A = P;

	// Compute the Cholesky decomposition of P
	vnl_cholesky chol(A,vnl_cholesky::verbose);
	A = chol.upper_triangle();
	// Apply the weight c
	A = c*A.transpose();

	// Create matrix Y with copies of reference point x
	vnl_matrix<double> Y(N,N);
	for( int i = 0; i<N; i++ )
		Y.set_column(i,x);
	// Add and subtract A from Y
	vnl_matrix<double> YpA = Y + A;
	vnl_matrix<double> YmA = Y - A;
	// Set columns of X with Sigma points
	X.set_column(0,x);
	for( int i = 0; i<N; i++ )
	{
		X.set_column(i+1,YpA.get_column(i));
		X.set_column(i+1+N,YmA.get_column(i));
	}
}


/// \brief		Nonlinear process model for needle steering
void UnscentedKalmanFilter::f(vnl_vector<double> x1, vnl_vector<double> u, 
	vnl_vector<double> &x2)
{
	// initialize the output to 0
	x2.fill(0.0);
	// isolate the position and orientation components of the input vector
	vnl_vector<double> p = x1.extract(3,0);
	vnl_vector<double> r = x1.extract(3,3);
	// change rotation vector representation to quaternion
	vnl_vector<double> r_norm = r;
	r_norm.normalize();
	vnl_quaternion<double> q(r_norm,r.magnitude());
	
	// isolate specific input variables
	double d_th = u[0];
	double ro = u[1];
	double l = u[2];

	// define x,z axes as vectors
	vnl_matrix<double> I(3,3); I.set_identity();
	vnl_vector<double> x_axis = I.get_column(0);
	vnl_vector<double> z_axis = I.get_column(2);

	// Update position
	
	// define rotation matrix for d_th about z_axis
	vnl_matrix<double> Rz_dth = vnl_rotation_matrix( (d_th*z_axis) );
	
	// define circular trajectory in y-z plane
	vnl_vector<double> circ(3,0.0);
	circ[1] = ro*(1-cos(l/ro));
	circ[2] = ro*sin(l/ro);

	// define delta position vector in current frame
	vnl_vector<double> d_p = Rz_dth*circ;

	// Transform delta vector into world frame using quaternion rotation
	vnl_vector<double> d_p_world = q.rotate(d_p);

	// add rotated vector to original position
	vnl_vector<double> p2 = d_p_world + p;

	// Update orientation

	// form quaternions for x-axis and z-axis rotations
	vnl_quaternion<double> q_b(z_axis,d_th);
	vnl_quaternion<double> q_a(x_axis,-l/ro);
	// multiply original orientation quaternion
	vnl_quaternion<double> q2 = q*q_b*q_a;
	vnl_vector<double> r2 = q2.axis()*q2.angle();

	// Compose final output
	for( int i = 0; i < 3; i++)
	{
		x2[i] = p2[i];
		x2[i+3] = r2[i];
	}
}

/// \brief		Nonlinear observation model for needle steering
void UnscentedKalmanFilter::g(vnl_vector<double> x, vnl_vector<double> &z)
{
	z = x;
}

/// \brief		Get the current needle state estimate
void UnscentedKalmanFilter::getCurrentStateEstimate(vnl_vector<double> &x_out)
{
	x_out = x_hat;
}

/// \brief		Reset the Kalman filter estimate
void UnscentedKalmanFilter::resetEstimate()
{
	// Set mean of initial position prior (orientation already 0)
	x_hat[0] = 0.0; x_hat[1] = 40.0; x_hat[2] = -15.0;
	x_hat[3] = 0.0; x_hat[4] = 0.0; x_hat[5] = 0.0;
	// Set diagonal covariance entries for prior
	P_hat.fill(0.0);
	P_hat.fill_diagonal(P_POS_INIT);
	P_hat(3,3) = P_ROT_INIT; P_hat(4,4) = P_ROT_INIT; P_hat(5,5) = P_ROT_INIT;
}

/// \brief Zero the orientation portion of the state estimate.
/// Avoids the orientation estimate gettings messed up by poor initial measurements
void UnscentedKalmanFilter::zeroRotationEstimate()
{
	x_hat[3] = 0.0; x_hat[4] = 0.0; x_hat[5] = 0.0;
	for (int i = 3; i<6; i++)
	{
		for (int j = 3; j<6; j++)
		{
			P_hat(i,j) = 0.0;
		}
	}
	P_hat(3,3) = P_ROT_INIT; P_hat(4,4) = P_ROT_INIT; P_hat(5,5) = P_ROT_INIT;
}