#include "UnscentedKalmanFilter.h"

#define		N					      6.0				        // length of state vector
#define   SQN             2.4495            // sqrt(N)

#define		P_POS_I   			3.0				        // prior covariance of position
#define		P_ROT_I   			1e-2			        // prior covariance of orientation

#define		Q_POS				    0.2				        // process covariance of position
#define		Q_ROT				    1e-3			        // process covariance of orientation

#define		R_POS				    1.0				        // measurement covariance of position
#define		R_ROT				    1.0				        // measurement covariance of orientation

#define   PI              3.141             // pi




namespace Nf
{
  /// \brief		Constructor: Initializes filter	
  UnscentedKalmanFilter::UnscentedKalmanFilter(void):
      x_hat(Matrix44d::Zero())
    , m_initialized(false)
    , Q( Matrix66d::Diagonal(Q_POS, Q_POS, Q_POS, Q_ROT, Q_ROT, Q_ROT) )
    , R( R = Matrix66d::Diagonal(R_POS, R_POS, R_POS, R_ROT, R_ROT, R_ROT) )
  {
  }

  /// \brief		Destructor
  UnscentedKalmanFilter::~UnscentedKalmanFilter(void)
  {
  }

  /// Initialize the UKF based on the robot insertion l and robot rotation theta from nominal
  void UnscentedKalmanFilter::initialize(f32 l, f32 theta)
  {
    // initialize estimate and covariance matrices
    x_hat = Matrix44d::FromOrientationAndTranslation(Rz(theta*PI/180.0),Vec3d(0.0,0.0,l));
    P_hat = Matrix66d::Diagonal(P_POS_I,P_POS_I,P_POS_I,P_ROT_I,P_ROT_I,P_ROT_I);
    m_initialized = true;
  }

  /// \brief		Update Kalman filter using measurement z
  void UnscentedKalmanFilter::fullUpdateUKF(Vec3d u, Matrix44d z)
  {
    if( m_initialized ){
      // create Sigma points X for the current estimate distribution
      std::vector<Matrix44d> X;
      sigmas(x_hat, P_hat+Q, X);

      // apply unscented transformation for process model 
      Matrix44d x_;                 // transformed mean
      Matrix66d P_;                 // transformed covariance
      std::vector<Vec6d> Ex_;       // transformed error vectors
      utf(X, u, x_, P_, Ex_);

      // resample sigma points
      std::vector<Matrix44d> X_;    
      sigmas(x_, P_+R, X_);

      // apply unscented transform for measurement model
      Matrix44d z_;                 // transformed measurement mean
      Matrix66d Pzz;                // transformed measurement covariance
      std::vector<Vec6d> Ez;        // transformed measurement error vectors
      utg(X_, z_, Pzz, Ez);

      // define transformated cross-covariance
      Matrix66d Pxz;
      for( int i = 0; i < Ez.size(); i++ )
        Pxz += ( (Matrix66d::OuterProduct(Ex_[i],Ez[i]) )*( 0.5/N ) );
      Matrix66d K = Pxz*Pzz.Inverse();

      // update state and covariance
      x_hat = addDifferentialPose( x_, K*differentialPose( z, z_) );
      P_hat = P_ - K*Pzz*K.Transpose();    

      x_hat = z;
    }
  }

  /// \brief		Update Kalman filter without measurement
  void UnscentedKalmanFilter::processUpdateUKF(Vec3d u)
  {
    if( m_initialized ){
      // create Sigma points X for the current estimate distribution
      std::vector<Matrix44d> X;
      sigmas(x_hat, P_hat+(Q*(u.z/5.0)), X); // scale uncertainty by length of insertion

      // apply unscented transformation for process model 
      std::vector<Vec6d> Ex_;       // transformed error vectors
      Matrix44d old_x_hat = x_hat;
      utf(X, u, x_hat, P_hat, Ex_);
    }
  }
  
  /// \brief		Unscented transform of process Sigma points
  void UnscentedKalmanFilter::utf(std::vector<Matrix44d> X, Vec3d u, 
    Matrix44d &x_, Matrix66d &P_, std::vector<Vec6d> &Ex_)
  {
    // determine number of sigma points
    unsigned int l = X.size();

    // transform the sigma points
    std::vector<Matrix44d> X_; 
    X_.clear();
    for( int k = 0; k < l; k++ )
      X_.push_back( f(X[k],u) );

    // find the mean, covariance, and error vectors for the set
    findMeanAndCov( X_, x_, P_, Ex_ );
  }


  /// \brief		Unscented transform of observation Sigma points
  void UnscentedKalmanFilter::utg(std::vector<Matrix44d> X, 
    Matrix44d &z, Matrix66d &Pzz, std::vector<Vec6d> &Ezz)
  {
    // determine number of sigma points
    unsigned int l = X.size();

    // transform the sigma points
    std::vector<Matrix44d> Z;
    Z.clear();
    for( int k = 0; k < l; k++ )
      Z.push_back( g(X[k]) );
    // find the mean, covariance, and error vectors for the set
    findMeanAndCov( Z, z, Pzz, Ezz );
  }

  /// \brief		Nonlinear process model for needle steering
  Matrix44d UnscentedKalmanFilter::f(Matrix44d x1, Vec3d u)
  {
    // isolate the position and orientation components of the input vector
    Vec3d p1 = x1.GetPosition();
    Matrix33d R1 = x1.GetOrientation();

    // isolate specific input variables
    double d_th = u.x;
    double rho = u.y;
    double l = u.z;

    // update tip position
    Vec3d d_p(0.0, rho*(1.0-cos(l/rho)), rho*(sin(l/rho)));
    Vec3d p2 = p1 + R1*Rz(d_th)*d_p;

    // update tip orientation
    Matrix33d R2 = R1*Rz(d_th)*Rx(-l/rho);

    // set the output to 
    Matrix44d x2 = Matrix44d::FromOrientationAndTranslation(R2,p2);

    return x2;
  }

  /// \brief		Observation model for needle steering
  Matrix44d UnscentedKalmanFilter::g(Matrix44d x)
  {
    Matrix44d z = x;
    return z;
  }

  /// \brief    Find mean, covariance, and deviation vectors (from mean) for vector of 4x4 pose matrices
  void UnscentedKalmanFilter::findMeanAndCov( std::vector<Matrix44d> X, Matrix44d &x, Matrix66d &P, std::vector<Vec6d> &E)
  {
    // find number of matrices
    unsigned int l = X.size();

    // find mean position
    Vec3d mu_p;
    for( int i = 0; i<l; i++ )
      mu_p += X[i].GetPosition();
    mu_p = mu_p/l;

    // find position deviations
    std::vector<Vec3d> E_p;
    for( int i = 0; i<l; i++ )
      E_p.push_back(X[i].GetPosition() - mu_p);
    
    // iteratively find average orientation and deviations
    Matrix33d mu_R = X[0].GetOrientation();
    Matrix33d mu_R_old = Matrix33d::I();
    
    std::vector<Vec3d> E_R;
    E.resize(l);
    E_R.resize(l);
        
    while(mat2vec(mu_R*mu_R_old.Inverse()).magnitude() > 1.0e-5) // loop to convergence
    {
      Vec3d E_Rsum;
      for( int i = 0; i<l; i++ )
      {
        // Debug
        Matrix33d Inv = mu_R.Inverse();
        Matrix33d Ri = X[i].GetOrientation();
        Vec3d d_v = mat2vec(Ri*Inv);
        E_R[i] = mat2vec(X[i].GetOrientation()*mu_R.Inverse());
        E_Rsum += E_R[i];
      }
      mu_R_old = mu_R;
      mu_R = vec2mat(E_Rsum/l)*mu_R;     
    }

    // calculate covariance and compose deviations
    P = Matrix66d::Zero();
    for( int i = 0; i<l; i++ )
    {
      E[i] = Vec6d::From2x3s(E_p[i],E_R[i]);
      P += (Matrix66d::OuterProduct(E[i],E[i])/l);
    }

    // compose mean
    x = Matrix44d::FromOrientationAndTranslation(mu_R, mu_p);
  }

  /// \brief		Get the mean of the current estimate
  Matrix44d UnscentedKalmanFilter::getCurrentEstimate(void)
  {
    return x_hat;
  }

  /// \brief		Get the covariance of the current estimate
  Matrix66d UnscentedKalmanFilter::getCurrentCovariance(void)
  {
    return P_hat;
  }
  
  /// \brief  Get the current variance of the x,y,z estimates
  Vec3d UnscentedKalmanFilter::getCurrentXYZVariance()
  {
    return Vec3d(P_hat.m_data[0][0],P_hat.m_data[1][1],P_hat.m_data[2][2]);
  }

  /// \brief		Create Sigma point to represent a normal distribution
  void UnscentedKalmanFilter::sigmas(Matrix44d x, Matrix66d P, std::vector<Matrix44d> &X)
  {
    // Clear input matrix X
    X.clear();

    // Compute the Cholesky decomposition of P
    Matrix66d S = P.cholesky();
    Matrix66d Spos = S*SQN;
    Matrix66d Sneg = S*-SQN;

    // Get the mean position and orientation
    Vec3d mu_p = x.GetPosition();
    Matrix33d mu_R = x.GetOrientation();

    // Add each difference to the mean and save
    for( int i = 0; i<2*N; i++ )
    {
      Vec3d d_p, d_r;
      if( i<N )
      {
        d_p = Vec3d(Spos.Col(i).x,Spos.Col(i).y,Spos.Col(i).z);
        d_r = Vec3d(Spos.Col(i).a,Spos.Col(i).b,Spos.Col(i).c);
      }else
      {
        d_p = Vec3d(Sneg.Col(i-N).x,Sneg.Col(i-N).y,Sneg.Col(i-N).z);
        d_r = Vec3d(Sneg.Col(i-N).a,Sneg.Col(i-N).b,Sneg.Col(i-N).c);
      }
      Vec3d p = mu_p + d_p;
      Matrix33d R = mu_R*vec2mat(d_r);
      X.push_back(Matrix44d::FromOrientationAndTranslation(R,p));
    }
  }

    /// \brief    Create a 3x3 rotation z-axis rotation matrix parameterized by th (in rad)
  Matrix33d UnscentedKalmanFilter::Rz( double th)
  {
    Matrix33d res(cos(th), -sin(th), 0.0, 
                  sin(th),  cos(th), 0.0, 
                  0.0,      0.0,     1.0);
    return res;
  }

  /// \brief    Create a 3x3 rotation x-axis rotation matrix parameterized by al (in rad)
  Matrix33d UnscentedKalmanFilter::Rx( double al)
  {
    Matrix33d res(  1.0,   0.0,      0.0,
                    0.0,   cos(al),  -sin(al), 
                    0.0,   sin(al),  cos(al) );
    return res;
  }
  
  /// \brief    Create a 3x3 rotation matrix equivalent to rotation vector input
  Matrix33d UnscentedKalmanFilter::vec2mat(Vec3d r)
  {
    // If the rotation vector is all zeros, return the identity matrix
    if (r.magnitude() < 1e-6)
      return Matrix33d::I();

    f64 c,s,v,x,y,z;
    c = cos(r.magnitude());
    s = sin(r.magnitude());
    v = 1 - c;
    Vec3d rn = r.normalized();
    x = rn.x;
    y = rn.y;
    z = rn.z;

    Vec3d i( (x*x*v)+c, (x*y*v)-(z*s), (x*z*v)+(y*s) );
    Vec3d j( (x*y*v)+(z*s), (y*y*v)+c, (y*z*v)-(x*s) );
    Vec3d k( (x*z*v)-(y*s), (y*z*v)+(x*s), (z*z*v)+c );

    return Matrix33d::FromRows(i,j,k);
  }
  
  /// \brief    Create a rotation vector equivalent to 3x3 rotation matrix input
  Vec3d UnscentedKalmanFilter::mat2vec(Matrix33d R)
  {
    f64 th = safeAcos( (R.Trace()-1.0)/2.0 );

    if( fabs(th) < 1e-3 )
      return Vec3d(0.0,0.0,0.0);

    Vec3d skw(    R.m_data[2][1]-R.m_data[1][2],
                  R.m_data[0][2]-R.m_data[2][0],
                  R.m_data[1][0]-R.m_data[0][1] );
    
    Vec3d retval = skw/(2.0*sin(th))*th;

    return retval;
  }

  /// \brief    Find the 6x1 vector difference between two poses (a-b)
  Vec6d UnscentedKalmanFilter::differentialPose( Matrix44d a, Matrix44d b )
  {
    Vec3d d_p = a.GetPosition() - b.GetPosition();
    Matrix33d d_R = a.GetOrientation()*b.GetOrientation().Inverse();
    Vec3d d_r = mat2vec(d_R);

    return Vec6d::From2x3s(d_p,d_r);
  }

  /// \brief    Add the 6x1 vector difference to a pose
  Matrix44d UnscentedKalmanFilter::addDifferentialPose( Matrix44d a, Vec6d d )
  {
    Vec3d d_p(d.x, d.y, d.z);
    Vec3d d_r(d.a, d.b, d.c);
    
    Vec3d p = a.GetPosition() + d_p;
    Matrix33d R = a.GetOrientation()*vec2mat(d_r);

    return Matrix44d::FromOrientationAndTranslation(R, p);
  }
  
  /// Get the current needle state estimate
  void UnscentedKalmanFilter::getCurrentStateEstimate(Matrix44d &x_out)
  {
    x_out = x_hat;
  }

  /// Return whether the UKF has been initialized
  bool UnscentedKalmanFilter::isInitialized(void)
  {
    return m_initialized;
  }

  double UnscentedKalmanFilter::safeAcos( double x )
  {
    if (x < -1.0) x = -1.0 ;
    else if (x > 1.0) x = 1.0 ;
    return acos (x) ;
  }
  }