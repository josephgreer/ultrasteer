#include "PorcineCadaverWidget.h"
#include <QtGui>

namespace Nf
{
  PorcineCadaverWidget::PorcineCadaverWidget(QWidget *parent)
    : Nf::ParameterCollection("Robot Hardware Widget")
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
    , m_saveDataWidget(new SaveDataWidget(parent))
    , m_hwWidget(new RobotHardwareWidget(this))
    , m_forceThread(new ForceSensorThread())
  {
    m_layout = new QGridLayout(parent);
    this->setLayout(m_layout);

    m_layout->addWidget(m_hwWidget.get(), 0, 0, 1, 1);
    m_layout->addWidget(m_saveDataWidget.get(), 0, 1, 1, 1);
  }

  PorcineCadaverWidget::~PorcineCadaverWidget()
  {
  }

  void PorcineCadaverWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
    m_hwWidget->UpdateSize(QSize(3*sz.width()/4-10, sz.height()));
    m_saveDataWidget->UpdateSize(QSize(sz.width()/4-10, sz.height()));
  }

  std::vector < QVTKWidget * > PorcineCadaverWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    return res;
  }

  void PorcineCadaverWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void PorcineCadaverWidget::setRobot(NeedleSteeringRobot* robot)
  {
    m_hwWidget->setRobot(robot);
  }

  ////////////////////////////////////////////////////////////////////
  /// Begin ForceSensorThread
  ////////////////////////////////////////////////////////////////////
  ForceSensorThread::ForceSensorThread()
    : m_forceSensor(new cForceSensor())
  {
  }

  ForceSensorThread::~ForceSensorThread()
  {
  }

  void ForceSensorThread::execute()
  {
  }


  std::vector < RPData > ForceSensorThread::GetUpdatedData()
  {

    return m_data;
  }


  ////////////////////////////////////////////////////////////////////
  /// End ForceSensorThread
  ////////////////////////////////////////////////////////////////////
}