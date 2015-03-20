#include "Teleoperation2DWidget.h"

namespace Nf
{
  Teleoperation2DWidget::Teleoperation2DWidget(QWidget *parent)
    : Nf::ParameterCollection("2D Teleoperation Widget")
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
  {
    m_layout = new QGridLayout(parent);
    this->setLayout(m_layout);

  }

  Teleoperation2DWidget::~Teleoperation2DWidget()
  {
  }

  void Teleoperation2DWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void Teleoperation2DWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
  }

  std::vector < QVTKWidget * > Teleoperation2DWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    return res;
  }
}