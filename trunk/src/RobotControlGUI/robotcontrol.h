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
	bool m_rollInit, m_insInit, m_artInit;
	bool m_displayTimerOn;
	double m_DC_angle, m_DC_percent, m_DC_ins_dist;
	QTimer m_DisplayQueryTimer;

private slots:
	void InitializeRoll(void);
	void RotPosInc(void);
	void RotNegInc(void);
	void RotatePosVel(void);
	void RotateNegVel(void);
	void StopRoll(void);
	void HomeRoll(void);

	void InitializeInsertion(void);
	void InsertPosInc(void);
	void InsertNegInc(void);
	void InsertPosVel(void);
	void InsertNegVel(void);
	void StopInsertion(void);
	void HomeInsertion(void);

	void InitializeArticulation(void);
	void SetArticulationAngle(int);

	void onDisplayQuery(void);
};

#endif // ROBOTCONTROL_H
