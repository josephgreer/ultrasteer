#pragma once
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "UICore.h"
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkPoints.h>
#include <vtkAxesActor.h>
#include <vtkGlyph3D.h>
#include <vtkPlaneSource.h>
#include <vtkSTLReader.h>
#include <vtkWindowToImageFilter.h>
#include <vtkAVIWriter.h>
#include <vtkRenderWindow.h>
#include <vtkRegularPolygonSource.h>

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
    virtual void SetColor(const Vec3d &color);
  };

  class CubeVisualizer : public GeometryVisualizer
  {
  protected:
    Cubed m_cube;

  public:
    CubeVisualizer(const Nf::Cubed &cube, const u8 color[3]);
  };

  class PolygonVisualizer : public GeometryVisualizer
  {
    vtkSmartPointer<vtkRegularPolygonSource> m_polygon;

  public:
    PolygonVisualizer(f64 radius, s32 numSides, bool outlineOnly);
    void SetPose(const Matrix44d &pose);
  };

  class PointCloudVisualizer : public GeometryVisualizer
  {
  protected:
    vtkSmartPointer < vtkPoints > m_points;
    vtkSmartPointer < vtkPolyData > m_polyData;
    vtkSmartPointer < vtkSphereSource > m_sphereSource;
    vtkSmartPointer < vtkGlyph3D > m_glyph3D;

  public:
    PointCloudVisualizer(f64 rad, Vec3d color);
    void AddPoint(const Vec3d &point);
    void ClearPoints();
    void SetRadius(f64 rad);
    void SavePoints(const char *path);
    void SetPoints(const std::vector < Vec3d > &points);
    void SetPoints(const std::vector < Vec3d > & points, const std::vector < f64 > & weights);
  };

  class STLVisualizer : public GeometryVisualizer 
  {
  protected:
    vtkSmartPointer < vtkSTLReader > m_stlReader;

  public:
    STLVisualizer(const char *stlPath);
  };

  class CurveVisualizer : public GeometryVisualizer
  {
  public:
    CurveVisualizer();
    void SetCurve(const Polynomial *p, Vec2d range, f64 ds = 1.0);
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

  class PlaneVisualizer : public GeometryVisualizer
  {
  protected:
    vtkSmartPointer < vtkPlaneSource > m_planeSource;

  public:
    PlaneVisualizer(Vec3d corner, Vec3d normal);
    void SetPlane(Vec3d corner, Vec3d axis1, Vec3d axis2);
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

  class AxisContainer
  {
  protected:
    vtkSmartPointer < vtkAxesActor > m_axis;
  public:
    AxisContainer();
    virtual void CreateAxis(const Matrix44d &posePos, s32 scale);
    virtual void UpdateAxis(const Matrix44d &posePos);
  };
 
  class ScreenWriter : public ParameterCollection
  {
  protected:
    vtkSmartPointer < vtkWindowToImageFilter > m_windowFilter;
    vtkSmartPointer < vtkAVIWriter > m_aviWriter;
    vtkSmartPointer < vtkRenderWindow > m_renderWindow;

  public:
    ScreenWriter(vtkSmartPointer < vtkRenderWindow > renderWindow);
    s32 WriteLatest();

    std::tr1::shared_ptr < BoolParameter > m_write;
    void onWrite();
    CLASS_CALLBACK(onWrite, ScreenWriter);

    std::tr1::shared_ptr < FloatParameter > m_fps;

    std::tr1::shared_ptr < FileParameter > m_file;
  };

  class CurveContainer
  {
  protected:
    std::tr1::shared_ptr < CurveVisualizer > m_curve;

  public:
    CurveContainer() { m_curve = std::tr1::shared_ptr < CurveVisualizer > (new CurveVisualizer()); }
    void SetPolynomial(const Polynomial *p, f64 a, f64 b, f64 ds = 1) { m_curve->SetCurve(p, Vec2d(a,b), ds); }
  };
};