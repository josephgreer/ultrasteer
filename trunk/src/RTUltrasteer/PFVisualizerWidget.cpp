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
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMIPFunction.h>
#include <vtkVolumeTextureMapper2D.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
#include <vtkVolumeProperty.h>
#include <vtkCamera.h>

#include "PFVisualizerWidget.h"

using namespace Nf;

PFVisualizerWidget::PFVisualizerWidget(RPVolumeCreator *rpvc)
: USVisualizerWidget(rpvc)
{
}

PFVisualizer::PFVisualizer(QWidget *parent, USVisualizerWidget *usVis)
: USVisualizer(parent, usVis)
, m_pf(NULL)
{
}

void PFVisualizer::SetParticleFilter(const Nf::ParticleFilter *pf)
{
  m_pf = pf;
}

PFVisualizer::~PFVisualizer()
{
}