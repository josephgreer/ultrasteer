#include "CubeVisualizer.h"

#include <vtkPoints.h>
#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>

namespace Nf {

  static void InsertVec(vtkSmartPointer<vtkPoints> pts, Vec3d pt)
  {
    pts->InsertNextPoint(pt.x, pt.y, pt.z);
  }

  
  ////////////////////////////////////////////////////////
  //GeometryVisualizer Class
  ////////////////////////////////////////////////////////
  vtkSmartPointer<vtkActor> GeometryVisualizer::GetActor()
  {
    return m_actor;
  }

  vtkSmartPointer<vtkPolyDataMapper> GeometryVisualizer::GetMapper()
  {
    return m_mapper;
  }

  void GeometryVisualizer::SetColor(const Vec3d &color)
  {
    m_actor->GetProperty()->SetColor(color.x, color.y, color.z);
  }
  ////////////////////////////////////////////////////////
  //End GeometryVisualizer Class
  ///////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////
  //CubeVisualizer Class
  ////////////////////////////////////////////////////////
  CubeVisualizer::CubeVisualizer(const Nf::Cubed &cube, const u8 color[3])
  {
    m_cube = cube;
    vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();

    Vec3d x_axis = cube.m_axes.Col(0);
    Vec3d y_axis = cube.m_axes.Col(1);
    Vec3d z_axis = cube.m_axes.Col(2);

    //Front Face
    Vec3d p0 = cube.m_ul;                 InsertVec(pts, p0);
    Vec3d p1 = p0+y_axis;                 InsertVec(pts, p1);     
    Vec3d p2 = p1+x_axis;                 InsertVec(pts, p2);
    Vec3d p3 = p2-y_axis;                 InsertVec(pts, p3);

    //Back Face
    Vec3d p4 = cube.m_ul+z_axis;          InsertVec(pts, p4);
    Vec3d p5 = p4+y_axis;                 InsertVec(pts, p5);
    Vec3d p6 = p5+x_axis;                 InsertVec(pts, p6);
    Vec3d p7 = p6-y_axis;                 InsertVec(pts, p7);

    //lines
    vtkSmartPointer<vtkLine> l01 = vtkSmartPointer<vtkLine>::New();
    vtkSmartPointer<vtkLine> l12 = vtkSmartPointer<vtkLine>::New();
    vtkSmartPointer<vtkLine> l23 = vtkSmartPointer<vtkLine>::New();
    vtkSmartPointer<vtkLine> l30 = vtkSmartPointer<vtkLine>::New();

    vtkSmartPointer<vtkLine> l45 = vtkSmartPointer<vtkLine>::New();
    vtkSmartPointer<vtkLine> l56 = vtkSmartPointer<vtkLine>::New();
    vtkSmartPointer<vtkLine> l67 = vtkSmartPointer<vtkLine>::New();
    vtkSmartPointer<vtkLine> l74 = vtkSmartPointer<vtkLine>::New();

    vtkSmartPointer<vtkLine> l04 = vtkSmartPointer<vtkLine>::New();
    vtkSmartPointer<vtkLine> l15 = vtkSmartPointer<vtkLine>::New();
    vtkSmartPointer<vtkLine> l37 = vtkSmartPointer<vtkLine>::New();
    vtkSmartPointer<vtkLine> l26 = vtkSmartPointer<vtkLine>::New();

    l01->GetPointIds()->SetId(0,0);  l01->GetPointIds()->SetId(1,1);  
    l12->GetPointIds()->SetId(0,1);  l12->GetPointIds()->SetId(1,2);  
    l23->GetPointIds()->SetId(0,2);  l23->GetPointIds()->SetId(1,3);  
    l30->GetPointIds()->SetId(0,3);  l30->GetPointIds()->SetId(1,0);  

    l45->GetPointIds()->SetId(0,4);  l45->GetPointIds()->SetId(1,5);  
    l56->GetPointIds()->SetId(0,5);  l56->GetPointIds()->SetId(1,6);  
    l67->GetPointIds()->SetId(0,6);  l67->GetPointIds()->SetId(1,7);  
    l74->GetPointIds()->SetId(0,7);  l74->GetPointIds()->SetId(1,4);  

    l04->GetPointIds()->SetId(0,0);  l04->GetPointIds()->SetId(1,4);  
    l15->GetPointIds()->SetId(0,1);  l15->GetPointIds()->SetId(1,5);  
    l37->GetPointIds()->SetId(0,3);  l37->GetPointIds()->SetId(1,7);  
    l26->GetPointIds()->SetId(0,2);  l26->GetPointIds()->SetId(1,6);  

    //cellArray
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    lines->InsertNextCell(l01);
    lines->InsertNextCell(l12);
    lines->InsertNextCell(l23);
    lines->InsertNextCell(l30);

    lines->InsertNextCell(l45);
    lines->InsertNextCell(l56);
    lines->InsertNextCell(l67);
    lines->InsertNextCell(l74);

    lines->InsertNextCell(l04);
    lines->InsertNextCell(l15);
    lines->InsertNextCell(l37);
    lines->InsertNextCell(l26);

    vtkSmartPointer<vtkPolyData> linesPolyData = vtkSmartPointer<vtkPolyData>::New();
    linesPolyData->SetPoints(pts);
    linesPolyData->SetLines(lines);

    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");

    for(s32 i=0; i<12; i++)
      colors->InsertNextTupleValue(color);

    linesPolyData->GetCellData()->SetScalars(colors);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(linesPolyData);

    m_mapper = mapper;

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(m_mapper);

    m_actor = actor;
  }
  ////////////////////////////////////////////////////////
  //End CubeVisualizer Class
  ///////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////
  //AxesVisualizer Class
  ////////////////////////////////////////////////////////
  AxesVisualizer::AxesVisualizer(const Nf::Cubed &cube)
  {
    m_cube = cube;
    vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();

    Vec3d x_axis = cube.m_axes.Col(0);
    Vec3d y_axis = cube.m_axes.Col(1);
    Vec3d z_axis = cube.m_axes.Col(2);

    //Front Base Point
    Vec3d p0 = cube.m_ul;                 InsertVec(pts, p0);
    Vec3d p1 = p0+y_axis;                 InsertVec(pts, p1);     
    Vec3d p2 = p1+x_axis;                 InsertVec(pts, p2);
    Vec3d p3 = p2+z_axis;                 InsertVec(pts, p3);

    //lines
    vtkSmartPointer<vtkLine> l01 = vtkSmartPointer<vtkLine>::New();
    vtkSmartPointer<vtkLine> l02 = vtkSmartPointer<vtkLine>::New();
    vtkSmartPointer<vtkLine> l03 = vtkSmartPointer<vtkLine>::New();

    l01->GetPointIds()->SetId(0,0);  l01->GetPointIds()->SetId(1,1);  
    l02->GetPointIds()->SetId(0,0);  l02->GetPointIds()->SetId(1,2);  
    l03->GetPointIds()->SetId(0,2);  l03->GetPointIds()->SetId(1,3);  

    //cellArray
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    lines->InsertNextCell(l01);
    lines->InsertNextCell(l02);
    lines->InsertNextCell(l03);

    vtkSmartPointer<vtkPolyData> linesPolyData = vtkSmartPointer<vtkPolyData>::New();
    linesPolyData->SetPoints(pts);
    linesPolyData->SetLines(lines);

    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");

    u8 clrs[3][3] = {
      {255, 0, 0},
      {0, 255, 0},
      {0, 0, 255}
    };

    for(s32 i=0; i<3; i++)
      colors->InsertNextTupleValue(&clrs[i][0]);

    linesPolyData->GetCellData()->SetScalars(colors);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(linesPolyData);

    m_mapper = mapper;

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(m_mapper);

    m_actor = actor;
  }
  ////////////////////////////////////////////////////////
  //End AxesVisualizer Class
  ///////////////////////////////////////////////////////

  //////////////////////////////////////////////////////
  //Begin SphereVisualizer Class
  //////////////////////////////////////////////////////
  SphereVisualizer::SphereVisualizer(const Vec3d &cen, f64 radius)
  {
    m_sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    m_sphereSource->SetCenter(cen.x, cen.y, cen.z);
    m_sphereSource->SetRadius(radius);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(m_sphereSource->GetOutputPort());

    m_mapper = mapper;

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(m_mapper);

    m_actor = actor;
   
  }

  void SphereVisualizer::SetCenter(const Vec3d &cen)
  {
    m_sphereSource->SetCenter(cen.x, cen.y, cen.z);
  }

  void SphereVisualizer::SetRadius(f64 rad)
  {
    m_sphereSource->SetRadius(rad);
    m_sphereSource->Modified();
    m_sphereSource->Update();
  }
  ////////////////////////////////////////////////////////
  //End SphereVisualizer Class
  ///////////////////////////////////////////////////////

  //////////////////////////////////////////////////////
  //Begin PointCloudVisualizer Class
  //////////////////////////////////////////////////////
  PointCloudVisualizer::PointCloudVisualizer(f64 rad, Vec3d color)
  {
    m_points = vtkSmartPointer<vtkPoints>::New();

    m_sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    m_sphereSource->SetRadius(rad);
    m_sphereSource->Update();
    
    m_polyData = vtkSmartPointer<vtkPolyData>::New();

    m_glyph3D = vtkSmartPointer<vtkGlyph3D>::New();
    m_glyph3D->SetSourceConnection(m_sphereSource->GetOutputPort());
    m_glyph3D->SetInputData(m_polyData);
    m_glyph3D->Update();

    m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_mapper->SetInputConnection(m_glyph3D->GetOutputPort());

    m_actor = vtkSmartPointer<vtkActor>::New();
    m_actor->SetMapper(m_mapper);
    SetColor(color);
  }

  void PointCloudVisualizer::AddPoint(const Vec3d &cen)
  {
    m_points->InsertNextPoint(cen.x, cen.y, cen.z);
    m_polyData->SetPoints(m_points);
    m_polyData->Modified();
  }

  void PointCloudVisualizer::ClearPoints()
  {
    m_points = vtkSmartPointer<vtkPoints>::New();
    m_polyData->SetPoints(m_points);
    m_polyData->Modified();
  }

  void PointCloudVisualizer::SetRadius(f64 rad)
  {
    m_sphereSource->SetRadius(rad);
    m_sphereSource->Modified();
  }
  ////////////////////////////////////////////////////////
  //End PointCloudVisualizer Class
  ///////////////////////////////////////////////////////
  
  ////////////////////////////////////////////////////////
  //Begin SphereContainer Class
  ///////////////////////////////////////////////////////
  SphereContainer::SphereContainer()
    : m_sphereVis(NULL)
  {
  }

  void SphereContainer::CreateSphere(const Vec3d &cen, f64 rad)
  {
    m_sphereVis = std::tr1::shared_ptr < SphereVisualizer > (new SphereVisualizer(cen, rad));
  }

  void SphereContainer::SetSphereCenter(const Vec3d &cen)
  {
    m_sphereVis->SetCenter(cen);
  }

  void SphereContainer::SetSphereRadius(f64 rad)
  {
    m_sphereVis->SetRadius(rad);
  }
  ////////////////////////////////////////////////////////
  //End SphereContainer Class
  ///////////////////////////////////////////////////////
  
  ////////////////////////////////////////////////////////
  //Begin AxisContainer Class
  ///////////////////////////////////////////////////////
  AxisContainer::AxisContainer()
    : m_axis(NULL)
  {
  }

  void AxisContainer::CreateAxis(const Matrix44d &posePos, s32 scale)
  {
    m_axis = vtkSmartPointer<vtkAxesActor>::New();
    m_axis->SetTotalLength(scale, scale, scale);
    m_axis->PokeMatrix(posePos.GetVTKMatrix());
    m_axis->Modified();
  }

  void AxisContainer::UpdateAxis(const Matrix44d &posePos)
  {
    m_axis->PokeMatrix(posePos.GetVTKMatrix());
    m_axis->Modified();
  }
  ////////////////////////////////////////////////////////
  //End AxisContainer Class
  ///////////////////////////////////////////////////////

};

