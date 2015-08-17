#ifndef NEEDLETIPCALIBRATIONPP_H
#define NEEDLETIPCALIBRATIONPP_H

#include "ImageViewerWidget.h"

#include "UICore.h"
#include <QVTKWidget.h>
#include "Resizable.h"
#include <vtkRendererCollection.h>
#include <vtkPointPicker.h>
#include <vtkInteractorStyleImage.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include "EstimatorWidget.h"

namespace Nf
{  
  class NeedleTipCalibrationPP : public vtkInteractorStyleImage
    //vtkInteractorStyleImage
  {
  private:
    EstimatorFileWidget *m_estimatorWidget;
    vtkSmartPointer < vtkRenderWindowInteractor > m_interactor;
    vtkSmartPointer < vtkPointData > m_pointData;
    vtkSmartPointer < vtkImageData > m_image;

  public:
    NeedleTipCalibrationPP();

    static NeedleTipCalibrationPP* New()
    { return new NeedleTipCalibrationPP(); }

    vtkTypeMacro(NeedleTipCalibrationPP, vtkInteractorStyleImage);

    void SetEstimatorWidget(EstimatorFileWidget *widget)
    { m_estimatorWidget = widget; }

    void SetRenderWindowInteractor(vtkSmartPointer < vtkRenderWindowInteractor > interactor)
    { m_interactor = interactor; }

    void SetImageData(vtkSmartPointer <vtkImageData> imageData) { m_image = imageData; }

    virtual void OnLeftButtonDown(); 
  };
}

#endif // NEEDLETIPCALIBRATIONPP_H