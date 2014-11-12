#pragma once

#include <QDebug>
#include <time.h>
#include "RollDevice.h"
#include "InsertionDevice.h"
#include "qtimer.h"
#include "time.h"

class NeedleSteeringRobot : public QObject 
{
    Q_OBJECT

public:
	NeedleSteeringRobot();
	~NeedleSteeringRobot();
	void processSweep();

	void InitializeRoll(QString Com);
	bool InitializeInsertion(QString Com);
	void InsertIncremental(float MM);
	void RotateIncremental(float Deg);
	void SetInsertionVelocity(float RPM);
	void SetRotationVelocity(float RPM);
	void DisableAll(void);
	void DutyCycleSteer(float DC, float angle, float insDist);
	void HomeRoll(void);
	void HomeInsertion(void);
	void setVibration(bool);
	double getRollAngle(void);
	double getInsMM(void);
	void cancelDutyCycling(void);


private:
	RollDevice m_RollDevice;
	InsertionDevice m_InsertionDevice;
	QTimer polling_timer, dwell_timer;
	clock_t m_roll_start, m_roll_stop;
	double m_DC_InsTarg, m_DC_RollTarg;
	double m_DC;

private slots:
	void OnDwellTimeout(void);
	void OnPollTimeout(void);
};

