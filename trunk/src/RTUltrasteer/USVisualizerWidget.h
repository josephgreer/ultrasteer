#pragma once

#include "QVTKWidget.h"
#include "UICore.h"

class USVisualizerWidget : public QVTKWidget, public Nf::ParameterCollection
{
    Q_OBJECT
public: 
  USVisualizerWidget();
  void Initialize();

  virtual QSize sizeHint() const;

  //Parameters
  std::tr1::shared_ptr < Nf::BoolParameter > m_showVolumeExtent;
};