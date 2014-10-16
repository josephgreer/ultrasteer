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

CTKColorTransferFucntionWidget::CTKColorTransferFucntionWidget()
: ctkTransferFunctionView()
, Nf::ParameterCollection("Transfer Function Widget")
{
#if 1
  
  vtkSmartPointer<vtkColorTransferFunction> ctf = 
    vtkSmartPointer<vtkColorTransferFunction>::New();
  //
  ctf->AddRGBPoint(0.2, 1.,0.,0., 0.5, 0.);
  //ctf->AddRGBPoint(0.5, 0.,0.,1.);
  ctf->AddRGBPoint(0.8, 0.,1.,0.);
  //ctf->AddHSVPoint(0., 0.,1.,1.);
  //ctf->AddHSVPoint(1., 0.66666,1.,1.);

  m_qtf = QSharedPointer<ctkTransferFunction>( new ctkVTKColorTransferFunction(ctf));
  m_qtf->insertControlPoint(.5);
  m_gradient = 
    new ctkTransferFunctionGradientItem(m_qtf.data());
  m_cp = 
    new ctkTransferFunctionControlPointsItem(m_qtf.data());
#else

  vtkSmartPointer<vtkPiecewiseFunction> pwf =
    vtkSmartPointer<vtkPiecewiseFunction>::New();
  //
  pwf->AddPoint(0., 1.);
  pwf->AddPoint(0.2, 1.2);
  pwf->AddPoint(0.3, 1.5);
  pwf->AddPoint(0.4, 2., 0.5, 0.5);
  pwf->AddPoint(0.9, 1.5);

  m_qtf =
    QSharedPointer<ctkTransferFunction>(new ctkVTKPiecewiseFunction(pwf));

  m_gradient = 
    new ctkTransferFunctionGradientItem(m_qtf.data());
  m_cp = 
    new ctkTransferFunctionControlPointsItem(m_qtf.data());
#endif
}

CTKColorTransferFucntionWidget::~CTKColorTransferFucntionWidget()
{
  if(this->m_gradient)
    delete m_gradient;
  if(this->m_cp)
    delete m_cp;
  if(this->m_tf)
    delete m_tf;
}

QSize CTKColorTransferFucntionWidget::sizeHint() const
{
  return QSize(640,100);
}

void CTKColorTransferFucntionWidget::Initialize()
{
  
  this->scene()->addItem(m_gradient);
  this->scene()->addItem(m_cp);
  //// the widget is not really shown here, only when app.exec() is called
  //transferFunctionView.show();
}