#ifndef ROBOTCONTROLSWIDGET_H
#define ROBOTCONTROLSWIDGET_H

#include <QFrame>
#include "ui_RobotControlsWidget.h"
#include "UICore.h"
#include "NeedleSteeringRobot.h"

namespace Nf
{
  class RobotControlsWidget : public QFrame
  {
    Q_OBJECT

  public:
    RobotControlsWidget(NeedleSteeringRobot* robot, QWidget *parent = 0, Qt::WFlags flags = 0);
    ~RobotControlsWidget();
    void enableRoll(bool);
    void enableInsertion(bool);
    void enableArticulation(bool);
    void setRobot(NeedleSteeringRobot*);

  private:
    NeedleSteeringRobot *m_robot;
    bool m_displayTimerOn;
    bool m_rollInit;
    bool m_insInit;
    bool m_artInit;
    QTimer m_DisplayQueryTimer;

    Ui::RobotControlsWidgetClass ui;

    private slots:
      void RotPosInc(void);
      void RotNegInc(void);
      void StopRoll(void);
      void HomeRoll(void);

      void InsertPosInc(void);
      void InsertNegInc(void);
      void InsertPosVel(void);
      void InsertNegVel(void);
      void StopInsertion(void);
      void HomeInsertion(void);

      void SetArticulationAngle(int);

      void onDisplayQuery(void);
  };

}

#endif // ROBOTCONTROLSWIDGET_H