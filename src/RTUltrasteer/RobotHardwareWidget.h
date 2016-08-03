#ifndef ROBOTHARDWAREWIDGET_H
#define ROBOTHARDWAREWIDGET_H

#include <QGridLayout>
#include "UICore.h"
#include "Resizable.h"
#include "NeedleSteeringRobot.h"
#include "RobotControlsWidget.h"

namespace Nf
{
  class RobotHardwareWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < BoolParameter > m_init;
    QGridLayout *m_layout;
    RobotControlsWidget *m_rcwidget;
    NeedleSteeringRobot *m_robot;

  public:
    RobotHardwareWidget(QWidget *parent);
    ~RobotHardwareWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();

    std::vector < QVTKWidget * > GetChildWidgets();

    //Parameters
    //rollCom
    std::tr1::shared_ptr < Nf::IntParameter > m_rollCom;
    void onSetRollCom();
    CLASS_CALLBACK(onSetRollCom, RobotHardwareWidget);

    //insertionCom
    std::tr1::shared_ptr < Nf::IntParameter > m_insertionCom;
    void onSetInsertionCom();
    CLASS_CALLBACK(onSetInsertionCom, RobotHardwareWidget);

    //articulationCom
    std::tr1::shared_ptr < Nf::IntParameter > m_articulationCom;
    void onSetArticulationCom();
    CLASS_CALLBACK(onSetArticulationCom, RobotHardwareWidget);

    //initRoll
    std::tr1::shared_ptr < Nf::BoolParameter > m_initRoll;
    void onInitRoll();
    CLASS_CALLBACK(onInitRoll, RobotHardwareWidget)

    //initInsertion
    std::tr1::shared_ptr < Nf::BoolParameter > m_initInsertion;
    void onInitInsertion();
    CLASS_CALLBACK(onInitInsertion, RobotHardwareWidget)

    //initArticulation
    std::tr1::shared_ptr < Nf::BoolParameter > m_initArticulation;
    void onInitArticulation();
    CLASS_CALLBACK(onInitArticulation, RobotHardwareWidget)

    std::tr1::shared_ptr < Nf::FloatParameter > m_dcInsertDist;
    void onSetDCInsertDist();
    CLASS_CALLBACK(onSetDCInsertDist, RobotHardwareWidget)

    void setRobot(NeedleSteeringRobot* robot);

    RobotControlsWidget *GetRCWidget() { return m_rcwidget; }
  };
}

#endif