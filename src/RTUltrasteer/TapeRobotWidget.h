#ifndef TAPEROBOTWIDGET_H
#define TAPEROBOTWIDGET_H

#include "ui_TapeRobot.h"
#include "UICore.h"
#include "Resizable.h"

namespace Nf
{
  class TapeRobotWidget : public ResizableQWidget
  {
    Q_OBJECT

  public:
    TapeRobotWidget(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~TapeRobotWidget();

  private:

    Ui::TapeRobot ui;
  };

}

#endif // TAPEROBOTWIDGET_H