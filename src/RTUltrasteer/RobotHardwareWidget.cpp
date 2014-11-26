#include "RobotHardwareWidget.h"

namespace Nf
{
  RobotHardwareWidget::RobotHardwareWidget(QWidget *parent)
    : Nf::ParameterCollection("Robot Hardware Widget")
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
  {
    m_layout = new QGridLayout(parent);
    this->setLayout(m_layout);

  }

  RobotHardwareWidget::~RobotHardwareWidget()
  {
  }

  void RobotHardwareWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void RobotHardwareWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
  }

  std::vector < QVTKWidget * > RobotHardwareWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    return res;
  }
}