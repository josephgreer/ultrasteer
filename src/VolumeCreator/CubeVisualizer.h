#pragma once
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>

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

  class SphereVisualizer : public GeometryVisualizer
  {
  protected:
    vtkSmartPointer < vtkSphereSource > m_sphereSource;

  public:
    SphereVisualizer(const Vec3d &cen, f64 radius);
    void SetCenter(const Vec3d &cen);
    void SetRadius(f64 rad);
  };

  class SphereContainer
  {
  protected:
    std::tr1::shared_ptr < SphereVisualizer > m_sphereVis;
  public:
    SphereContainer();
    virtual void CreateSphere(const Vec3d &cen, f64 rad);
    virtual void SetSphereCenter(const Vec3d &cen);
    virtual void SetSphereRadius(f64 rad);
  };
};