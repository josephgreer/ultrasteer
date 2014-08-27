#pragma once
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>

#include "SegmentCore.h"

namespace Nf {
  class CubeVisualizer 
  {
  protected:
    Cubed m_cube;
    vtkSmartPointer<vtkPolyDataMapper> m_mapper;
    vtkSmartPointer<vtkActor> m_actor;

  public:
    CubeVisualizer(const Cubed &cube);
    vtkSmartPointer<vtkActor> GetActor();
    vtkSmartPointer<vtkPolyDataMapper> GetMapper();
  };
};