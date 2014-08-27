#pragma once

#include "QVTKWidget.h"

class USVisualizerWidget : public QVTKWidget
{
    Q_OBJECT
public: 
  USVisualizerWidget();
  void Initialize();

  virtual QSize sizeHint() const;
};