#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
#include "VTKTransferFunctionWidget.h"
#include <vtkContextScene.h>
#include <vtkChart.h>

using namespace Nf;

VTKTransferFunctionWidget::VTKTransferFunctionWidget()
: QVTKWidget()
, Nf::ParameterCollection("Piecewise Transfer Function Widget")
, m_opacity(NULL)
, m_cp(NULL)
, m_ccp(NULL)
{
  m_opacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
  m_opacity->AddPoint(0., 1);
  m_opacity->AddPoint(128, 0.5);
  m_opacity->AddPoint(255, 1.0);

  m_col = vtkSmartPointer<vtkColorTransferFunction>::New();
  m_col->AddRGBPoint(0, 0, 0, 0);
  m_col->AddRGBPoint(128, 0.5, 0.5, 0.5);
  m_col->AddRGBPoint(255, 1, 1, 1);
  m_col->Build();

  m_chart = vtkSmartPointer<vtkChartXY>::New();

  m_view = vtkSmartPointer<vtkContextView>::New();

  m_comp = vtkSmartPointer<vtkCompositeTransferFunctionItem>::New();

  m_cp = vtkSmartPointer<vtkPiecewiseControlPointsItem>::New();
  m_cp->SetPiecewiseFunction(m_opacity);

  m_ccp = vtkSmartPointer<vtkColorTransferControlPointsItem>::New();
 // m_ccp->SetColorTransferFunction(m_col);
 
  m_connections = vtkSmartPointer<vtkEventQtSlotConnect>::New();
}

VTKTransferFunctionWidget::~VTKTransferFunctionWidget()
{
}

QSize VTKTransferFunctionWidget::sizeHint() const
{
  return QSize(VIS_WIDTH,150);
}

void VTKTransferFunctionWidget::Initialize()
{
  m_view->GetRenderer()->SetBackground(0.5,0,0);
  m_view->SetRenderWindow(this->GetRenderWindow());
 // this->SetRenderWindow(m_view->GetRenderWindow());

  m_comp->SetOpacityFunction(m_opacity);
  m_comp->SetColorTransferFunction(m_col);
  m_comp->SetColor(1,1,1);
  m_comp->SetOpacity(1);
  m_comp->SetMaskAboveCurve(true);

  m_chart->AddPlot(m_comp);
  m_chart->AddPlot(m_cp);
  //m_chart->AddPlot(m_ccp);
  m_chart->DrawAxesAtOriginOn();
  m_view->GetScene()->AddItem(m_chart);

  m_chart->SetInteractive(true);
  m_cp->SetEndPointsXMovable(false);
  m_cp->SetEndPointsYMovable(true);

  //m_view->SetInteractor(this->GetInteractor());
}


vtkSmartPointer<vtkColorTransferFunction> VTKTransferFunctionWidget::GetColorTransferFunction()
{
  return m_col;
}

vtkSmartPointer<vtkPiecewiseFunction> VTKTransferFunctionWidget::GetOpacityTransferFunction()
{
  return m_opacity;
}

void VTKTransferFunctionWidget::SetInteractionObserver(QObject *qobj)
{
  m_connections->Connect(this->GetRenderWindow()->GetInteractor(), vtkCommand::LeftButtonReleaseEvent, 
    qobj,
    SLOT(TransferFunctionChanged(vtkObject *, unsigned long, void*, void*, vtkCommand *)));
}