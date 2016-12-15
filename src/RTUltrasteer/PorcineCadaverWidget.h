#ifndef PorcineCadaverWidget_H
#define PorcineCadaverWidget_H

#include <QGridLayout>
#include "UICore.h"
#include "Resizable.h"
#include "NeedleSteeringRobot.h"
#include "RobotControlsWidget.h"
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
    RobotControlsWidget *m_rcwidget;
    NeedleSteeringRobot *m_robot;
    std::tr1::shared_ptr < SaveDataWidget > m_saveDataWidget;
    std::tr1::shared_ptr < cForceSensor > m_forceSensor;

  public:
    PorcineCadaverWidget(QWidget *parent);
    ~PorcineCadaverWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();

    std::vector < QVTKWidget * > GetChildWidgets();

    //Parameters
    //rollCom
    std::tr1::shared_ptr < Nf::IntParameter > m_rollCom;
    void onSetRollCom();
    CLASS_CALLBACK(onSetRollCom, PorcineCadaverWidget);

    //insertionCom
    std::tr1::shared_ptr < Nf::IntParameter > m_insertionCom;
    void onSetInsertionCom();
    CLASS_CALLBACK(onSetInsertionCom, PorcineCadaverWidget);

    //articulationCom
    std::tr1::shared_ptr < Nf::IntParameter > m_articulationCom;
    void onSetArticulationCom();
    CLASS_CALLBACK(onSetArticulationCom, PorcineCadaverWidget);

    //initRoll
    std::tr1::shared_ptr < Nf::BoolParameter > m_initRoll;
    void onInitRoll();
    CLASS_CALLBACK(onInitRoll, PorcineCadaverWidget)

    //initInsertion
    std::tr1::shared_ptr < Nf::BoolParameter > m_initInsertion;
    void onInitInsertion();
    CLASS_CALLBACK(onInitInsertion, PorcineCadaverWidget)

    //initArticulation
    std::tr1::shared_ptr < Nf::BoolParameter > m_initArticulation;
    void onInitArticulation();
    CLASS_CALLBACK(onInitArticulation, PorcineCadaverWidget)

    std::tr1::shared_ptr < Nf::FloatParameter > m_dcInsertDist;
    void onSetDCInsertDist();
    CLASS_CALLBACK(onSetDCInsertDist, PorcineCadaverWidget)

    void setRobot(NeedleSteeringRobot* robot);

    RobotControlsWidget *GetRCWidget() { return m_rcwidget; }
  };
}

#endif