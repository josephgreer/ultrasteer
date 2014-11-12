#ifndef ROBOTCONTROL_H
#define ROBOTCONTROL_H

#include <QtGui/QMainWindow>
#include "ui_robotcontrol.h"
#include "NeedleSteeringRobot.h"

class RobotControl : public QMainWindow
{
	Q_OBJECT

public:
	RobotControl(QWidget *parent = 0, Qt::WFlags flags = 0);
	~RobotControl();

private:
	Ui::RobotControlClass ui;
	NeedleSteeringRobot m_robot;
	bool m_rollInit, m_insInit;
	double m_DC_angle, m_DC_percent, m_DC_ins_dist;
	QTimer m_insertRollQueryTimer;

private slots:
	void InitializeRoll(void);
	void SetMaxPosVelocity(void);
	void SetMaxNegVelocity(void);
	void StopMotor(void);
	void RotPosInc(void);
	void RotNegInc(void);
	void InitializeInsertion(void);
	void Insert2mm(void);
	void Retract2mm(void);
	void Insert30rpm(void);
	void Retract30rpm(void);
	void StopInsertion(void);
	void ExecuteDutyCycle(void);
	void SetDCPercent(int);
	void SetDCInsDist(int);
	void SetDCAngle(int);
	void homeRoll(void);
	void homeInsertion(void);
	void vibrationOn(void);
	void vibrationOff(void);
	void DitherOn(void);
	void DitherOff(void);
	void onCheckInsertRoll(void);
	
private:
	void EnableDCControls(bool enable);
};

#endif // ROBOTCONTROL_H
