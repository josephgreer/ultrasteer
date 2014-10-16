#pragma once

#include "UICore.h"

// CTK includes
#include "ctkTransferFunctionView.h"
#include "ctkTransferFunctionGradientItem.h"
#include "ctkTransferFunctionControlPointsItem.h"
#include <vtkColorTransferFunction.h>

class CTKColorTransferFucntionWidget : public ctkTransferFunctionView , public Nf::ParameterCollection
{
    Q_OBJECT
private:
  vtkSmartPointer<vtkColorTransferFunction> *m_tf;
  ctkTransferFunctionGradientItem *m_gradient;
  ctkTransferFunctionControlPointsItem *m_cp;
  QSharedPointer<ctkTransferFunction> m_qtf;

public: 
  CTKColorTransferFucntionWidget();
  ~CTKColorTransferFucntionWidget();
  void Initialize();

  virtual QSize sizeHint() const;
};