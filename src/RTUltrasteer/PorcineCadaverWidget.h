#ifndef PorcineCadaverWidget_H
#define PorcineCadaverWidget_H

#include <QGridLayout>
#include "UICore.h"
#include "Resizable.h"
#include "NeedleSteeringRobot.h"
#include "RobotHardwareWidget.h"
#include "SaveDataWidget.h"
#include "cForceSensor.h"
#include "VineWidget.h"

namespace Nf
{
	class ForceSensorThread : public BasicThread
	{
		Q_OBJECT

	public:
		ForceSensorThread();
		virtual ~ForceSensorThread();

    std::vector < RPData > GetUpdatedData();

	protected:
    QMutex m_dataMutex;
    std::vector < RPData > m_data;
    std::tr1::shared_ptr < cForceSensor > m_forceSensor;

		void execute();
	};

  class PorcineCadaverWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < BoolParameter > m_init;
    QGridLayout *m_layout;
    NeedleSteeringRobot *m_robot;
    std::tr1::shared_ptr < RobotHardwareWidget > m_hwWidget;
    std::tr1::shared_ptr < SaveDataWidget > m_saveDataWidget;
		std::tr1::shared_ptr < ForceSensorThread > m_forceThread;

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