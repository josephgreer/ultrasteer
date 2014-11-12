#include "robotcontrol.h"
#include <QtGui>

RobotControl::RobotControl(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	// disable all the controls we don't want to be accessible until robot is initialized
	// roll motor controls
	ui.rotatePosInc->setDisabled(true);
	ui.stopMotor->setDisabled(true);
	ui.maxNegVelocity->setDisabled(true);
	ui.maxPosVelocity->setDisabled(true);
	ui.rotateNegInc->setDisabled(true);
	ui.homeRoll->setDisabled(true);
	// insertion motor controls
	ui.retract2mm->setDisabled(true);
	ui.insert2mm->setDisabled(true);
	ui.insert30rpm->setDisabled(true);
	ui.retract30rpm->setDisabled(true);
	ui.stopInsertion->setDisabled(true);
	ui.homeInsertion->setDisabled(true);/*
	ui.ditherOn->setDisabled(true);
	ui.ditherOff->setDisabled(true);*/
	// duty cycle controls
	EnableDCControls(false);
	// Initialize member variables
	m_rollInit = false;
	m_insInit = false;
	m_DC_angle = ui.DC_angle_setting->value();
	m_DC_percent = ui.DC_setting->value();
	m_DC_ins_dist = ui.DC_depth_setting->value();

	connect(&m_insertRollQueryTimer, SIGNAL(timeout()), this, SLOT(onCheckInsertRoll()));
}

RobotControl::~RobotControl()
{
	m_robot.HomeRoll();
	Sleep(2000);
	m_robot.DisableAll();
}

void RobotControl::InitializeRoll(void)
{
	m_robot.InitializeRoll("COM6");
	ui.InitRoll->setDisabled(true);
	ui.rotatePosInc->setEnabled(true);
	ui.stopMotor->setEnabled(true);
	ui.maxNegVelocity->setEnabled(true);
	ui.maxPosVelocity->setEnabled(true);
	ui.rotateNegInc->setEnabled(true);
	ui.homeRoll->setEnabled(true);
	m_rollInit = true;
	if(m_rollInit && m_insInit)
		EnableDCControls(true);
	if(!m_insInit)  {
		//start insertion query timer for GUI update
		m_insertRollQueryTimer.start(100);
	}
	ui.roll_pos->setEnabled(true);
	ui.roll_pos->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);
}

void RobotControl::EnableDCControls(bool enable)
{
	if(enable)
	{
		ui.dutyCycle->setEnabled(true);
		ui.DC_angle_setting->setEnabled(true);
		ui.DC_depth_setting->setEnabled(true);
		ui.DC_setting->setEnabled(true);
	}
	else
	{
		ui.dutyCycle->setDisabled(true);
		ui.DC_angle_setting->setDisabled(true);
		ui.DC_depth_setting->setDisabled(true);
		ui.DC_setting->setDisabled(true);
	}
}


void RobotControl::onCheckInsertRoll(void)
{
	if(m_insInit) {
		float pos = m_robot.getInsertionPos();
		//display negative pos because it's more intuitive
		ui.insertion_pos->display(-pos);
	}
	if(m_rollInit) {
		ui.roll_pos->setEnabled(true);
		float angle = m_robot.getRotationAngle();
		ui.roll_pos->display(fmodf(angle,360));
	}
}

void RobotControl::InitializeInsertion(void)
{
	// Display a message box to warn that movement will occur
	QMessageBox msgBox;
	msgBox.setWindowTitle("HAZARD");
	msgBox.setText("Ensure linear slide is clear before continuing to calibration.");
	QPushButton *continueButton = msgBox.addButton("Calibrate", QMessageBox::ActionRole);
	QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);
	msgBox.exec();
	if (msgBox.clickedButton() == continueButton) 
	{
		if(m_robot.InitializeInsertion("COM7"))
		{
			ui.InitInsertion->setDisabled(true);
			ui.retract2mm->setEnabled(true);
			ui.insert2mm->setEnabled(true);
			ui.insert30rpm->setEnabled(true);
			ui.retract30rpm->setEnabled(true);
			ui.stopInsertion->setEnabled(true);
			ui.homeInsertion->setEnabled(true);
			m_insInit = true;
			ui.ditherOn->setEnabled(true);
			ui.ditherOff->setEnabled(true);
			if(m_rollInit && m_insInit)
				EnableDCControls(true);
		}
	}

	m_insInit = true;
	if(!m_rollInit) {
		//start insertion query timer for GUI update
		m_insertRollQueryTimer.start(100);
	}
	ui.insertion_pos->setEnabled(true);
	ui.insertion_pos->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);
}

void RobotControl::Insert2mm(void)
{
	m_robot.InsertIncremental(-3.0);
}

void RobotControl::Retract2mm(void)
{
	m_robot.InsertIncremental(20.0);
}

void RobotControl::Insert30rpm(void)
{
	m_robot.SetInsertionVelocity(-30);
}

void RobotControl::Retract30rpm(void)
{
	m_robot.SetInsertionVelocity(30);
}

void RobotControl::StopInsertion(void)
{
	m_robot.SetInsertionVelocity(0);
}

void RobotControl::SetMaxPosVelocity(void)
{
	m_robot.SetRotationVelocity(10000);
}

void RobotControl::SetMaxNegVelocity(void)
{
	m_robot.SetRotationVelocity(-10000);
}

void RobotControl::StopMotor(void)
{
	m_robot.SetRotationVelocity(0);
}

void RobotControl::RotPosInc(void)
{
	m_robot.RotateIncremental(10.0);
}

void RobotControl::RotNegInc(void)
{
	m_robot.RotateIncremental(-10.0);
}

void RobotControl::SetDCPercent(int value)
{
	m_DC_percent = double(value);
}

void RobotControl::SetDCInsDist(int value)
{
	m_DC_ins_dist = double(value);
}

void RobotControl::SetDCAngle(int value)
{
	m_DC_angle = double(value);
}

void RobotControl::ExecuteDutyCycle(void)
{
	double DC = m_DC_percent/100.0;
	m_robot.DutyCycleSteer(DC,m_DC_angle,m_DC_ins_dist);
}

void RobotControl::homeRoll(void)
{
	m_robot.HomeRoll();
}

void RobotControl::homeInsertion(void)
{
	m_robot.HomeInsertion();
}

void RobotControl::vibrationOn(void)
{
	m_robot.setVibration(true);
}

void RobotControl::vibrationOff(void)
{
	m_robot.setVibration(false);
}

void RobotControl::DitherOn(void)
{
	m_robot.StartStopDither(true);
}

void RobotControl::DitherOff(void)
{
	m_robot.StartStopDither(false);
}