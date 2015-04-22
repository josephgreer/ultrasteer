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
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkPointPicker.h>
#include <vtkInteractorStyleImage.h>
#include <QtDebug>

namespace Nf
{
  class ImageViewerWidget : public ResizableQVTKWidget, public ParameterCollection
  {
  protected:
    vtkSmartPointer < vtkImageImport > m_importer;
    vtkSmartPointer < vtkImageMapper > m_mapper;
    vtkSmartPointer < vtkImageActor > m_imageActor;
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

  class ImageViewer2DTeleoperationWidget : public ImageViewerWidget
  {
  protected:
    vtkSmartPointer < vtkTextActor > m_textActor;
    vtkSmartPointer < vtkPointPicker > m_pointPicker;
    double m_x,m_y;
    bool m_initTeleop;

  public:
    ImageViewer2DTeleoperationWidget(QWidget *parent);
    virtual ~ImageViewer2DTeleoperationWidget();
    void SetImage(const IplImage *im);
    void SetXY(double x, double y);
  };

}

#endif // IMAGEVIEWERWIDGET_H
