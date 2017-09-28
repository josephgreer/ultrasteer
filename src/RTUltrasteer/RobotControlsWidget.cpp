#include "RobotControlsWidget.h"

namespace Nf
{
  RobotControlsWidget::RobotControlsWidget(NeedleSteeringRobot* robot, QWidget *parent, Qt::WFlags flags)
    : QFrame(parent, flags)
    , m_dcInsertVal(1)
    , m_dcInsertAngle(0)
    , m_dcInsertDist(100)
  {
    // Set up the gui
    ui.setupUi(this);

    // Initialize timer flag
    m_displayTimerOn = false;

    // Disable gui elements until the robot has been initialized
    ui.rotatePosInc->setDisabled(true);
    ui.rotateNegInc->setDisabled(true);

    ui.rotatePosIncSmall->setDisabled(true); //gg
    ui.rotateNegIncSmall->setDisabled(true); //gg

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

    connect(ui.rotatePosIncSmall, SIGNAL(clicked(bool)), this, SLOT(RotPosIncSmall())); //gg
    connect(ui.rotateNegIncSmall, SIGNAL(clicked(bool)), this, SLOT(RotNegIncSmall())); //gg

    connect(ui.stopInsertion, SIGNAL(clicked(bool)), this, SLOT(StopInsertion()));
    connect(ui.homeInsertion, SIGNAL(clicked(bool)), this, SLOT(HomeInsertion()));
    connect(ui.insertPosInc, SIGNAL(clicked(bool)), this, SLOT(InsertPosInc()));
    connect(ui.insertNegInc, SIGNAL(clicked(bool)), this, SLOT(InsertNegInc()));
    connect(ui.insertPosVel, SIGNAL(clicked(bool)), this, SLOT(InsertPosVel()));
    connect(ui.insertNegVel, SIGNAL(clicked(bool)), this, SLOT(InsertNegVel()));
    connect(ui.threeSixNeg, SIGNAL(clicked(bool)), this, SLOT(FullNegRot()));
    connect(ui.threeSixPos, SIGNAL(clicked(bool)), this, SLOT(FullPosRot()));
    connect(ui.cancelDCIns, SIGNAL(clicked(bool)), this, SLOT(CancelDCInsert()));

    connect(ui.dcInsert, SIGNAL(clicked(bool)), this, SLOT(DCInsert()));



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

    ui.rotatePosIncSmall->setEnabled(true);   //gg
    ui.rotateNegIncSmall->setEnabled(true);   //gg

    ui.stopRoll->setEnabled(true);
    ui.homeRoll->setEnabled(true);
    ui.threeSixNeg->setEnabled(true);
    ui.threeSixPos->setEnabled(true);
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
    ui.dcInsert->setEnabled(true);
    ui.cancelDCIns->setEnabled(true);
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
      float pos = m_robot->getInsMM2();
      ui.insertion_pos->display(pos);
    }
    if(m_rollInit) {
      ui.roll_pos->setEnabled(true);
      float angle = m_robot->getRollAngle2();
      ui.roll_pos->display(fmodf(angle,360));
    }
    if(m_artInit) {
      ui.articulation_pos->setEnabled(true);
      float angle = m_robot->getArticulationAngle2();
      ui.articulation_pos->display(fmodf(angle,360));
    }
  }

  void RobotControlsWidget::DCInsert()
  {
    //m_robot->DutyCycleSteer(this->m_dcInsertVal, m_robot->getRollAngle2(), this->m_dcInsertDist);
    m_robot->RotateIncremental(2);
  }

  void RobotControlsWidget::CancelDCInsert()
  {
    //m_robot->cancelDutyCycling();
    m_robot->RotateIncremental(-2);

  }

  double RobotControlsWidget::GetInsertion()
  {
    return ui.insertion_pos->value();
  }

  double RobotControlsWidget::GetRoll()
  {
    return ui.roll_pos->value();
  }

  double RobotControlsWidget::GetArticulationAngle()
  {
    return ui.articulation_pos->value();
  }

  void RobotControlsWidget::InsertPosInc(void)
  {
    m_robot->InsertIncremental(17);
  }

  void RobotControlsWidget::InsertNegInc(void)
  {
    m_robot->InsertIncremental(-17);
  }

  void RobotControlsWidget::InsertPosVel(void)
  {
   // m_robot->SetInsertionVelocity(0.5);
     m_robot->SetInsertionVelocity(1);
  }

  void RobotControlsWidget::InsertPosVel(double vel)
  {
    m_robot->SetInsertionVelocity((float)vel);
  }

  void RobotControlsWidget::InsertNegVel(void)
  {
   // m_robot->SetInsertionVelocity(-0.5);
    m_robot->SetInsertionVelocity(-1);
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
    m_robot->RotateIncremental(90);
    //m_robot->RotateIncremental(2);
    //m_robot->SetRotationVelocity(100);
  }

  void RobotControlsWidget::RotNegInc(void)
  {
    m_robot->RotateIncremental(-90);
    //m_robot->RotateIncremental(-2);
   // m_robot->SetRotationVelocity(-100);
  }

  void RobotControlsWidget::FullPosRot(void)
  {
    //m_robot->RotateIncremental(360.0);
    //m_robot->SetRotationVelocity(300);
   m_robot->InsertIncremental(-14);
   Sleep(3000);
   m_robot->HomeRoll();
   //Sleep(5500);

   /*
   m_robot->RotateIncremental(20);
   Sleep(1500);
   m_robot->RotateIncremental(-20);
   Sleep(1500);
   m_robot->RotateIncremental(-20);
   Sleep(1500);
   m_robot->RotateIncremental(20);
   */
    
   
  }

  void RobotControlsWidget::FullNegRot(void)
  {
    //m_robot->RotateIncremental(-360.0);
     m_robot->SetRotationVelocity(300);
   
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

  //giada widget additions
  
    void RobotControlsWidget::RotPosIncSmall(void)
  {
    //m_robot->RotateIncremental(360.0);
    m_robot->SetRotationVelocity(250);
    m_robot->InsertIncremental(26);
    //Sleep(15000);
    //m_robot->InsertIncremental(-10);
   // m_robot->HomeRoll();
    //m_robot->RotateIncremental(3);
  }

  void RobotControlsWidget::RotNegIncSmall(void)
  {
    m_robot->InsertIncremental(-30);
      Sleep(4000);
    m_robot->RotateIncremental(-30);
     Sleep(3000);
    m_robot->RotateIncremental(30);
    Sleep(3000);
    m_robot->RotateIncremental(30);
    Sleep(3000);
    m_robot->RotateIncremental(-30);
    Sleep(4000);
    m_robot->RotateIncremental(-90);
    //m_robot->SetRotationVelocity(300);
   // m_robot->SetInsertionVelocity(1);
  }
  
}

