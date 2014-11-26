#pragma once

#include <QGridLayout>
#include "UICore.h"
#include "Resizable.h"

namespace Nf
{
  class RobotHardwareWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < BoolParameter > m_init;
    QGridLayout *m_layout;

  public:
    RobotHardwareWidget(QWidget *parent);
    ~RobotHardwareWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();

    std::vector < QVTKWidget * > GetChildWidgets();
  };
}
