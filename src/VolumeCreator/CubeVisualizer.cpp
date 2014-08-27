#include "CubeVisualizer.h"

#include <vtkPoints.h>
#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPolyData.h>

namespace Nf {
  ////////////////////////////////////////////////////////
  //CubeVisualizer Class
  ////////////////////////////////////////////////////////
  CubeVisualizer::CubeVisualizer(const Nf::Cubed &cube)
  {
    m_cube = cube;
    vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();

    //front face
    pts->InsertNextPoint(cube.m_ul.x, cube.m_ul.y, cube.m_ul.z);  //0
    pts->InsertNextPoint(cube.m_ul.x, cube.m_br.y, cube.m_ul.z);  //1
    pts->InsertNextPoint(cube.m_br.x, cube.m_br.y, cube.m_ul.z);  //2
    pts->InsertNextPoint(cube.m_br.x, cube.m_ul.y, cube.m_ul.z);  //3

    //back face
    pts->InsertNextPoint(cube.m_ul.x, cube.m_ul.y, cube.m_br.z);  //4
    pts->InsertNextPoint(cube.m_ul.x, cube.m_br.y, cube.m_br.z);  //5
    pts->InsertNextPoint(cube.m_br.x, cube.m_br.y, cube.m_br.z);  //6
    pts->InsertNextPoint(cube.m_br.x, cube.m_ul.y, cube.m_br.z);  //7

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

    unsigned char red[3] = {27, 161, 226};
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");

    for(s32 i=0; i<12; i++)
      colors->InsertNextTupleValue(red);

    linesPolyData->GetCellData()->SetScalars(colors);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(linesPolyData);

    m_mapper = mapper;

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(m_mapper);

    m_actor = actor;
  }

  vtkSmartPointer<vtkActor> CubeVisualizer::GetActor()
  {
    return m_actor;
  }

  vtkSmartPointer<vtkPolyDataMapper> CubeVisualizer::GetMapper()
  {
    return m_mapper;
  }

  ////////////////////////////////////////////////////////
  //End CubeVisualizer Class
  ///////////////////////////////////////////////////////
};

