#include <vtkSmartPointer.h>
#include <vtkPoints.h>
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

using namespace Nf;

USVisualizerWidget::USVisualizerWidget()
: QVTKWidget()
, Nf::ParameterCollection("Ultrasound Visualization")
{
  ADD_BOOL_PARAMETER(m_showVolumeExtent, "Show Volume Extent", CALLBACK_POINTER(onShowVolumeExtentChanged, USVisualizerWidget), this, true);
}

QSize USVisualizerWidget::sizeHint() const
{
  return QSize(1050,800);
}

void USVisualizerWidget::onShowVolumeExtentChanged()
{
  bool reRender = false;
  if(m_extentVis)
    m_renderer->RemoveActor(m_extentVis->GetActor());
  if(m_showVolumeExtent->GetValue()) {
    m_extentVis = std::tr1::shared_ptr < CubeVisualizer > (new CubeVisualizer(m_rpvc.GetVolumePhysicalExtent()));
    m_renderer->AddActor(m_extentVis->GetActor());
    reRender = true;
  } else if(m_extentVis) {
    reRender = true;
  }

  if(reRender) {
    this->repaint();
  }
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

  //Volume visualization
  Vec3d spacing(83.0/1000.0*4, 83.0/1000.0*4, 83.0/1000.0*4);
  Matrix44d cal(14.8449, 0.9477, -0.0018, 0.0, 15.0061, 0.0016, 1.00, 0.0, 0.1638, 0.0166, 0.0052, 0.0, 0.0, 0.0, 0.0, 1.0);
  m_rpvc.Initialize("V:/NeedleScan/Feb13_LiverScan/Scan 1/scan", cal, Vec2d(83, 83), VOL_QUARTER, 
    Vec3d(60.0, 60.0, 60.0), spacing, 0.5);
  m_rpvc.Start();

  Vec3i dims = m_rpvc.GetVolumeDims();

  vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();
  importer->SetDataSpacing(spacing.x, spacing.y, spacing.z);
  importer->SetDataOrigin(0, 0, 0);
  importer->SetWholeExtent(0, dims.x-1, 0, dims.y-1, 0, dims.z-1);
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetNumberOfScalarComponents(1);
  importer->SetImportVoidPointer(m_rpvc.GetVolumeOriginData());
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

  m_renderer = 
    vtkSmartPointer<vtkRenderer>::New();
  m_renderer->AddViewProp(volume);
  m_renderer->SetBackground(0.0, 0.0, 0.0);

  //Cube Visualization
  if(m_showVolumeExtent->GetValue()) {
    m_extentVis = std::tr1::shared_ptr < CubeVisualizer > (new CubeVisualizer(m_rpvc.GetVolumePhysicalExtent()));
    m_renderer->AddActor(m_extentVis->GetActor());
  };

  this->GetRenderWindow()->AddRenderer(m_renderer);

#endif
}