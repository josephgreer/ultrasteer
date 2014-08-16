#include "USVisualizerWidget.h"
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkImageImport.h>
#include "VolumeCreator.h"


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

  Nf::RPFullVolumeCreator rpvc;
  rpvc.Initialize("V:/NeedleScan/Feb13_LiverScan/Scan 1/scan", cal, Vec2d(83/1000.0, 83/1000.0), VOL_MIDDLE, 
    Vec3d(60.0, 60.0, 60.0), Vec3d(83.0*2, 83.0*2, 83.0*2), 0.5);

  vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();
  importer->SetDataSpacing(1, 1, 1);
  importer->SetDataOrigin(0, 0, 0);
  importer->SetWholeExtent(0, width-1, 0, height-1, 0, 0);
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetNumberOfScalarComponents(1);
  importer->SetImportVoidPointer(cImage);
  importer->Update();



  vtkSmartPointer<vtkRenderer> renderer = 
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(sphereActor);
  renderer->SetBackground(0.0, 0.0, 0.0);
  this->GetRenderWindow()->AddRenderer(renderer);

#endif
}