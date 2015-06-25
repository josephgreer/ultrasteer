#pragma once
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>

#include "RTCore.h"

namespace Nf {

  class GeometryVisualizer
  {
  protected:
    vtkSmartPointer<vtkPolyDataMapper> m_mapper;
    vtkSmartPointer<vtkActor> m_actor;

  public:
    virtual vtkSmartPointer<vtkActor> GetActor();
    virtual vtkSmartPointer<vtkPolyDataMapper> GetMapper();
  };

  class CubeVisualizer : public GeometryVisualizer
  {
  protected:
    Cubed m_cube;

  public:
    CubeVisualizer(const Nf::Cubed &cube, const u8 color[3]);
  };

  class AxesVisualizer : public GeometryVisualizer
  {
  protected:
    Cubed m_cube;

  public:
    AxesVisualizer(const Nf::Cubed &cube);
  };
};