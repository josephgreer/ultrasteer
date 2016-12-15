#ifndef PorcineCadaverWidget_H
#define PorcineCadaverWidget_H

#include <QGridLayout>
#include "UICore.h"
#include "Resizable.h"
#include "NeedleSteeringRobot.h"
#include "RobotHardwareWidget.h"
#include "SaveDataWidget.h"
#include "cForceSensor.h"

namespace Nf
{
  class PorcineCadaverWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < BoolParameter > m_init;
    QGridLayout *m_layout;
    NeedleSteeringRobot *m_robot;
    std::tr1::shared_ptr < RobotHardwareWidget > m_hwWidget;
    std::tr1::shared_ptr < SaveDataWidget > m_saveDataWidget;
    std::tr1::shared_ptr < cForceSensor > m_forceSensor;

  public:
    PorcineCadaverWidget(QWidget *parent);
    ~PorcineCadaverWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();

    std::vector < QVTKWidget * > GetChildWidgets();

    void setRobot(NeedleSteeringRobot* robot);
  };
}

#endif