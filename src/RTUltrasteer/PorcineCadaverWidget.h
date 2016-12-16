#ifndef PorcineCadaverWidget_H
#define PorcineCadaverWidget_H

#include <QGridLayout>
#include <qtimer.h>
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
		ForceSensorThread(std::tr1::shared_ptr < SaveDataWidget > saveDataWidget);
		virtual ~ForceSensorThread();
    void SetWidget(std::tr1::shared_ptr < RobotHardwareWidget > hwWidget);

    std::vector < RPData > GetUpdatedData();

	protected:
    QMutex m_dataMutex;
    std::vector < RPData > m_data;
    std::tr1::shared_ptr < cForceSensor > m_forceSensor;
    std::tr1::shared_ptr < SaveDataWidget > m_saveDataWidget;
    std::tr1::shared_ptr < RobotHardwareWidget > m_hwWidget;

    void setup();
		void execute();
    void cleanup();
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
    QTimer m_heartbeat;

  public:
    PorcineCadaverWidget(QWidget *parent);
    ~PorcineCadaverWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();

    //initForceSensor
    std::tr1::shared_ptr < Nf::BoolParameter > m_initForceSensor;
    void onInitForceSensor();
    CLASS_CALLBACK(onInitForceSensor, PorcineCadaverWidget);

    std::vector < QVTKWidget * > GetChildWidgets();

    void setRobot(NeedleSteeringRobot* robot);

  public slots:
    void heartbeat();

signals:
    void stopForceSensorThread();
  };
}

#endif