#pragma once

#include "UICore.h"

// CTK includes
#include "ctkTransferFunctionView.h"
#include "ctkTransferFunctionGradientItem.h"
#include "ctkTransferFunctionControlPointsItem.h"

class CTKTransferFunctionWidget : public ctkTransferFunctionView , public Nf::ParameterCollection
{
    Q_OBJECT
private:
  ctkTransferFunction *m_tf;
  ctkTransferFunctionGradientItem *m_gradient;
  ctkTransferFunctionControlPointsItem *m_cp;
  QSharedPointer<ctkTransferFunction> m_qtf;

public: 
  CTKTransferFunctionWidget();
  ~CTKTransferFunctionWidget();
  void Initialize();

  virtual QSize sizeHint() const;
};