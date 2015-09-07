#include "NeedleSteeringRobot.h"
#include "math.h"
#include <iostream>
#include <fstream>
#include <time.h>

#define PI 3.14159265
#define InsRPM 10.0
#define ONE_OUTPUT_ROTATION		511 422

NeedleSteeringRobot::NeedleSteeringRobot() :
      m_rollInit(false)
    , m_insInit(false)
    , m_artInit(false)
{
	connect(&polling_timer, SIGNAL(timeout()), this, SLOT(OnPollTimeout()));
	connect(&dwell_timer, SIGNAL(timeout()), this, SLOT(OnDwellTimeout()));
}

NeedleSteeringRobot::~NeedleSteeringRobot()
{

}

void NeedleSteeringRobot::InitializeRoll(QString Com)
{
	m_RollDevice.Init(Com);
	m_RollDevice.EnableDevice();
  m_rollInit = true;
}

bool NeedleSteeringRobot::InitializeInsertion(QString Com)
{
	m_InsertionDevice.Init(Com);
	m_InsertionDevice.EnableDevice();
	// Short pause to allow communication to sync up.
	Sleep(10);
	m_InsertionDevice.HomingSequence();
  m_insInit = true;
	return true;
}

void NeedleSteeringRobot::InitializeArticulation(QString Com)
{
	m_ArticulationDevice.Init(Com);
	m_ArticulationDevice.EnableDevice();
  m_artInit = true;
}

bool NeedleSteeringRobot::isRollInitialized(void)
{
  return m_rollInit;
}

bool NeedleSteeringRobot::isInsertionInitialized(void)
{
  return m_insInit;
}

bool NeedleSteeringRobot::isArticulationInitialized(void)
{
  return m_artInit;
}

void NeedleSteeringRobot::InsertIncremental(float MM)
{
	m_InsertionDevice.ClearExistingData();
	long ins_pos = m_InsertionDevice.GetPosition();
	long diff = m_InsertionDevice.ConvertMMToPosition(MM);
	long new_ins_pos = ins_pos + diff;
	m_InsertionDevice.SetPosition(new_ins_pos,false);
}

void NeedleSteeringRobot::RotateIncremental(float Deg)
{
	m_RollDevice.ClearExistingData();
	long roll_pos = m_RollDevice.GetPosition();
	long diff = m_RollDevice.ConvertAngleToPosition(Deg);
	long new_roll_pos = roll_pos + diff;
	m_RollDevice.SetPosition(new_roll_pos,false);
}

void NeedleSteeringRobot::SetArticulationAngle(float Deg)
{
	m_ArticulationDevice.ClearExistingData();
	
	// Check that the desired position is within allowable limits
	float max,min;
	m_ArticulationDevice.GetAngleLimits(&min,&max);
	if (Deg > max)
		Deg = max;
	if (Deg < min)
		Deg = min;

	int pos = m_ArticulationDevice.ConvertAngleToPosition(Deg);
	m_ArticulationDevice.SetPosition(pos,false);
}

void NeedleSteeringRobot::SetInsertionVelocity(float MMPS)
{
	// get the number of mm for one motor turn
	float MMPR = m_InsertionDevice.GetPitch();
	
	// divide mm per second to get rotations per second
	float RPS = MMPS /MMPR;

	// mulitply by 60 to get rotations per minute
	float RPM = RPS*60.0;
	
	m_InsertionDevice.SetVelocity(RPM);
}

void NeedleSteeringRobot::SetRotationVelocity(float RPM)
{
	m_RollDevice.SetVelocity(RPM);
}

void NeedleSteeringRobot::DisableAll(void)
{
	m_RollDevice.Stop();
	m_InsertionDevice.Stop();
}

void NeedleSteeringRobot::HomeRoll(void)
{
	// Returns rotation to an interval of the home position first
	long current_pos = m_RollDevice.GetPosition();
	long delta = current_pos % m_RollDevice.ConvertAngleToPosition(360.0);
	m_RollDevice.SetPosition(current_pos-delta);
}

void NeedleSteeringRobot::HomeArticulation(void)
{
	// Returns articulation to the zero position
	m_ArticulationDevice.SetPosition(0);
}

void NeedleSteeringRobot::HomeInsertion(void)
{
	// Returns insertion device to the home position
	m_InsertionDevice.SetPosition(0);
}

void NeedleSteeringRobot::setVibration(bool OnOff)
{
	// Activates or deactivates vibration coil using digital output of roll controller
	m_RollDevice.setVibration(OnOff);
}

double NeedleSteeringRobot::getRollAngle(void)
{
	// Returns the current angle of the needle
	return m_RollDevice.GetAngle();
}

double NeedleSteeringRobot::getArticulationAngle(void)
{
	// Returns the current angle of the needle
	return m_ArticulationDevice.GetAngle();
}

double NeedleSteeringRobot::getInsMM(void)
{
	// Returns the current angle of the needle
	return m_InsertionDevice.GetMM();
}

//// brief: 
//void NeedleSteeringRobot::DutyCycleSteer(float DC, float angle, float InsDist)
//{
//	// Save input duty cycle value
//	m_DC = DC;
//	// Save desired insertion position
//	double currentIns = m_InsertionDevice.GetMM();
//	m_DC_InsTarg = m_InsertionDevice.GetMM() - InsDist;
//		
//	// Save desired rotation position
//	double currentRot = m_RollDevice.GetAngle();
//	double currentDelta = long(currentRot) % 360;
//	double currentRotZero = currentRot - currentDelta; 
//	m_DC_RollTarg = currentRotZero + angle;
//		
//	// Move the rotation stage to the desired position
//	m_RollDevice.SetPosition(m_RollDevice.ConvertAngleToPosition(m_DC_RollTarg));	
//	
//	// Start insertion
//	m_InsertionDevice.SetVelocity(-10);
//	
//	// Start timer to monitor roll time
//	m_roll_start = clock();
//	// Set polling timer
//	polling_timer.start(20);
//}
//
//void NeedleSteeringRobot::OnPollTimeout(void)
//{
//	// If insertion has reached desired insertion distance
//	double currentIns = m_InsertionDevice.GetMM();
//	if (    abs( currentIns ) > abs( m_DC_InsTarg )       )
//	{
//		// Stop insertion 
//		m_InsertionDevice.SetVelocity(0.0);
//		// Stop polling timer
//		polling_timer.stop();
//		// Cancel dwell timer
//		if( dwell_timer.isActive() )
//			dwell_timer.stop();
//	}
//	// If roll has reached dwell position 
//	double currentRot = m_RollDevice.GetAngle();
//	if (    abs(currentRot - m_DC_RollTarg) < 0.1     )
//	{
//		// Update desired rotation position
//		double currentRot = m_RollDevice.GetAngle();
//		m_DC_RollTarg = currentRot + 360.0;
//		// Calculate dwell time
//		m_roll_stop = clock();
//		double roll_time = (m_roll_stop - m_roll_start)/double(CLOCKS_PER_SEC);
//		double dwell_time;
//		if( m_DC > 0.0)
//			dwell_time = roll_time*(1.0 - m_DC)/m_DC;
//		else
//			dwell_time = 1000; // Effectively set dwell time to inf
//		// Start dwell timer
//		dwell_timer.setSingleShot(true);
//		dwell_timer.start(dwell_time*1000.0);
//	}
//}
//
//void NeedleSteeringRobot::OnDwellTimeout(void)
//{
//	// Start roll
//	m_RollDevice.SetPosition(m_RollDevice.ConvertAngleToPosition(m_DC_RollTarg));
//	// Start timer to monitor roll time
//	m_roll_start = clock();
//}
//
//void NeedleSteeringRobot::cancelDutyCycling(void)
//{
//	// Cancel DC timers if necessary
//	if( polling_timer.isActive() )
//		polling_timer.stop();
//	if( dwell_timer.isActive() )
//		dwell_timer.stop();
//}