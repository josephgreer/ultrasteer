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
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMIPFunction.h>
#include "VolumeCreator.h"

using namespace Nf;


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
  Matrix44d cal(14.8449, 0.9477, -0.0018, 0.0, 15.0061, 0.0016, 1.00, 0.0, 0.1638, 0.0166, 0.0052, 0.0, 0.0, 0.0, 0.0, 1.0);
  rpvc.Initialize("V:/NeedleScan/Feb13_LiverScan/Scan 1/scan", cal, Vec2d(83, 83), VOL_MIDDLE, 
    Vec3d(60.0, 60.0, 60.0), Vec3d(83.0/1000.0*2, 83.0/1000.0*2, 83.0/1000.0*2), 0.5);
  rpvc.Start();

  Vec3i dims = rpvc.GetVolumeDims();

  vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();
  importer->SetDataSpacing(83.0*2, 83.0*2, 83.0*2);
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
  this->GetRenderWindow()->AddRenderer(renderer);

#endif
}