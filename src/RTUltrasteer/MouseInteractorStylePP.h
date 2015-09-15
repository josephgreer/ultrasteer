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
#include <vtkRendererCollection.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkPointPicker.h>
#include <vtkInteractorStyleImage.h>
#include <QtDebug>
#include <vtkInteractorStyleTrackballCamera.h>
#include "Teleoperation2DWidget.h"
#include "ControlAlgorithms.h"
#include <QSound>

namespace Nf
{  
  class MouseInteractorStylePP : public vtkInteractorStyleImage
    //vtkInteractorStyleImage
  {
  public:
    static MouseInteractorStylePP* New()
    { return new MouseInteractorStylePP; }

    vtkTypeMacro(MouseInteractorStylePP, vtkInteractorStyleImage);

    void SetImageViewerWidget(ImageViewer2DTeleoperationWidget *viewer) 
    { m_imageViewerWidget = viewer; }

    void SetTeleoperation2DWidget(Teleoperation2DWidget *widget)
    { m_TeleoperationWidget = widget; }

    void SetControl(ControlAlgorithms *control)
    { m_control = control; }

    void SetRenderWindowInteractor(vtkSmartPointer < vtkRenderWindowInteractor > interactor)
    { m_interactor = interactor; }

    virtual void OnRightButtonDown() 
    {	
      qDebug() << "Pressed right mouse button." << "\n";

      // play a sound effect
      QSound::play("C:/Windows/Media/Speech On.wav");

      // Check that the robot calibration has been loaded
      m_TeleoperationWidget->checkCalibrations();

      // find coordinates in viewer pixels
      Vec2d vw;
      vw.x = this->Interactor->GetEventPosition()[0];
      vw.y = this->Interactor->GetEventPosition()[1];

      // find coordinates in image pixels
      this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0], 
        this->Interactor->GetEventPosition()[1], 
        0,  // always zero.
        this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
      double ima[3];
      this->Interactor->GetPicker()->GetPickPosition(ima);
      Vec3d im(ima[0],ima[1],ima[2]);
      // flip the y-axis point
      im.y = -1.0*ima[1]; 

      // Update the target control point
      Vec2d im2D( im.x, im.y );
      m_control->SetTarget(im2D);
      m_TeleoperationWidget->updateTeleoperationVisualization();

      // Forward events if functionality is desired
      //vtkInteractorStyleImage::OnLeftButtonDown();
    }

    virtual void OnLeftButtonDown() 
    {	
      qDebug() << "Pressed left mouse button." << "\n";

      // play a sound effect
      QSound::play("C:/Windows/Media/Windows Default.wav");
   
      // Check that the robot calibration has been loaded
      m_TeleoperationWidget->checkCalibrations();

      // find coordinates in viewer pixels
      Vec2d vw;
      vw.x = this->Interactor->GetEventPosition()[0];
      vw.y = this->Interactor->GetEventPosition()[1];

      // find coordinates in image pixels
      this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0], 
        this->Interactor->GetEventPosition()[1], 
        0,  // always zero.
        this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
      double ima[3];
      this->Interactor->GetPicker()->GetPickPosition(ima);
      Vec3d im(ima[0],ima[1],ima[2]);
      // flip the y-axis point
      im.y = -1.0*ima[1]; 

      // Update the target control point
      Vec2d im2D( im.x, im.y );
      m_control->ManualNeedleTipSelection(im2D);
      m_TeleoperationWidget->updateTeleoperationVisualization();
    }
  
  private:
    // Point arry for user selected image point
    ImageViewer2DTeleoperationWidget* m_imageViewerWidget;
    Teleoperation2DWidget *m_TeleoperationWidget;
    ControlAlgorithms *m_control;
    vtkSmartPointer < vtkRenderWindowInteractor > m_interactor;
  };
}

#endif // MOUSEINTERACTORSTYLEPP_H