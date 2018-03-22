/// class:		NeedleSteeringRobot
/// brief:		Encapsulates the functionality of the entire needle steering robot, 
///				including all 3DOF.
///
///	author:		Troy Adebar
///	date:		November 13, 2014

#pragma once

#include "RollDevice.h"
#include "InsertionDevice.h"
#include "ArticulationDevice.h"
#include "qtimer.h"
#include <time.h>

class NeedleSteeringRobot : public QObject 
{
    Q_OBJECT

public:
	NeedleSteeringRobot();
	~NeedleSteeringRobot();
	void processSweep();

	void InitializeRoll(QString Com);
	bool InitializeInsertion(QString Com);
	void InitializeArticulation(QString Com);
	void InsertIncremental(float MM);
	void RotateIncremental(float Deg);
	void SetArticulationAngle(float Deg);
	void SetInsertionVelocity(float MMPS);
	void SetRotationVelocity(float RPM);
	void DisableAll(void);
	void HomeRoll(void);
	void HomeInsertion(void);
	void HomeArticulation(void);
	void setVibration(bool);
	double getRollAngle(void);
  double getRollAngle2(void);
	double getInsMM(void);
  double getInsMM2(void);
	double getArticulationAngle(void);
  double getArticulationAngle2(void);
  bool isRollInitialized(void);
  bool isInsertionInitialized(void);
  bool isArticulationInitialized(void);
	void cancelDutyCycling(void);
	void DutyCycleSteer(float DC, float angle, float insDist);
  void setMaxRollVelocity(int);
  void setMaxInsertionVelocity(int);
  void resetMaxVelocities();

private:
	RollDevice m_RollDevice;
	InsertionDevice m_InsertionDevice;
	ArticulationDevice m_ArticulationDevice;
	QTimer polling_timer, dwell_timer;
	clock_t m_roll_start, m_roll_stop;
	double m_DC_InsTarg, m_DC_RollTarg;
	double m_DC;
  double RollAngle;
  double ArticulationAngle;
  double insMM;
  int RollVel;
  int InsVel;
  bool m_rollInit, m_insInit, m_artInit; 

public:
  InsertionDevice *GetInsertionDevice() { return &m_InsertionDevice; }
public slots:
		void OnDwellTimeout(void);
		void OnPollTimeout(void);
};

