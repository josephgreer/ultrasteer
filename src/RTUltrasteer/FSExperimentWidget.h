#include "UICore.h"
#include "EMCalibrationWidget.h"
#pragma once

namespace Nf
{
  class FSExperimentFileWidget : public EMCalibrationFileWidget
  {
    Q_OBJECT 

  public:
    FSExperimentFileWidget(QWidget *parent);
    virtual ~FSExperimentFileWidget();
  };

  class FSExperimentStreamingWidget : public EMCalibrationStreamingWidget
  {
    Q_OBJECT 

  public:
    FSExperimentStreamingWidget(QWidget *parent);
    virtual ~FSExperimentStreamingWidget();
  };

}