#include "RobotControlsWidget.h"

namespace Nf
{
  RobotControlsWidget::RobotControlsWidget(NeedleSteeringRobot* robot, QWidget *parent, Qt::WFlags flags)
    : QFrame(parent, flags)
  {
    // Set up the gui
    ui.setupUi(this);

    // Initialize timer flag
    m_displayTimerOn = false;

    // Disable gui elements until the robot has been initialized
    ui.rotatePosInc->setDisabled(true);
    ui.rotateNegInc->setDisabled(true);
    ui.stopRoll->setDisabled(true);
    ui.homeRoll->setDisabled(true);
    ui.insertPosInc->setDisabled(true);
    ui.insertNegInc->setDisabled(true);
    ui.insertPosVel->setDisabled(true);
    ui.insertNegVel->setDisabled(true);
    ui.stopInsertion->setDisabled(true);
    ui.homeInsertion->setDisabled(true);
    ui.articulationScrollBar->setDisabled(true);

    // Connect signals and slots
    connect(ui.stopRoll, SIGNAL(clicked(bool)), this, SLOT(StopRoll()));
    connect(ui.homeRoll, SIGNAL(clicked(bool)), this, SLOT(HomeRoll()));
    connect(ui.rotatePosInc, SIGNAL(clicked(bool)), this, SLOT(RotPosInc()));
    connect(ui.rotateNegInc, SIGNAL(clicked(bool)), this, SLOT(RotNegInc()));

    connect(ui.stopInsertion, SIGNAL(clicked(bool)), this, SLOT(StopInsertion()));
    connect(ui.homeInsertion, SIGNAL(clicked(bool)), this, SLOT(HomeInsertion()));
    connect(ui.insertPosInc, SIGNAL(clicked(bool)), this, SLOT(InsertPosInc()));
    connect(ui.insertNegInc, SIGNAL(clicked(bool)), this, SLOT(InsertNegInc()));
    connect(ui.insertPosVel, SIGNAL(clicked(bool)), this, SLOT(InsertPosVel()));
    connect(ui.insertNegVel, SIGNAL(clicked(bool)), this, SLOT(InsertNegVel()));

    connect(ui.articulationScrollBar, SIGNAL(valueChanged(int)), this, SLOT(SetArticulationAngle(int)));
    connect(&m_DisplayQueryTimer, SIGNAL(timeout()), this, SLOT(onDisplayQuery()));
  
  }
  
  RobotControlsWidget::~RobotControlsWidget()
  {
  }

  void RobotControlsWidget::enableRoll(bool enabled)
  {
    m_rollInit = enabled;
    ui.rotatePosInc->setEnabled(true);
    ui.rotateNegInc->setEnabled(true);
    ui.stopRoll->setEnabled(true);
    ui.homeRoll->setEnabled(true);
    if(!m_displayTimerOn)  {
      //start insertion query timer for GUI update
      m_DisplayQueryTimer.start(100);
      m_displayTimerOn = true;
    }
    ui.roll_pos->setEnabled(true);
    ui.roll_pos->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);
  }

  void RobotControlsWidget::enableInsertion(bool enabled)
  {
    m_insInit = enabled;
    ui.insertPosInc->setEnabled(true);
    ui.insertNegInc->setEnabled(true);
    ui.insertPosVel->setEnabled(true);
    ui.insertNegVel->setEnabled(true);
    ui.stopInsertion->setEnabled(true);
    ui.homeInsertion->setEnabled(true);
    if(!m_displayTimerOn) {
      //start insertion query timer for GUI update
      m_DisplayQueryTimer.start(100);
      m_displayTimerOn = true;
    }
    ui.insertion_pos->setEnabled(true);
    ui.insertion_pos->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);
  }
  
  void RobotControlsWidget::enableArticulation(bool enabled)
  {
    m_artInit = enabled;
    ui.articulationScrollBar->setEnabled(true);
    if(!m_displayTimerOn)  {
      //start insertion query timer for GUI update
      m_DisplayQueryTimer.start(100);
      m_displayTimerOn = true;
    }
    ui.articulation_pos->setEnabled(true);
    ui.articulation_pos->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);
  }

  void RobotControlsWidget::onDisplayQuery(void)
  {
    if(m_insInit) {
      float pos = m_robot->getInsMM();
      ui.insertion_pos->display(pos);
    }
    if(m_rollInit) {
      ui.roll_pos->setEnabled(true);
      float angle = m_robot->getRollAngle();
      ui.roll_pos->display(fmodf(angle,360));
    }
    if(m_artInit) {
      ui.articulation_pos->setEnabled(true);
      float angle = m_robot->getArticulationAngle();
      ui.articulation_pos->display(fmodf(angle,360));
    }
  }

  void RobotControlsWidget::InsertPosInc(void)
  {
    m_robot->InsertIncremental(10.0);
  }

  void RobotControlsWidget::InsertNegInc(void)
  {
    m_robot->InsertIncremental(-10.0);
  }

  void RobotControlsWidget::InsertPosVel(void)
  {
    m_robot->SetInsertionVelocity(2);
  }

  void RobotControlsWidget::InsertNegVel(void)
  {
    m_robot->SetInsertionVelocity(-2);
  }

  void RobotControlsWidget::StopInsertion(void)
  {
    m_robot->SetInsertionVelocity(0);
  }

  void RobotControlsWidget::StopRoll(void)
  {
    m_robot->SetRotationVelocity(0);
  }

  void RobotControlsWidget::RotPosInc(void)
  {
    m_robot->RotateIncremental(10.0);
  }

  void RobotControlsWidget::RotNegInc(void)
  {
    m_robot->RotateIncremental(-10.0);
  }

  void RobotControlsWidget::HomeRoll(void)
  {
    m_robot->HomeRoll();
  }

  void RobotControlsWidget::HomeInsertion(void)
  {
    m_robot->HomeInsertion();
  }

  void RobotControlsWidget::SetArticulationAngle(int angle)
  {
    m_robot->SetArticulationAngle(float(angle));
  }

  void RobotControlsWidget::setRobot(NeedleSteeringRobot* robot)
  {
    m_robot = robot;
  }

}

