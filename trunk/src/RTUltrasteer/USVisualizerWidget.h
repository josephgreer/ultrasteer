#pragma once

#include "QVTKWidget.h"
#include "UICore.h"
#include "VolumeCreator.h"
#include "CubeVisualizer.h"
#include <vtkAxesActor.h>
#include <vtkVolume.h>

class USVisualizerWidget : public QVTKWidget, public Nf::ParameterCollection
{
    Q_OBJECT
public: 
  USVisualizerWidget();
  void Initialize();

  virtual QSize sizeHint() const;

  // helper function for setViewXY
  void SetUSVisView(s32 axis1, s32 axis2);

  //Volume Data Creator
  Nf::RPFullVolumeCreator m_rpvc;
  vtkSmartPointer<vtkRenderer> m_renderer;

  //Volume
  vtkSmartPointer<vtkVolume> m_volume;

  //Volume Axes
  vtkSmartPointer<vtkAxesActor> m_volumeAxes;
  std::tr1::shared_ptr < Nf::CubeVisualizer > m_extentVis;

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
  CLASS_CALLBACK(onSetViewYZ, USVisualizerWidget);
};