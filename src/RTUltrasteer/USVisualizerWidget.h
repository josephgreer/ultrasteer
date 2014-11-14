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

class USVisualizerWidget : public Nf::ResizableQVTKWidget, public Nf::ParameterCollection, public Nf::Reinitializer
{
    Q_OBJECT
public: 
  USVisualizerWidget(Nf::RPVolumeCreator *rpvc = NULL);
  virtual void Initialize(Nf::RPData rp, vtkSmartPointer<vtkColorTransferFunction> ctf, vtkSmartPointer<vtkPiecewiseFunction> otf);
  virtual void Initialize(vtkSmartPointer<vtkColorTransferFunction> ctf, vtkSmartPointer<vtkPiecewiseFunction> otf);

  // helper function for setViewXY
  virtual void SetUSVisView(s32 axis1, s32 axis2);

  //Volume Data Creator
  Nf::RPVolumeCreator *m_rpvc;
  vtkSmartPointer<vtkRenderer> m_renderer;

  //Add a frame of RPData to the volume
  virtual void AddRPData(Nf::RPData data);

  //Volume
  vtkSmartPointer<vtkVolume> m_volume;

  //Volume Axes
  vtkSmartPointer<vtkAxesActor> m_volumeAxes;
  std::tr1::shared_ptr < Nf::CubeVisualizer > m_extentVis;

  //Frame Boundaries
  std::tr1::shared_ptr < Nf::CubeVisualizer > m_frameBoundaries;

  //Parameters

  //colorTransferFunction
  vtkSmartPointer<vtkColorTransferFunction> m_ctf;

  //opacity transfer function
  vtkSmartPointer<vtkPiecewiseFunction> m_otf;

  //showVolumeExtent
  std::tr1::shared_ptr < Nf::BoolParameter > m_showVolumeExtent;
  void onShowExtrasChanged();
  CLASS_CALLBACK(onShowExtrasChanged, USVisualizerWidget);

  //showVolumeAxes
  std::tr1::shared_ptr < Nf::BoolParameter > m_showVolumeAxes;

  //showFrameBoundaries
  std::tr1::shared_ptr < Nf::BoolParameter > m_showFrameBoundaries;

  //setViewXY
  std::tr1::shared_ptr < Nf::BoolParameter > m_setViewXY;
  void onSetViewXY();
  CLASS_CALLBACK(onSetViewXY, USVisualizerWidget)

  //setViewXZ
  std::tr1::shared_ptr < Nf::BoolParameter > m_setViewXZ;
  void onSetViewXZ();
  CLASS_CALLBACK(onSetViewXZ, USVisualizerWidget);

  //setViewYZ
  std::tr1::shared_ptr < Nf::BoolParameter > m_setViewYZ;
  void onSetViewYZ();
  CLASS_CALLBACK(onSetViewYZ, USVisualizerWidget);

  //renderMode
  std::tr1::shared_ptr < Nf::EnumParameter > m_renderMode;
  void onSetRenderMode();
  CLASS_CALLBACK(onSetRenderMode, USVisualizerWidget);


  //reinitialize callback
  virtual void Reinitialize();


  //For changes to the transfer function
  virtual void Execute(vtkObject *caller, unsigned long, void*);

public slots:
  void TransferFunctionChanged(vtkObject *caller, unsigned long, void*, void*, vtkCommand *);

protected:
  Nf::RPData m_last;
  void UpdateFrameBoundaries();

};

class USVisualizerWidgetFullRP : public USVisualizerWidget
{
    Q_OBJECT
protected:
  Nf::RPFullVolumeCreator *m_frpvc;

public: 
  USVisualizerWidgetFullRP();

  virtual void Initialize(vtkSmartPointer<vtkColorTransferFunction> ctf, vtkSmartPointer<vtkPiecewiseFunction> otf);

  //Add a frame of RPData to the volume
  virtual void AddRPData(Nf::RPData data);

  //reinitialize callback
  virtual void Reinitialize();
};

class USVisualizer : public Nf::ResizableQWidget, public Nf::ParameterCollection, public Nf::Reinitializer
{
  Q_OBJECT
protected:
  USVisualizerWidget *m_usVis;
  VTKTransferFunctionWidget *m_tfWidget;
  QGridLayout *m_layout;
  bool m_init;

public:
  USVisualizer(QWidget *parent, USVisualizerWidget *usVis = NULL);
  virtual ~USVisualizer();
  virtual void UpdateSize(QSize sz);
  virtual void AddRPData(Nf::RPData rp);
  virtual void Initialize(Nf::RPData rp);
  virtual void Reinitialize();
  CLASS_CALLBACK(Reinitialize, USVisualizer);

  std::tr1::shared_ptr < Nf::BoolParameter > m_clear;

  std::vector < QVTKWidget * > GetRepaintList();
};

class USVisualizerFullRP : public USVisualizer
{
  Q_OBJECT

public:
  USVisualizerFullRP(QWidget *parent);
  virtual void AddRPData(Nf::RPData rp);
  virtual void Initialize();
};