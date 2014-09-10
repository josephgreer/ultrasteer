#pragma once

#include "QVTKWidget.h"
#include "UICore.h"
#include "VolumeCreator.h"
#include "CubeVisualizer.h"
#include <vtkAxesActor.h>

class USVisualizerWidget : public QVTKWidget, public Nf::ParameterCollection
{
    Q_OBJECT
public: 
  USVisualizerWidget();
  void Initialize();

  virtual QSize sizeHint() const;

  Nf::RPFullVolumeCreator m_rpvc;
  std::tr1::shared_ptr < Nf::CubeVisualizer > m_extentVis;
  vtkSmartPointer<vtkAxesActor> m_volumeAxes;
  vtkSmartPointer<vtkRenderer> m_renderer;

  //Parameters

  //showVolumeExtent
  std::tr1::shared_ptr < Nf::BoolParameter > m_showVolumeExtent;
  void onShowVolumeExtentChanged();
  CLASS_CALLBACK(onShowVolumeExtentChanged, USVisualizerWidget);

  //showVolumeAxes
  std::tr1::shared_ptr < Nf::BoolParameter > m_showVolumeAxes;
  void onShowVolumeAxesChanged();
  CLASS_CALLBACK(onShowVolumeAxesChanged, USVisualizerWidget);

  //setViewXY
  std::tr1::shared_ptr < Nf::BoolParameter > m_setViewXY;
  void onSetViewXY();
  CLASS_CALLBACK(onSetViewXY, USVisualizerWidget)

  //setViewXZ
  std::tr1::shared_ptr < Nf::BoolParameter > m_setViewXZ;
  void onSetViewXZ();
  CLASS_CALLBACK(onSetViewXZ, USVisualizerWidget);

  //setViewYZ
  std::tr1::shared_ptr < Nf::BoolParameter > m_setViewYZ;
  void onSetViewYZ();
  CLASS_CALLBACK(onSetViewYZ, USVisualizerWidget)
};