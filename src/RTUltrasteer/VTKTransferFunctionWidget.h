#pragma once

#include "QVTKWidget.h"
#include <vtkContextView.h>
#include <vtkChartXY.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPiecewiseControlPointsItem.h>
#include <vtkColorTransferControlPointsItem.h>
#include <vtkCompositeTransferFunctionItem.h>
#include <vtkColorTransferFunction.h>
#include <vtkCommand.h>
#include <vtkEventQtSlotConnect.h>
#include "UICore.h"
#include "Resizable.h"

class VTKTransferFunctionWidget : public Nf::Resizable, public Nf::ParameterCollection
{
  Q_OBJECT 
private:
  vtkSmartPointer<vtkPiecewiseFunction> m_opacity;
  vtkSmartPointer<vtkContextView> m_view;
  vtkSmartPointer<vtkChartXY> m_chart;
  vtkSmartPointer<vtkPiecewiseControlPointsItem> m_cp;
  vtkSmartPointer<vtkColorTransferControlPointsItem> m_ccp;
  vtkSmartPointer<vtkCompositeTransferFunctionItem> m_comp;
  vtkSmartPointer<vtkColorTransferFunction> m_col;
  vtkSmartPointer<vtkEventQtSlotConnect> m_connections;

public: 
  VTKTransferFunctionWidget();
  ~VTKTransferFunctionWidget();
  void Initialize();

  vtkSmartPointer<vtkColorTransferFunction> GetColorTransferFunction();
  vtkSmartPointer<vtkPiecewiseFunction> GetOpacityTransferFunction();

  void SetInteractionObserver(QObject *qobj);
};