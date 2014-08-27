#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkImageImport.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMIPFunction.h>

#include "USVisualizerWidget.h"
#include "VolumeCreator.h"

using namespace Nf;

static vtkSmartPointer<vtkPolyDataMapper> createCubeVisualization(const Cubed &_cube)
{
  vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();

  Cubed cube = _cube;
  cube.m_br = cube.m_br-cube.m_ul;
  cube.m_ul = Vec3d(0,0,0);

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

  return mapper;
}

USVisualizerWidget::USVisualizerWidget()
: QVTKWidget()
{
}

QSize USVisualizerWidget::sizeHint() const
{
  return QSize(1050,800);
}

void USVisualizerWidget::Initialize()
{
#if 0
  // Sphere
  vtkSmartPointer<vtkSphereSource> sphereSource = 
    vtkSmartPointer<vtkSphereSource>::New();
  //sphereSource->Update();
  vtkSmartPointer<vtkPolyDataMapper> sphereMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkSmartPointer<vtkActor> sphereActor = 
    vtkSmartPointer<vtkActor>::New();
  sphereActor->SetMapper(sphereMapper);

  vtkSmartPointer<vtkRenderer> renderer = 
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(sphereActor);
  renderer->SetBackground(0.0, 0.0, 0.0);
  this->GetRenderWindow()->AddRenderer(renderer);

#else

  RPFullVolumeCreator rpvc;
  Vec3d spacing(83.0/1000.0*4, 83.0/1000.0*4, 83.0/1000.0*4);
  Matrix44d cal(14.8449, 0.9477, -0.0018, 0.0, 15.0061, 0.0016, 1.00, 0.0, 0.1638, 0.0166, 0.0052, 0.0, 0.0, 0.0, 0.0, 1.0);
  rpvc.Initialize("V:/NeedleScan/Feb13_LiverScan/Scan 1/scan", cal, Vec2d(83, 83), VOL_QUARTER, 
    Vec3d(60.0, 60.0, 60.0), spacing, 0.5);
  rpvc.Start();

  Vec3i dims = rpvc.GetVolumeDims();

  vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();
  importer->SetDataSpacing(spacing.x, spacing.y, spacing.z);
  importer->SetDataOrigin(0, 0, 0);
  importer->SetWholeExtent(0, dims.x-1, 0, dims.y-1, 0, dims.z-1);
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetNumberOfScalarComponents(1);
  importer->SetImportVoidPointer(rpvc.GetVolumeOriginData());
  importer->Update();
  
  vtkSmartPointer<vtkVolumeRayCastMIPFunction> rayCastFunction =
    vtkSmartPointer<vtkVolumeRayCastMIPFunction>::New();

  vtkSmartPointer<vtkVolumeRayCastMapper> volumeMapper =
    vtkSmartPointer<vtkVolumeRayCastMapper>::New();
  volumeMapper->SetInputConnection(importer->GetOutputPort(0));
  volumeMapper->SetVolumeRayCastFunction(rayCastFunction);

  vtkSmartPointer<vtkVolume> volume =
    vtkSmartPointer<vtkVolume>::New();
  volume->SetMapper(volumeMapper);

  vtkSmartPointer<vtkRenderer> renderer = 
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddViewProp(volume);
  renderer->SetBackground(0.0, 0.0, 0.0);

  vtkSmartPointer<vtkActor> cubeActor = vtkSmartPointer<vtkActor>::New();
  cubeActor->SetMapper(createCubeVisualization(rpvc.GetVolumePhysicalExtent()));

  renderer->AddActor(cubeActor);

  this->GetRenderWindow()->AddRenderer(renderer);

#endif
}