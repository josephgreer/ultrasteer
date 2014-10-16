#pragma once

#include "UICore.h"

// CTK includes
#include "ctkTransferFunctionView.h"
#include "ctkTransferFunctionNativeItem.h"
#include "ctkTransferFunctionGradientItem.h"
#include "ctkTransferFunctionControlPointsItem.h"
#include <vtkPieceWiseFunction.h>

class CTKTransferFunctionWidget : public ctkTransferFunctionView , public Nf::ParameterCollection
{
    Q_OBJECT
private:
  vtkSmartPointer<vtkPiecewiseFunction> m_tf;
  ctkTransferFunctionNativeItem *m_item;
  ctkTransferFunctionGradientItem *m_gradient;
  ctkTransferFunctionControlPointsItem *m_cp;
  QSharedPointer<ctkTransferFunction> m_qtf;

public: 
  CTKTransferFunctionWidget();
  ~CTKTransferFunctionWidget();
  void Initialize();

  virtual QSize sizeHint() const;
};