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
#include "RPWidget.h"

namespace Nf
{
//#define ULTRASOUND_DRIVEN

#ifndef ULTRASOUND_DRIVEN
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
#endif

  class PorcineCadaverWidget : public ResizableQWidget, public RPFrameHandler, public ParameterCollection
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < BoolParameter > m_init;
    QGridLayout *m_layout;
    NeedleSteeringRobot *m_robot;
    std::tr1::shared_ptr < RobotHardwareWidget > m_hwWidget;
    std::tr1::shared_ptr < SaveDataWidget > m_saveDataWidget;
#ifndef ULTRASOUND_DRIVEN
		std::tr1::shared_ptr < ForceSensorThread > m_forceThread;
    QTimer m_heartbeat;
#else
    std::tr1::shared_ptr < cForceSensor > m_forceSensor;
    RPPushReceiver *m_receiver;
#endif

  public:
    PorcineCadaverWidget(QWidget *parent);
    ~PorcineCadaverWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();

    virtual void HandleFrame(RPData &rp);

    //initForceSensor
    std::tr1::shared_ptr < Nf::BoolParameter > m_initForceSensor;
    void onInitForceSensor();
    CLASS_CALLBACK(onInitForceSensor, PorcineCadaverWidget);

    std::vector < QVTKWidget * > GetChildWidgets();

    void setRobot(NeedleSteeringRobot* robot);

#ifndef ULTRASOUND_DRIVEN
  public slots:
    void heartbeat();

signals:
    void stopForceSensorThread();
#endif
  };
}

#endif