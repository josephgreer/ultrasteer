#define ROLLPORT 5
#define INSPORT 7
#define ARTPORT 8

#include "robotcontrol.h"
#include <QtGui>

RobotControl::RobotControl(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	// disable all the controls we don't want to be accessible until robot is initialized
	// roll motor controls
	ui.rotatePosInc->setDisabled(true);
	ui.rotateNegInc->setDisabled(true);
	ui.rotatePosVel->setDisabled(true);
	ui.rotateNegVel->setDisabled(true);
	ui.stopRoll->setDisabled(true);
	ui.homeRoll->setDisabled(true);
	// insertion motor controls
	ui.insertPosInc->setDisabled(true);
	ui.insertNegInc->setDisabled(true);
	ui.insertPosVel->setDisabled(true);
	ui.insertNegVel->setDisabled(true);
	ui.stopInsertion->setDisabled(true);
	ui.homeInsertion->setDisabled(true);
	// articulation motor controls
	ui.articulationScrollBar->setDisabled(true);
	// set default COM port values
	ui.rollPort->setCurrentIndex(ROLLPORT-1);
	ui.insertionPort->setCurrentIndex(INSPORT-1);
	ui.articulationPort->setCurrentIndex(ARTPORT-1);

	// Initialize member variables
	m_rollInit = false;
	m_insInit = false;
	m_artInit = false;
	m_displayTimerOn = false;
	connect(&m_DisplayQueryTimer, SIGNAL(timeout()), this, SLOT(onDisplayQuery()));
}

RobotControl::~RobotControl()
{
	m_robot.HomeRoll();
	m_robot.HomeArticulation();
	Sleep(2000);
	m_robot.DisableAll();
}

void RobotControl::InitializeRoll(void)
{
	QString port = ui.rollPort->currentText();
	m_robot.InitializeRoll(port);
	ui.rollPort->setDisabled(true);
	ui.InitRoll->setDisabled(true);
	ui.rotatePosInc->setEnabled(true);
	ui.stopRoll->setEnabled(true);
	ui.rotateNegVel->setEnabled(true);
	ui.rotatePosVel->setEnabled(true);
	ui.rotateNegInc->setEnabled(true);
	ui.homeRoll->setEnabled(true);
	m_rollInit = true;
	if(!m_displayTimerOn)  {
		//start insertion query timer for GUI update
		m_DisplayQueryTimer.start(100);
		m_displayTimerOn = true;
	}
	ui.roll_pos->setEnabled(true);
	ui.roll_pos->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);
}

void RobotControl::onDisplayQuery(void)
{
	if(m_insInit) {
		//float pos = m_robot.getInsMM();
    float pos=0;
		//display negative pos because it's more intuitive
		ui.insertion_pos->display(pos);
	}
	if(m_rollInit) {
		ui.roll_pos->setEnabled(true);
		//float angle = m_robot.getRollAngle();
		ui.roll_pos->display(fmodf(0,360));
	}
	if(m_artInit) {
		ui.articulation_pos->setEnabled(true);
		float angle = m_robot.getArticulationAngle();
		ui.articulation_pos->display(fmodf(angle,360));
	}
}

void RobotControl::InitializeInsertion(void)
{
	QString port = ui.insertionPort->currentText();

	// Display a message box to warn that movement will occur
	QMessageBox msgBox;
	msgBox.setWindowTitle("HAZARD");
	msgBox.setText("Ensure linear slide is clear before continuing to calibration.");
	QPushButton *continueButton = msgBox.addButton("Calibrate", QMessageBox::ActionRole);
	QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);
	msgBox.exec();
	if (msgBox.clickedButton() == continueButton) 
	{
		if(m_robot.InitializeInsertion(port))
		{
			ui.InitInsertion->setDisabled(true);
			ui.insertPosInc->setEnabled(true);
			ui.insertNegInc->setEnabled(true);
			ui.insertPosVel->setEnabled(true);
			ui.insertNegVel->setEnabled(true);
			ui.stopInsertion->setEnabled(true);
			ui.homeInsertion->setEnabled(true);

			m_insInit = true;
		}
	}

	m_insInit = true;
	if(!m_displayTimerOn) {
		//start insertion query timer for GUI update
		m_DisplayQueryTimer.start(100);
		m_displayTimerOn = true;
	}
	ui.insertion_pos->setEnabled(true);
	ui.insertionPort->setDisabled(true);
	ui.insertion_pos->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);
}

void RobotControl::InitializeArticulation(void)
{
	QString port = ui.articulationPort->currentText();
	m_robot.InitializeArticulation(port);
	ui.articulationPort->setDisabled(true);
	ui.InitArticulation->setDisabled(true);
	ui.articulationScrollBar->setEnabled(true);
	m_artInit = true;
	if(!m_displayTimerOn)  {
		//start insertion query timer for GUI update
		m_DisplayQueryTimer.start(100);
		m_displayTimerOn = true;
	}
	ui.articulation_pos->setEnabled(true);
	ui.articulation_pos->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);
}

void RobotControl::InsertPosInc(void)
{
	m_robot.InsertIncremental(10.0);
}

void RobotControl::InsertNegInc(void)
{
	m_robot.InsertIncremental(-10.0);
}

void RobotControl::InsertPosVel(void)
{
	m_robot.SetInsertionVelocity(2);
}

void RobotControl::InsertPosVel(double vel)
{
  m_robot.SetInsertionVelocity((float)vel);
}

void RobotControl::InsertNegVel(void)
{
	m_robot.SetInsertionVelocity(-2);
}

void RobotControl::StopInsertion(void)
{
	m_robot.SetInsertionVelocity(0);
}

void RobotControl::RotatePosVel(void)
{
	m_robot.SetRotationVelocity(10000);
}

void RobotControl::RotateNegVel(void)
{
	m_robot.SetRotationVelocity(-10000);
}

void RobotControl::StopRoll(void)
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

void RobotControl::HomeRoll(void)
{
	m_robot.HomeRoll();
}

void RobotControl::HomeInsertion(void)
{
	m_robot.HomeInsertion();
}

void RobotControl::SetArticulationAngle(int angle)
{
	m_robot.SetArticulationAngle(float(angle));
}
