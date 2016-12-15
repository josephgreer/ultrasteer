#include "PorcineCadaverWidget.h"
#include <QtGui>

namespace Nf
{
  PorcineCadaverWidget::PorcineCadaverWidget(QWidget *parent)
    : Nf::ParameterCollection("Robot Hardware Widget")
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
    , m_saveDataWidget(new SaveDataWidget(parent))
    , m_forceSensor(new cForceSensor())
  {
    m_layout = new QGridLayout(parent);
    this->setLayout(m_layout);

    m_rcwidget = new RobotControlsWidget(m_robot);
    m_layout->addWidget(m_rcwidget, 0, 0, 1, 2);
    m_layout->addWidget(m_saveDataWidget.get(), 0, 2, 1, 1);

    ADD_ACTION_PARAMETER(m_initRoll, "Initialize Roll Motor", CALLBACK_POINTER(onInitRoll, PorcineCadaverWidget), this, true); 
    ADD_ACTION_PARAMETER(m_initInsertion, "Initialize Insertion Motor", CALLBACK_POINTER(onInitInsertion, PorcineCadaverWidget), this, true); 
    ADD_ACTION_PARAMETER(m_initArticulation, "Initialize Articulation Motor", CALLBACK_POINTER(onInitArticulation, PorcineCadaverWidget), this, true); 

    ADD_CHILD_COLLECTION(m_saveDataWidget.get())
 
    ADD_INT_PARAMETER(m_rollCom, "Roll Port", NULL, this, 7, 1, 9, 1);   
    ADD_INT_PARAMETER(m_insertionCom, "Insertion Port", NULL, this, 6, 1, 9, 1);
    ADD_INT_PARAMETER(m_articulationCom, "Articulation Port", NULL, this, 8, 1, 9, 1);
    ADD_FLOAT_PARAMETER(m_dcInsertDist, "DC Insert Dist", CALLBACK_POINTER(onSetDCInsertDist, PorcineCadaverWidget), this, 100, 20, 150, 1.0);
  }

  PorcineCadaverWidget::~PorcineCadaverWidget()
  {
  }

  void PorcineCadaverWidget::onInitRoll()
  {

    QString port;
    QTextStream(&port) << "COM" << m_rollCom->GetValue();
    m_robot->InitializeRoll(port);
    m_rcwidget->enableRoll(true);

  }
  
  void PorcineCadaverWidget::onInitInsertion()
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

  void PorcineCadaverWidget::onInitArticulation()
  {

    QString port;
    QTextStream(&port) << "COM" << m_articulationCom->GetValue();
    m_robot->InitializeArticulation(port);
    m_rcwidget->enableArticulation(true);

  }

  void PorcineCadaverWidget::onSetDCInsertDist()
  {
    m_rcwidget->SetDcInsertDist(this->m_dcInsertDist->GetValue());
  }

  void PorcineCadaverWidget::setRobot(NeedleSteeringRobot* robot)
  {
    m_robot = robot;
    m_rcwidget->setRobot(robot);
  }

  void PorcineCadaverWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void PorcineCadaverWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
  }

  std::vector < QVTKWidget * > PorcineCadaverWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    return res;
  }
}