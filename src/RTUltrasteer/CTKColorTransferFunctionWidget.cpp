#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
#include <vtkPieceWiseFunction.h>
#include <vtkColorTransferFunction.h>

#include "CTKColorTransferFunctionWidget.h"
#include "ctkVTKColorTransferFunction.h"
#include "ctkVTKPiecewiseFunction.h"

using namespace Nf;

CTKColorTransferFunctionWidget::CTKColorTransferFunctionWidget()
: ctkTransferFunctionView()
, Nf::ParameterCollection("Color Transfer Function Widget")
, m_tf(NULL)
, m_gradient(NULL)
, m_cp(NULL)
, m_qtf(NULL)
{
  m_tf = vtkSmartPointer<vtkColorTransferFunction>::New();
  //
  m_tf->AddRGBPoint(0.2, 1.,0.,0., 0.5, 0.);
  //ctf->AddRGBPoint(0.5, 0.,0.,1.);
  m_tf->AddRGBPoint(0.8, 0.,1.,0.);
  //ctf->AddHSVPoint(0., 0.,1.,1.);
  //ctf->AddHSVPoint(1., 0.66666,1.,1.);

  m_qtf = QSharedPointer<ctkTransferFunction>( new ctkVTKColorTransferFunction(m_tf));
  m_qtf->insertControlPoint(.5);
  m_gradient = 
    new ctkTransferFunctionGradientItem(m_qtf.data());
  m_cp = 
    new ctkTransferFunctionControlPointsItem(m_qtf.data());
}

CTKColorTransferFunctionWidget::~CTKColorTransferFunctionWidget()
{
  if(this->m_gradient)
    delete m_gradient;
  if(this->m_cp)
    delete m_cp;
}

QSize CTKColorTransferFunctionWidget::sizeHint() const
{
  return QSize(640,100);
}

void CTKColorTransferFunctionWidget::Initialize()
{
  
  this->scene()->addItem(m_gradient);
  this->scene()->addItem(m_cp);
  //// the widget is not really shown here, only when app.exec() is called
  //transferFunctionView.show();
}