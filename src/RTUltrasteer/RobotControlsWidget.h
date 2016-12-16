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

    Ui::RobotControlsWidgetClass ui;

  private:
    NeedleSteeringRobot *m_robot;
    bool m_displayTimerOn;
    bool m_rollInit;
    bool m_insInit;
    bool m_artInit;
    QTimer m_DisplayQueryTimer;

    f64 m_dcInsertAngle;
    f64 m_dcInsertVal;
    f64 m_dcInsertDist;

    public slots:
      void RotPosInc(void);
      void RotNegInc(void);
      void StopRoll(void);
      void HomeRoll(void);

      void InsertPosInc(void);
      void InsertNegInc(void);
      void InsertPosVel(void);
      void InsertPosVel(double vel);
      void InsertNegVel(void);
      void StopInsertion(void);
      void HomeInsertion(void);
      void FullPosRot(void);
      void FullNegRot(void);

      void DCInsert(void);
      void CancelDCInsert(void);

      double GetInsertion(void);
      double GetRoll(void);
      double GetArticulationAngle(void);
      
      bool isRollInitialized(void) { return m_robot->isRollInitialized(); }
      bool isInsertionInitialized(void) { return m_robot->isInsertionInitialized(); }
      bool isArticulationInitialized(void) { return m_robot->isArticulationInitialized(); }

      void SetArticulationAngle(int);

      void onDisplayQuery(void);

  public:
    void SetDcInsertDist(f64 dist) { m_dcInsertDist = dist; }
  };

}

#endif // ROBOTCONTROLSWIDGET_H