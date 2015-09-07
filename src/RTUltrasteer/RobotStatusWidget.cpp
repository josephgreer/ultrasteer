#include "RobotStatusWidget.h"

namespace Nf
{
  RobotStatusWidget::RobotStatusWidget(QWidget *parent, NeedleSteeringRobot* robot, Qt::WFlags flags)
    : ResizableQFrame(parent, QSize(VIS_WIDTH, VIS_HEIGHT))
  {
    ui.setupUi(this);
    m_robot = robot;
    connect(&m_DisplayQueryTimer, SIGNAL(timeout()), this, SLOT(onDisplayQuery()));
    m_DisplayQueryTimer.start(100);
  }

  RobotStatusWidget::~RobotStatusWidget()
  {
  }

  void RobotStatusWidget::onDisplayQuery(void)
  {

    if( m_robot->isRollInitialized() )
    {
      ui.rotDial->setEnabled(true);
      ui.rotLCD->setEnabled(true);
      double th = m_robot->getRollAngle();
      ui.rotLCD->display(th);
      ui.rotDial->setSliderPosition(int(th)-90); // adjust zero so dial mimics actual orientation
    }else{
      ui.rotDial->setEnabled(false);
      ui.rotLCD->setEnabled(false);
    }

    if( m_robot->isInsertionInitialized() )
    {
      ui.insSlider->setEnabled(true);
      ui.insLCD->setEnabled(true);
      double l = m_robot->getInsMM();
      ui.insLCD->display(l);
      ui.insSlider->setSliderPosition(int(l));
    }else{
      ui.insSlider->setEnabled(false);
      ui.insLCD->setEnabled(false);
    }
  }

}