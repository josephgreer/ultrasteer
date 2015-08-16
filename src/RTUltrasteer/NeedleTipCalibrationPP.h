#ifndef NEEDLETIPCALIBRATIONPP_H
#define NEEDLETIPCALIBRATIONPP_H

#include "ImageViewerWidget.h"

#include "UICore.h"
#include <QVTKWidget.h>
#include "Resizable.h"
#include <vtkRendererCollection.h>
#include <vtkPointPicker.h>
#include <vtkInteractorStyleImage.h>
#include "EstimatorWidget.h"

namespace Nf
{  
  class NeedleTipCalibrationPP : public vtkInteractorStyleImage
    //vtkInteractorStyleImage
  {
  private:
    EstimatorFileWidget *m_estimatorWidget;
    vtkSmartPointer < vtkRenderWindowInteractor > m_interactor;

  public:
    static NeedleTipCalibrationPP* New()
    { return new NeedleTipCalibrationPP; }

    vtkTypeMacro(NeedleTipCalibrationPP, vtkInteractorStyleImage);

    void SetEstimatorWidget(EstimatorFileWidget *widget)
    { m_estimatorWidget = widget; }

    void SetRenderWindowInteractor(vtkSmartPointer < vtkRenderWindowInteractor > interactor)
    { m_interactor = interactor; }

    virtual void OnLeftButtonDown(); 
  };
}

#endif // NEEDLETIPCALIBRATIONPP_H