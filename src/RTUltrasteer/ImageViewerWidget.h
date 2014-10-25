#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H

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

namespace Nf
{
  class ImageViewerWidget : public ResizableQVTKWidget, public ParameterCollection
  {
  protected:
    vtkSmartPointer < vtkImageImport > m_importer;
    vtkSmartPointer < vtkImageMapper > m_mapper;
    vtkSmartPointer < vtkActor2D > m_imageActor;
    vtkSmartPointer < vtkImageActor > m_imAct;
    vtkSmartPointer < vtkRenderer > m_renderer;
    vtkSmartPointer < vtkImageFlip > m_flip;
    vtkSmartPointer < vtkRenderWindowInteractor > m_interactor;
    IplImage *m_im;
    bool m_init;

  public:
    ImageViewerWidget(QWidget *parent);
    ~ImageViewerWidget();
    void SetImage(const IplImage *im);

  };
}

#endif // IMAGEVIEWERWIDGET_H
