#pragma once

#include "QVTKWidget.h"
#include "UICore.h"
#include "RPFileReader.h"
#include "VolumeCreator.h"
#include "CubeVisualizer.h"
#include <vtkAxesActor.h>
#include <vtkVolume.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkCommand.h>
#include <vtkImageImport.h>
#include "Resizable.h"
#include <QGridLayout>
#include "VTKTransferFunctionWidget.h"
#include "USVisualizerWidget.h"
#include "ParticleFilter.h"

class PFVisualizerWidget : public USVisualizerWidget
{
    Q_OBJECT
public: 
  PFVisualizerWidget(Nf::RPVolumeCreator *rpvc = NULL);
};

class PFVisualizer : public USVisualizer
{
  Q_OBJECT

protected:
  const Nf::ParticleFilter *m_pf;

public:
  PFVisualizer(QWidget *parent, USVisualizerWidget *usVis = NULL);
  virtual ~PFVisualizer();
  void SetParticleFilter(const Nf::ParticleFilter *pf);
};