#ifndef MOUSEINTERACTORSTYLEPP_H
#define MOUSEINTERACTORSTYLEPP_H

#include "ImageViewerWidget.h"

#include "UICore.h"
#include <QVTKWidget.h>
#include "Resizable.h"
#include <vtkImageImport.h>
#include <vtkImageMapper.h>
#include <vtkActor2D.h>
#include <vtkImageActor.h>
#include <vtkImageFlip.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkPointPicker.h>
#include <vtkInteractorStyleImage.h>
#include <QtDebug>
#include <vtkInteractorStyleTrackballCamera.h>

namespace Nf
{  
  class MouseInteractorStylePP : public vtkInteractorStyleImage
  {
  public:
    static MouseInteractorStylePP* New()
    { return new MouseInteractorStylePP; }

    vtkTypeMacro(MouseInteractorStylePP, vtkInteractorStyleImage);

    void SetImageViewerWidget(ImageViewer2DTeleoperationWidget *viewer) 
    { m_imageViewerWidget = viewer; }

    void SetImageActor(vtkSmartPointer < vtkImageActor > imageActor)
    { m_imageActor = imageActor; }

    virtual void OnLeftButtonDown() 
    {	
      qDebug() << "Pressed left mouse button." << "\n";
      point[0] = this->Interactor->GetEventPosition()[0];
      point[1] = this->Interactor->GetEventPosition()[1];
      qDebug() << "(x,y) = (" << point[0] << "," << point[1] << ")" << "\n";

      double bounds[6];
      m_imageActor->GetDisplayBounds(bounds);

      // Tell the ImageViewerWidget what the selected point is
      m_imageViewerWidget->SetXY(point[0],point[1]);

      // Forward events
      vtkInteractorStyleImage::OnLeftButtonDown();
    }
    void GetPoint(double &x, double &y)
    {	//Set inputs x and y to the most recent read image point  
      x = point[0];
      y = point[1];
    }
  private:
    // Point arry for user selected image point
    double point[2];
    ImageViewer2DTeleoperationWidget* m_imageViewerWidget;
    vtkSmartPointer < vtkImageActor > m_imageActor;
  };
}

#endif // MOUSEINTERACTORSTYLEPP_H