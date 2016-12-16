#include "PorcineCadaverWidget.h"
#include <QtGui>

namespace Nf
{
  PorcineCadaverWidget::PorcineCadaverWidget(QWidget *parent)
    : Nf::ParameterCollection("Robot Hardware Widget")
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
    , m_saveDataWidget(new SaveDataWidget(parent))
    , m_hwWidget(new RobotHardwareWidget(this))
    , m_forceThread(new ForceSensorThread(m_saveDataWidget))
  {
    m_layout = new QGridLayout(parent);
    this->setLayout(m_layout);

    m_layout->addWidget(m_hwWidget.get(), 0, 0, 1, 1);
    m_layout->addWidget(m_saveDataWidget.get(), 0, 1, 1, 1);

    ADD_CHILD_COLLECTION(m_hwWidget.get());
    ADD_BOOL_PARAMETER(m_initForceSensor, "Init Force Sensor", CALLBACK_POINTER(onInitForceSensor, PorcineCadaverWidget), this, false);
    
		connect(this, SIGNAL(stopForceSensorThread()), m_forceThread.get(), SLOT(stopThread()));
    connect(&m_heartbeat, SIGNAL(timeout()), this, SLOT(heartbeat()));
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
    m_forceThread->SetWidget(m_hwWidget);
  }

  void PorcineCadaverWidget::onInitForceSensor()
  {
    if(m_forceThread->isRunning()) {
      emit stopForceSensorThread();
      m_forceThread->wait();
    }

    m_heartbeat.stop();
    m_saveDataWidget->SetEnabled(false);
    if(m_initForceSensor->GetValue()) {
      m_forceThread->start();
      m_heartbeat.start(100);
      m_saveDataWidget->SetEnabled(true);
    }
  }

  void PorcineCadaverWidget::heartbeat()
  {
    std::vector < RPData > data = m_forceThread->GetUpdatedData();

    for(s32 i=0; i<data.size(); i++) {
      m_saveDataWidget->SaveDataFrame(data[i]);
    }
  }

  ////////////////////////////////////////////////////////////////////
  /// Begin ForceSensorThread
  ////////////////////////////////////////////////////////////////////
  ForceSensorThread::ForceSensorThread(std::tr1::shared_ptr < SaveDataWidget > saveDataWidget)
    : m_forceSensor(new cForceSensor())
    , m_saveDataWidget(saveDataWidget)
  {
  }

  ForceSensorThread::~ForceSensorThread()
  {
  }

  void ForceSensorThread::SetWidget(std::tr1::shared_ptr < RobotHardwareWidget > hwWidget)
  {
    m_hwWidget = hwWidget;
  }

  void ForceSensorThread::setup()
  {
    m_forceSensor->Set_Calibration_File_Loc("C:/Joey/ultrasteer/src/Nano17/FT14057.cal"); 
    s32 rv = m_forceSensor->Initialize_Force_Sensor("Dev3/ai0:5");
    if(rv != 0) {
      NTrace("error initializign force sensor\n");
      throw std::runtime_error("Error intializing force sensor");
    }
    msleep(100);
    m_forceSensor->Zero_Force_Sensor();
  }

  void ForceSensorThread::execute()
  {
    RPData rp;
    s32 rv = m_forceSensor->AcquireFTData();
    if(rv != 0)
      throw std::runtime_error("Error acquiring force data");
    f64 force[3] = {0};
    m_forceSensor->GetForceReading(force);
    rp.force.force = Vec3d(force[0], force[1], force[2]);
    f64 torque[3] = {0};
    m_forceSensor->GetTorqueReading(torque);
    rp.force.torque = Vec3d(torque[0], torque[1], torque[2]);

    rp.u.v = m_hwWidget->GetRCWidget()->ui.insertion_pos->value();
    rp.u.dtheta = m_hwWidget->GetRCWidget()->ui.roll_pos->value();
    rp.u.dutyCycle = m_hwWidget->GetRCWidget()->ui.articulation_pos->value();
    rp.u.tick = timeGetTime();

    m_dataMutex.lock();
    m_data.push_back(rp);
    m_dataMutex.unlock();

    msleep(1);
  }

  void ForceSensorThread::cleanup()
  {
    s32 rv = m_forceSensor->Stop_Force_Sensor();
    NTrace("Stopped force sensor %d\n", rv);
  }


  std::vector < RPData > ForceSensorThread::GetUpdatedData()
  {
    m_dataMutex.lock();

    std::vector < RPData > res = m_data;
    m_data.clear();
    
    m_dataMutex.unlock();
    return res;
  }


  ////////////////////////////////////////////////////////////////////
  /// End ForceSensorThread
  ////////////////////////////////////////////////////////////////////
}