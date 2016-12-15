#include "TapeRobotWidget.h"

namespace Nf
{
  TapeRobotWidget::TapeRobotWidget(QWidget *parent, Qt::WFlags flags)
    : ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
  {
    // Set up the gui
    ui.setupUi((QWidget *)this);
  }
  
  TapeRobotWidget::~TapeRobotWidget()
  {
  }
}

