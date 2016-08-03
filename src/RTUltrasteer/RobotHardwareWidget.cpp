#include "RobotHardwareWidget.h"
#include <QtGui>

namespace Nf
{
  RobotHardwareWidget::RobotHardwareWidget(QWidget *parent)
    : Nf::ParameterCollection("Robot Hardware Widget")
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
  {
    m_layout = new QGridLayout(parent);
    this->setLayout(m_layout);

    m_rcwidget = new RobotControlsWidget(m_robot);
    m_layout->addWidget(m_rcwidget);

    ADD_ACTION_PARAMETER(m_initRoll, "Initialize Roll Motor", CALLBACK_POINTER(onInitRoll, RobotHardwareWidget), this, true); 
    ADD_ACTION_PARAMETER(m_initInsertion, "Initialize Insertion Motor", CALLBACK_POINTER(onInitInsertion, RobotHardwareWidget), this, true); 
    ADD_ACTION_PARAMETER(m_initArticulation, "Initialize Articulation Motor", CALLBACK_POINTER(onInitArticulation, RobotHardwareWidget), this, true); 
 



    ADD_INT_PARAMETER(m_rollCom, "Roll Port", NULL, this, 7, 1, 9, 1);   
    ADD_INT_PARAMETER(m_insertionCom, "Insertion Port", NULL, this, 6, 1, 9, 1);
    ADD_INT_PARAMETER(m_articulationCom, "Articulation Port", NULL, this, 8, 1, 9, 1);
    ADD_FLOAT_PARAMETER(m_dcInsertDist, "DC Insert Dist", CALLBACK_POINTER(onSetDCInsertDist, RobotHardwareWidget), this, 100, 20, 150, 1.0);
  }

  RobotHardwareWidget::~RobotHardwareWidget()
  {
  }

  void RobotHardwareWidget::onInitRoll()
  {

    QString port;
    QTextStream(&port) << "COM" << m_rollCom->GetValue();
    m_robot->InitializeRoll(port);
    m_rcwidget->enableRoll(true);

  }
  
  void RobotHardwareWidget::onInitInsertion()
  {

    QString port;
    QMessageBox msgBox;
    msgBox.setWindowTitle("HAZARD");
    msgBox.setText("Ensure linear slide is clear before continuing to calibration.");
    QPushButton *continueButton = msgBox.addButton("Calibrate", QMessageBox::ActionRole);
    QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);
    msgBox.exec();
    if (msgBox.clickedButton() == continueButton) 
    {
      QTextStream(&port) << "COM" << m_insertionCom->GetValue();
      m_robot->InitializeInsertion(port);
      m_rcwidget->enableInsertion(true);
    }

  }

  void RobotHardwareWidget::onInitArticulation()
  {

    QString port;
    QTextStream(&port) << "COM" << m_articulationCom->GetValue();
    m_robot->InitializeArticulation(port);
    m_rcwidget->enableArticulation(true);

  }

  void RobotHardwareWidget::onSetDCInsertDist()
  {
    m_rcwidget->SetDcInsertDist(this->m_dcInsertDist->GetValue());
  }

  void RobotHardwareWidget::setRobot(NeedleSteeringRobot* robot)
  {
    m_robot = robot;
    m_rcwidget->setRobot(robot);
  }

  void RobotHardwareWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void RobotHardwareWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
  }

  std::vector < QVTKWidget * > RobotHardwareWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    return res;
  }
}