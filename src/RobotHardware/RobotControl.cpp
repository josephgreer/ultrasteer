#include "stdafx.h"
#include "RobotControl.h"
#include "NeedleSteeringRobot.h"
#include "math.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <vnl/vnl_cross.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/algo/vnl_svd.h>
#include "Propello.h"
#include <sstream>

#define PI 3.14159265
#define NEEDLE_MIN_R 51.4
#define MIN_DC 0.0
#define MAX_DC 1.0
#define DCINSERTION 5.0
#define N_INIT	3
#define CLINSVEL 10.0 // insertion velocity [RPM] for closed-loop steering

RobotControl::RobotControl(Propello* theParent): ParentWindow(theParent)
{
	m_index = 0;
	m_t.set_size(3);
	m_u.set_size(3);
	m_u.put(0,0);
	m_u.put(1,NEEDLE_MIN_R);
	m_u.put(2,0);
	m_stepNumber = 1;
	m_file = NULL;
}

RobotControl::~RobotControl()
{
}

void RobotControl::InitializeRoll(QString Com)
{
	m_robot.InitializeRoll(Com);
}

void RobotControl::InitializeInsertion(QString Com)
{
	m_robot.InitializeInsertion(Com);
}

void RobotControl::beginClosedLoopSteering()
{
	// Home the roll stage
	homeRoll();
	// Reset the system for a new steering sequence
	resetSystem();
}

void RobotControl::stopClosedLoopSteering()
{
	// Stop the insertion stage
	SetInsertionVelocity(0);
	// Cancel duty cycling 
	m_robot.cancelDutyCycling();
}

bool RobotControl::runControlLoop(vnl_vector<double> z)
{
	// Declare outpout boolean (true indicates needle steering is complete)
	bool steeringComplete = false;
	// For UKF, update u vector with actual change in insertion position
	updateU();
	// Update the UKF
	m_UKF.updateUKF(m_u, z);
	// Check number of updates
	if ( m_stepNumber <= N_INIT) // For the first few UKF scans/steps
	{
		// Zero the orientation estimate, since it's bad info
		m_UKF.zeroRotationEstimate();
	}
	else // Once beyond the initial UKF convergence scans 
	{
		// Set insertion velocity
		m_robot.SetInsertionVelocity(-CLINSVEL);
		// Get the UKF state estimate
		vnl_vector<double> x_est;
		m_UKF.getCurrentStateEstimate(x_est);
		// Apply controller to the state estimate to get input vector u
		steeringComplete = applyController(x_est, m_t);
		// Execute control actions based on input vector u
		executeControl(m_u);


		if(!m_file) {
			QDateTime tm = QDateTime::currentDateTime();
			char fname[100] = "";
			sprintf(fname, "C:/%s.log", tm.toString("mm"));
			m_file = fopen(fname, "w");
		}

		//output is [z xest u]
		vnl_vector < f64 > xest = getUKFEstimate();
		fprintf(m_file, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", z[0], z[1], z[2], z[3], z[4], z[5],
			xest[0], xest[1], xest[2], xest[3], xest[4], xest[5], m_u[0], m_u[1], m_u[2]);
		fflush(m_file);
	}
	// Increment the count of scan steps
	m_stepNumber = m_stepNumber + 1;
	// Return indicator for whether steering is complete
	return steeringComplete;
}

bool RobotControl::applyController(vnl_vector<double> x_est, vnl_vector<double> t)
{
	// define x,y,z axes as vectors
	vnl_matrix<double> I(3,3); I.set_identity();
	vnl_vector<double> z_axis = I.get_column(2);

	// isolate the position and orientation components of the input vector
	vnl_vector<double> p = x_est.extract(3,0);
	vnl_vector<double> r = x_est.extract(3,3);
	// change rotation vector representation to quaternion
	vnl_vector<double> r_norm = r;
	r_norm.normalize();
	vnl_quaternion<double> q(r_norm,-r.magnitude());
	// describe the target point, t, relative to the current tip frame
	vnl_vector<double> t_rel = q.rotate(t-p);
	// project the relative vector onto the x-y plane of the tip frame
	vnl_vector<double> v = t_rel.extract(2,0);
	v = v.normalize();
	// solve for the necessary change in rotation angle
	double d_th = atan2(-v.get(0),v.get(1));
	// describe the target in the rotated tip frame
	vnl_quaternion<double> q_z_dth(z_axis,-d_th);
	vnl_vector<double> t_rel2 = q_z_dth.rotate(t_rel);
	// solve for the radius of the relative path
	double two_norm = t_rel2.two_norm();
	double ro = pow(two_norm,2.0)/2.0/t_rel2.get(1);
	// solve for the distance remaining to the target
	double l = two_norm;
	// format everything we have solved for into the command vector
	m_u.put(0,d_th);
	m_u.put(1,ro);
	m_u.put(2, l);
	// Check completion: if the relative z-component is positive, keep steering
	return (t_rel[2] <= 0.0);
}

void RobotControl::executeControl(vnl_vector<double> u)
{
		// Seperate the first individual commands 
		double d_th = u.get(0), ro = u.get(1), l = u.get(2);
		// Insert the robot towards the target
		// negative d_th b/c tip frame and robot have opposite signs
		double ang_command = fmod(m_robot.getRollAngle(),360.0) - (180.0*d_th/PI); 
		// Map the desired radius into duty cycle
		double DCcommand;
		mapRadiusToDC( ro, DCcommand);
		m_robot.DutyCycleSteer(DCcommand,ang_command,l);
}

void RobotControl::mapRadiusToDC( double RadiusMM, double &DC)
{
	// maps the desired radius of curvature for the needle to a duty cycle percentage
	DC = 1.0 - (NEEDLE_MIN_R/RadiusMM);

	if(DC > 1.0)
		DC = 1.0;
	else if(DC < 0.0)
		DC = 0.0;
}

void RobotControl::setVibration(bool OnOff)
{
	// Activates or deactivates vibration coil using digital output of roll controller
	m_robot.setVibration(OnOff);
}

vnl_vector < double > RobotControl::getUKFEstimate()
{
	vnl_vector < double > res;
	m_UKF.getCurrentStateEstimate(res);
	return res;
}

void RobotControl::InsertIncremental(double inc)
{
	m_robot.InsertIncremental(inc);
}

void RobotControl::SetInsertionVelocity(double v)
{
	m_robot.SetInsertionVelocity(v);
}

void RobotControl::SetRotationVelocity(double v)
{
	m_robot.SetRotationVelocity(v);
}

void RobotControl::RotateIncremental(double inc)
{
	m_robot.RotateIncremental(inc);
}

void RobotControl::homeRoll(void)
{
	m_robot.HomeRoll();
}

void RobotControl::homeInsertion(void)
{
	m_robot.HomeInsertion();
}

void RobotControl::setTargetX(int X)
{
	m_t[0] = X; 
}

void RobotControl::setTargetY(int Y)
{
	m_t[1] = Y; 
}

void RobotControl::setTargetZ(int Z)
{
	m_t[2] = Z;
}

void RobotControl::resetSystem()
{
	// Set number of scans to one
	m_stepNumber = 1;
	// Reset prior for X in the Kalman filter
	m_UKF.resetEstimate();
	// Record the insertion stage position
	m_lastInsPos = m_robot.getInsMM();
}

void RobotControl::updateU()
{
	// Get the current insertion position
	double currentInsPos = m_robot.getInsMM();
	// Find the difference between the current and previous position
	m_u[2] = currentInsPos - m_lastInsPos;
	// Update the saved previous position
	m_lastInsPos = currentInsPos;
}
