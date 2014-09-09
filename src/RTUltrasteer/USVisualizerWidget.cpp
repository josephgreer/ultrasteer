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
#include <vtkTransform.h>
#include <vtkProperty.h>

#include "USVisualizerWidget.h"

using namespace Nf;

USVisualizerWidget::USVisualizerWidget()
: QVTKWidget()
, Nf::ParameterCollection("Ultrasound Visualization")
, m_volumeAxes(NULL)
{
  ADD_BOOL_PARAMETER(m_showVolumeExtent, "Show Volume Extent", CALLBACK_POINTER(onShowVolumeExtentChanged, USVisualizerWidget), this, true);
  ADD_BOOL_PARAMETER(m_showVolumeAxes, "Show Volume Axes", CALLBACK_POINTER(onShowVolumeAxesChanged, USVisualizerWidget), this, true);
  ADD_CHILD_COLLECTION(m_rpvc);
}

QSize USVisualizerWidget::sizeHint() const
{
  return QSize(640,480);
}

void USVisualizerWidget::onShowVolumeExtentChanged()
{
  if(m_showVolumeExtent->GetValue() && m_extentVis) {
    m_renderer->AddActor(m_extentVis->GetActor());
  } else if(m_extentVis) {
    m_renderer->RemoveActor(m_extentVis->GetActor());
  }
}

void USVisualizerWidget::onShowVolumeAxesChanged()
{
  if(m_showVolumeAxes->GetValue() && m_volumeAxes != NULL) {
    m_renderer->AddActor(m_volumeAxes);
  } else if(m_volumeAxes) {
    m_renderer->RemoveActor(m_volumeAxes);
  }
}

static void outputMatrix(Matrix44d matrix)
{
  NTrace("[%f %f %f %f; %f %f %f %f; %f %f %f %f; %f %f %f %f]\n", 
    matrix.m_data[0][0], matrix.m_data[0][1], matrix.m_data[0][2], matrix.m_data[0][3],
    matrix.m_data[1][0], matrix.m_data[1][1], matrix.m_data[1][2], matrix.m_data[1][3],
    matrix.m_data[2][0], matrix.m_data[2][1], matrix.m_data[2][2], matrix.m_data[2][3],
    matrix.m_data[3][0], matrix.m_data[3][1], matrix.m_data[3][2], matrix.m_data[3][3]);
}


static void outputMatrix(Matrix33d matrix)
{
  NTrace("[%f %f %f; %f %f %f; %f %f %f; %f %f %f]\n", 
    matrix.m_data[0][0], matrix.m_data[0][1], matrix.m_data[0][2], 
    matrix.m_data[1][0], matrix.m_data[1][1], matrix.m_data[1][2], 
    matrix.m_data[2][0], matrix.m_data[2][1], matrix.m_data[2][2]);
}

static void outputVector(Vec3d vector)
{
  NTrace("[%f; %f; %f]\n", vector.x, vector.y, vector.z);
}

static void outputCube(Cubed cube)
{
  outputVector(cube.m_ul);
  outputMatrix(cube.m_axes);
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

  //Raw volume data
  Vec3d spacing(83.0/1000.0*4, 83.0/1000.0*4, 83.0/1000.0*4);
  Matrix44d cal(14.8449, 0.9477, -0.0018, 0.0, 15.0061, 0.0016, 1.00, 0.0, 0.1638, 0.0166, 0.0052, 0.0, 0.0, 0.0, 0.0, 1.0);
  m_rpvc.Initialize("V:/NeedleScan/Feb13_LiverScan/Scan 1/scan", cal, Vec2d(83, 83), VOL_QUARTER, 
    Vec3d(60.0, 60.0, 60.0), spacing, 0.5);
  m_rpvc.Start();

  Vec3i dims = m_rpvc.GetVolumeDims();
  Cubed volExtent = m_rpvc.GetVolumeCubeExtent();
  Cubed physExtent = m_rpvc.GetVolumePhysicalExtent();

  //vtkImageData importer
  vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();
  importer->SetDataSpacing(spacing.x, spacing.y, spacing.z);
  importer->SetDataOrigin(0,0,0);
  importer->SetWholeExtent(0, dims.x-1, 0, dims.y-1, 0, dims.z-1);
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetNumberOfScalarComponents(1);
  importer->SetImportVoidPointer(m_rpvc.GetVolumeOriginData());
  importer->Update();
  
  //Ray Casting
  vtkSmartPointer<vtkVolumeRayCastMIPFunction> rayCastFunction =
    vtkSmartPointer<vtkVolumeRayCastMIPFunction>::New();

  vtkSmartPointer<vtkVolumeRayCastMapper> volumeMapper =
    vtkSmartPointer<vtkVolumeRayCastMapper>::New();
  volumeMapper->SetInputConnection(importer->GetOutputPort(0));
  volumeMapper->SetVolumeRayCastFunction(rayCastFunction);

  //Volumne
  Matrix33d orientation = m_rpvc.GetVolumeOrientation();
  Matrix44d volTrans = Matrix44d::FromOrientationAndTranslation(m_rpvc.GetVolumeOrientation(), volExtent.m_ul);
  vtkSmartPointer<vtkVolume> volume =
    vtkSmartPointer<vtkVolume>::New();
  volume->SetMapper(volumeMapper);
  volume->PokeMatrix(volTrans.GetVTKMatrix());
  volume->Update();
  f64 *boundsVolume = volume->GetBounds();

  //Volume Renderer
  m_renderer = 
    vtkSmartPointer<vtkRenderer>::New();
  m_renderer->AddViewProp(volume);
  m_renderer->SetBackground(0.0, 0.0, 0.0);

  //Cube Visualization
  u8 cubeColor[3] = {27, 161, 226};
  m_extentVis = std::tr1::shared_ptr < CubeVisualizer > (new CubeVisualizer(volExtent, cubeColor));
  onShowVolumeExtentChanged();

  //Volume Axes Visualization
  m_volumeAxes = vtkSmartPointer<vtkAxesActor>::New();
  m_volumeAxes->SetTotalLength(physExtent.m_axes.Col(0).x, physExtent.m_axes.Col(1).y, physExtent.m_axes.Col(2).z);
 
  vtkSmartPointer<vtkTransform> axesTransform =
    vtkSmartPointer<vtkTransform>::New();
  //axesTransform->Translate(volExtent.m_ul.x, volExtent.m_ul.y, volExtent.m_ul.z);
  //f64 volOr[3] = {0};  vtkTransform::GetOrientation(volOr, volTrans.GetVTKMatrix());
  //axesTransform->RotateZ(volOr[2]);  axesTransform->RotateX(volOr[0]);  axesTransform->RotateY(volOr[1]);  
  //m_volumeAxes->SetUserTransform(axesTransform);
  m_volumeAxes->PokeMatrix(volTrans.GetVTKMatrix());
  onShowVolumeAxesChanged();

  this->GetRenderWindow()->AddRenderer(m_renderer);

#endif
}