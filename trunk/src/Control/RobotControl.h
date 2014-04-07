#pragma once
#include "stdafx.h"
#include "NeedleSteeringRobot.h"
#include <QDebug>
#include <time.h>
#include <vnl/vnl_vector.h>
#include "UnscentedKalmanFilter.h"

class Propello;

class RobotControl 
{
public:
    RobotControl(Propello* theParent);
    ~RobotControl();
	void processSweep();
	void InitializeRoll(QString Com);
	void InitializeInsertion(QString Com);
	void setVibration(bool OnOff);
	void InsertIncremental(double inc);
	void SetInsertionVelocity(double v);
	void SetRotationVelocity(double v);
	void RotateIncremental(double inc);
	void homeRoll();
	void homeInsertion();
	void setTargetX(int);
	void setTargetY(int);
	void setTargetZ(int);	
	void callFunctionDebug();
	void resetSystem();
	void beginClosedLoopSteering();
	bool runControlLoop(vnl_vector<double> z);
	
private:
	int determineRadiusAndAngle(std::vector<double> &v_scX, std::vector<double> &v_scY, std::vector<double> &v_scZ, double &R, double &del_theta);
	bool applyController(vnl_vector<double> x, vnl_vector<double> t);	
	void executeControl(vnl_vector<double> u);
	void mapRadiusToDC( double RadiusMM, double &DC);
	void updateU(void);
	
private:
	int m_index;
	int fpV;
	int volumes;
	NeedleSteeringRobot m_robot;
	Propello* ParentWindow;
	vnl_vector<double> m_t;
	UnscentedKalmanFilter m_UKF;
	vnl_vector<double> m_u;
	int m_stepNumber;
	double m_lastInsPos;
};
