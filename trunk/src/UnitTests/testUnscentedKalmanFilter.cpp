#include "CppUnitLite/Test.h"
#include "SegmentCore.h"
#include "Segment.h"
#include "UnscentedKalmanFilter.h"

#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <Windows.h>

using namespace Nf;

TEST(Basic, UKF)
{
	// Create a UKF and check initial x, P
  UnscentedKalmanFilter m_UKF;
  Matrix44d x_hat = m_UKF.getCurrentEstimate();
  Matrix66d P_hat = m_UKF.getCurrentCovariance();
  
  NTrace("\nx_hat at init\n");
  x_hat.Print();
  NTrace("\n");

  NTrace("\nP_hat at init\n");
  P_hat.Print();
  NTrace("\n");

  // Create some simple inputs and measurements
  Vec3d u(0, 1000, 2);
  Matrix33d R = Matrix33d::I();
  Vec3d p(1.0, 1.0, 1.0);
  Matrix44d z = Matrix44d::FromOrientationAndTranslation(R,p);

  // Update the filter and print the results
  m_UKF.updateUKF(u,z);
  x_hat = m_UKF.getCurrentEstimate();
  P_hat = m_UKF.getCurrentCovariance();
    
  NTrace("\nx_hat after first update\n");
  x_hat.Print();
  NTrace("\n");

  NTrace("\nP_hat after first update\n");
  P_hat.Print();
  NTrace("\n");

    // Create some more simple inputs and measurements
  u = Vec3d(0.5, 50, 100);
  R = Matrix33d::I();
  p = Vec3d(1,1,3);
  z = Matrix44d::FromOrientationAndTranslation(R,p);

  // Update the filter and print the results
  m_UKF.updateUKF(u,z);
  x_hat = m_UKF.getCurrentEstimate();
  P_hat = m_UKF.getCurrentCovariance();
    
  NTrace("\nx_hat after second update\n");
  x_hat.Print();
  NTrace("\n");

  NTrace("\nP_hat after second update\n");
  P_hat.Print();
  NTrace("\n");

}

TEST(Vec2matMat2vec, UKF)
{
	// Create a UKF and check initial x, P
   UnscentedKalmanFilter m_UKF;

  // Create a rotation vector
  Vec3d r1(0.5, 0.2, 0.1);

  // Turn it into a rotation matrix and display
  Matrix33d R1 = m_UKF.vec2mat(r1);
  NTrace("\nR1 = \n");
  R1.Print();
  NTrace("\n");

  // Turn it back into a rotation vector
  Vec3d r2 = m_UKF.mat2vec(R1);
}