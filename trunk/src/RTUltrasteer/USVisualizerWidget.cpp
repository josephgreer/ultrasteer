#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMIPFunction.h>
#include <vtkVolumeTextureMapper2D.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
#include <vtkVolumeProperty.h>
#include <vtkCamera.h>

#include "USVisualizerWidget.h"

using namespace Nf;

USVisualizerWidget::USVisualizerWidget(RPVolumeCreator *rpvc)
: Nf::ResizableQVTKWidget(NULL, QSize(VIS_WIDTH, VIS_HEIGHT))
, Nf::ParameterCollection("Ultrasound Visualization")
, m_volumeAxes(NULL)
, m_volume(NULL)
, m_renderer(NULL)
{
  if(rpvc == NULL)
    rpvc = new RPVolumeCreator();
  m_rpvc = rpvc;

  ADD_BOOL_PARAMETER(m_showVolumeExtent, "Show Volume Extent", CALLBACK_POINTER(onShowVolumeExtentChanged, USVisualizerWidget), this, false);
  ADD_BOOL_PARAMETER(m_showVolumeAxes, "Show Volume Axes", CALLBACK_POINTER(onShowVolumeAxesChanged, USVisualizerWidget), this, false);
  ADD_ACTION_PARAMETER(m_setViewXY, "Set View XY", CALLBACK_POINTER(onSetViewXY, USVisualizerWidget), this, true); 
  ADD_ACTION_PARAMETER(m_setViewXZ, "Set View XZ", CALLBACK_POINTER(onSetViewXZ, USVisualizerWidget), this, true); 
  ADD_ACTION_PARAMETER(m_setViewYZ, "Set View YZ", CALLBACK_POINTER(onSetViewYZ, USVisualizerWidget), this, true); 
  ADD_ENUM_PARAMETER(m_renderMode, "Render Mode", CALLBACK_POINTER(onSetRenderMode, USVisualizerWidget), this, QtEnums::VisRenderMethod::RayCastingMIP, "VisRenderMethod");
  ADD_CHILD_COLLECTION(m_rpvc);
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

void USVisualizerWidget::SetUSVisView(s32 axis1, s32 axis2)
{
  vtkSmartPointer <vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
  Vec3d up = m_rpvc->GetVolumeOrientation().Col(axis1);
  Vec3d focal = m_rpvc->GetVolumeOrientation().Col(axis2);
  camera->SetPosition(0,0,0);
  camera->SetFocalPoint(focal.x, focal.y, focal.z);
  camera->SetViewUp(up.x, up.y, up.z);

  f64 *bounds = m_volume->GetBounds();
  m_renderer->SetActiveCamera(camera);
  m_renderer->ResetCamera(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], 
    bounds[5]);
  m_renderer->GetActiveCamera()->Zoom(1.3);
}

void USVisualizerWidget::onSetViewXY()
{
  SetUSVisView(1,2);
}

void USVisualizerWidget::onSetViewXZ()
{
  SetUSVisView(2,1);
}

void USVisualizerWidget::onSetViewYZ()
{
  SetUSVisView(2,0);
}

void USVisualizerWidget::onSetRenderMode()
{
  if(m_renderMode->GetValue() == QtEnums::VisRenderMethod::RayCastingMIP) {
    //Volume Mapper
    vtkSmartPointer<vtkVolumeRayCastMIPFunction> rayCastFunction =
      vtkSmartPointer<vtkVolumeRayCastMIPFunction>::New();

    vtkSmartPointer<vtkVolumeRayCastMapper> volumeMapper =
      vtkSmartPointer<vtkVolumeRayCastMapper>::New();
    volumeMapper->SetInputConnection(m_rpvc->GetImporter()->GetOutputPort(0));
    volumeMapper->SetVolumeRayCastFunction(rayCastFunction); 
    volumeMapper->SetBlendModeToMaximumIntensity();
    m_volume->Modified();
    m_volume->SetMapper(volumeMapper);
  } else if(m_renderMode->GetValue() == QtEnums::VisRenderMethod::Texture_2D) {
    vtkSmartPointer<vtkVolumeTextureMapper3D> volumeMapper = vtkSmartPointer<vtkVolumeTextureMapper3D>::New();
    volumeMapper->SetInputConnection(m_rpvc->GetImporter()->GetOutputPort(0));
    m_volume->Modified();
    m_volume->SetMapper(volumeMapper);
  } else {
    assert(0);
  }
}


void USVisualizerWidget::Reinitialize()
{
  m_rpvc->Release();
  if(m_renderer != NULL) {
    m_renderer->RemoveAllViewProps();
    this->GetRenderWindow()->RemoveRenderer(m_renderer);
  }
  if(m_last.gps.valid && m_last.b8)
    Initialize(m_last, m_ctf, m_otf);
  Reinitializer::Reinitialize();
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

void USVisualizerWidget::Initialize(vtkSmartPointer<vtkColorTransferFunction> ctf, vtkSmartPointer<vtkPiecewiseFunction> otf)
{
  m_ctf = ctf;
  m_otf = otf;

  m_renderer = 
    vtkSmartPointer<vtkRenderer>::New();
  m_renderer->SetBackground(0.0, 0.0, 0.0);

  Vec3i dims = m_rpvc->GetVolumeDims();
  Cubed volExtent = m_rpvc->GetVolumeCubeExtent();
  Cubed physExtent = m_rpvc->GetVolumePhysicalExtent();

  //Volumne
  Matrix33d orientation = m_rpvc->GetVolumeOrientation();
  Matrix44d volTrans = Matrix44d::FromOrientationAndTranslation(m_rpvc->GetVolumeOrientation(), volExtent.m_ul);
  m_volume = vtkSmartPointer<vtkVolume>::New();
  onSetRenderMode();
  m_volume->PokeMatrix(volTrans.GetVTKMatrix());
  m_volume->Update();

  //Volume Property
  vtkSmartPointer<vtkVolumeProperty> volumeProperty =
    vtkSmartPointer<vtkVolumeProperty>::New();
  volumeProperty->SetColor(m_ctf);
  volumeProperty->SetScalarOpacity(m_otf);

  m_volume->SetProperty(volumeProperty);

  //Volume Renderer
  m_renderer->AddViewProp(m_volume);
  m_renderer->TwoSidedLightingOn();

  //Cube Visualization
  u8 cubeColor[3] = {27, 161, 226};
  m_extentVis = std::tr1::shared_ptr < CubeVisualizer > (new CubeVisualizer(volExtent, cubeColor));
  onShowVolumeExtentChanged();

  //Volume Axes Visualization
  m_volumeAxes = vtkSmartPointer<vtkAxesActor>::New();
  m_volumeAxes->SetTotalLength(physExtent.m_axes.Col(0).x, physExtent.m_axes.Col(1).y, physExtent.m_axes.Col(2).z);
  char lbl[100];
  sprintf(lbl, "X - %2.1f cm", physExtent.m_axes.Col(0).x/10.0);
  m_volumeAxes->SetXAxisLabelText(lbl);
  sprintf(lbl, "Y - %2.1f cm", physExtent.m_axes.Col(1).y/10.0);
  m_volumeAxes->SetYAxisLabelText(lbl);
  sprintf(lbl, "Z - %2.1f cm", physExtent.m_axes.Col(2).z/10.0);
  m_volumeAxes->SetZAxisLabelText(lbl);
 
  vtkSmartPointer<vtkTransform> axesTransform =
    vtkSmartPointer<vtkTransform>::New();
  //axesTransform->Translate(volExtent.m_ul.x, volExtent.m_ul.y, volExtent.m_ul.z);
  //f64 volOr[3] = {0};  vtkTransform::GetOrientation(volOr, volTrans.GetVTKMatrix());
  //axesTransform->RotateZ(volOr[2]);  axesTransform->RotateX(volOr[0]);  axesTransform->RotateY(volOr[1]);  
  //m_volumeAxes->SetUserTransform(axesTransform);
  m_volumeAxes->PokeMatrix(volTrans.GetVTKMatrix());
  onShowVolumeAxesChanged();

  this->GetRenderWindow()->AddRenderer(m_renderer);

  f64 *volBounds = m_volume->GetBounds();
  this->m_renderer->ResetCamera(volBounds[0], volBounds[1], volBounds[2], volBounds[3],
    volBounds[4], volBounds[5]);

  vtkCamera *cam = m_renderer->GetActiveCamera();
  cam->Zoom(1.5);
  this->m_renderer->SetActiveCamera(cam);


}

void USVisualizerWidget::Initialize(RPData rp, vtkSmartPointer<vtkColorTransferFunction> ctf, vtkSmartPointer<vtkPiecewiseFunction> otf)
{
  //Raw volume data
  m_rpvc->Initialize(rp, (Reinitializer *)this);
  Initialize(ctf, otf);
}

void USVisualizerWidget::AddRPData(RPData rp)
{
  m_rpvc->AddRPData(rp);
  m_last.Release();
  m_last = rp.Clone();
}

void USVisualizerWidget::Execute(vtkObject *caller, unsigned long, void*)
{
  vtkSmartPointer<vtkVolumeProperty> volumeProperty =
    vtkSmartPointer<vtkVolumeProperty>::New();
  volumeProperty->SetColor(m_ctf);
  volumeProperty->SetScalarOpacity(m_otf);

  m_volume->SetProperty(volumeProperty);
  m_volume->Modified();
  //this->repaint();
}


void USVisualizerWidget::TransferFunctionChanged(vtkObject *caller, unsigned long, void*, void*, vtkCommand *)
{
  vtkSmartPointer<vtkVolumeProperty> volumeProperty =
    vtkSmartPointer<vtkVolumeProperty>::New();
  volumeProperty->SetColor(m_ctf);
  volumeProperty->SetScalarOpacity(m_otf);

  m_volume->SetProperty(volumeProperty);
  m_volume->Modified();
  this->repaint();
}


USVisualizerWidgetFullRP::USVisualizerWidgetFullRP()
: USVisualizerWidget((RPVolumeCreator *)new RPFullVolumeCreator())
{
}

void USVisualizerWidgetFullRP::Initialize(vtkSmartPointer<vtkColorTransferFunction> ctf, vtkSmartPointer<vtkPiecewiseFunction> otf)
{
  RPFullVolumeCreator * rpvc = (RPFullVolumeCreator *)m_rpvc;
  rpvc->Initialize(this);
  rpvc->Start();

  USVisualizerWidget::Initialize(ctf, otf);
}

void USVisualizerWidgetFullRP::AddRPData(RPData data)
{
  assert(0);
}

void USVisualizerWidgetFullRP::Reinitialize()
{
  m_rpvc->Release();
  m_renderer->RemoveAllViewProps();
  this->GetRenderWindow()->RemoveRenderer(m_renderer);
  m_rpvc->Release();
  Initialize(m_ctf, m_otf);
  Reinitializer::Reinitialize();
}

USVisualizer::USVisualizer(QWidget *parent, USVisualizerWidget *usVis)
: Nf::ParameterCollection("Ultrasound Volume Visualizer")
, Nf::ResizableQWidget(parent, QSize(VIS_WIDTH, VIS_HEIGHT))
, m_init(false)
{
  m_tfWidget = new VTKTransferFunctionWidget();
  if(usVis == NULL)
    usVis = new USVisualizerWidget();
  m_usVis = usVis;

  m_layout = new QGridLayout(parent);
  m_layout->addWidget(m_tfWidget, 0, 0);
  m_layout->addWidget(m_usVis, 1, 0);
  this->setLayout(m_layout);

  m_tfWidget->SetInteractionObserver(m_usVis);

  ADD_ACTION_PARAMETER(m_clear, "Clear", CALLBACK_POINTER(Reinitialize, USVisualizer), this, false);
  ADD_CHILD_COLLECTION(m_usVis);
}

void USVisualizer::Reinitialize()
{
  if(m_init) {
    m_usVis->Reinitialize();
  }
}

void USVisualizer::Initialize(RPData rp)
{
  m_tfWidget->Initialize();
  m_usVis->Initialize(rp, m_tfWidget->GetColorTransferFunction(), m_tfWidget->GetOpacityTransferFunction());
  m_init = true;
}

USVisualizer::~USVisualizer()
{
  delete m_tfWidget;
  delete m_usVis;
  delete m_layout;
}

std::vector < QVTKWidget * > USVisualizer::GetRepaintList()
{
  std::vector < QVTKWidget * > res;
  res.push_back(m_usVis);
  return res;
}

void USVisualizer::UpdateSize(QSize sz)
{
  ResizableQWidget::UpdateSize(sz);
  m_tfWidget->UpdateSize(QSize(sz.width()-20, (sz.height()/4)));
  m_usVis->UpdateSize(QSize(sz.width()-20, (sz.height()*3/4)));
}

void USVisualizer::AddRPData(RPData rp)
{
  if(!m_init)
    Initialize(rp);
  m_usVis->AddRPData(rp);
}

USVisualizerFullRP::USVisualizerFullRP(QWidget *parent)
: USVisualizer(parent, (USVisualizerWidget *)new USVisualizerWidgetFullRP())
{
}

void USVisualizerFullRP::Initialize()
{
  USVisualizerWidgetFullRP * usVis = (USVisualizerWidgetFullRP *)m_usVis;
  m_tfWidget->Initialize();
  usVis->Initialize(m_tfWidget->GetColorTransferFunction(), m_tfWidget->GetOpacityTransferFunction());
}

void USVisualizerFullRP::AddRPData(RPData rp)
{
  assert(0);
}