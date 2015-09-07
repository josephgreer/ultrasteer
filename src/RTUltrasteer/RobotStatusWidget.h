#ifndef ROBOTSTATUSWIDGET_H
#define ROBOTSTATUSWIDGET_H

#include <QFrame>
#include <QtGui/QFileDialog>
#include "ui_RobotStatusWidget.h"
#include "UICore.h"
#include "Resizable.h"
#include "NeedleSteeringRobot.h"

namespace Nf 
{
  class RobotStatusWidget : public ResizableQFrame
  {
    Q_OBJECT

  public:
    
    RobotStatusWidget(QWidget *parent = 0, NeedleSteeringRobot* robot = 0, Qt::WFlags flags = 0);
    ~RobotStatusWidget();

  public slots:
      void onDisplayQuery(void);

  public:
    Ui::RobotStatusWidget ui;

  private:
    QTimer m_DisplayQueryTimer;
    NeedleSteeringRobot *m_robot;
  };
};

#endif // ROBOTSTATUSWIDGET_H
