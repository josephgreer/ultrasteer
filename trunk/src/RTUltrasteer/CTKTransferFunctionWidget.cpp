#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
#include <vtkPieceWiseFunction.h>
#include <vtkColorTransferFunction.h>

#include "CTKTransferFunctionWidget.h"
#include "ctkVTKPiecewiseFunction.h"

using namespace Nf;

CTKTransferFunctionWidget::CTKTransferFunctionWidget()
: ctkTransferFunctionView()
, Nf::ParameterCollection("Piecewise Transfer Function Widget")
, m_tf(NULL)
, m_item(NULL)
, m_cp(NULL)
, m_qtf(NULL)
{
  m_tf = vtkSmartPointer<vtkPiecewiseFunction>::New();;
  m_tf->AddPoint(0., 1.);
  m_tf->AddPoint(0.2, 1.2);
  m_tf->AddPoint(0.3, 1.5);
  m_tf->AddPoint(0.9, 1.5);

  m_qtf = QSharedPointer<ctkTransferFunction>( new ctkVTKPiecewiseFunction(m_tf));
  //m_qtf->insertControlPoint(.5);
  m_item = new ctkTransferFunctionNativeItem(m_qtf.data());
  m_gradient = new ctkTransferFunctionGradientItem(m_qtf.data());
  m_cp = new ctkTransferFunctionControlPointsItem(m_qtf.data());
}

CTKTransferFunctionWidget::~CTKTransferFunctionWidget()
{
  if(this->m_item)
    delete m_item;
  if(this->m_cp)
    delete m_cp;
}

QSize CTKTransferFunctionWidget::sizeHint() const
{
  return QSize(640,100);
}

void CTKTransferFunctionWidget::Initialize()
{
  
  //this->scene()->addItem(m_item);
  this->scene()->addItem(m_gradient);
  this->scene()->addItem(m_cp);
  //// the widget is not really shown here, only when app.exec() is called
  //transferFunctionView.show();
}