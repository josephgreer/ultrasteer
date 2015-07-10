#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H

#include "UICore.h"
#include <QVTKWidget.h>
#include "Resizable.h"
#include "RPFileReader.h"
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
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageMapper3D.h>
#include <vtkImageData.h>
#include <QtDebug>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Nf
{
  class Teleoperation2DWidget; // Empty to avoid circular dependency

  class ImageViewerWidget : public ResizableQVTKWidget, public ParameterCollection
  {
  protected:
    vtkSmartPointer < vtkImageImport > m_importer;
    vtkSmartPointer < vtkImageMapper > m_mapper;
    vtkSmartPointer < vtkImageActor > m_imageActor;
    vtkSmartPointer < vtkRenderer > m_renderer;
    vtkSmartPointer < vtkImageFlip > m_flip;
    vtkSmartPointer < vtkRenderWindowInteractor > m_interactor;
    RPData m_rp;
    bool m_init;

  public:
    ImageViewerWidget(QWidget *parent);
    virtual ~ImageViewerWidget();
    virtual void SetImage(const RPData *rp);
  };

  class ImageViewerWidget3DPlane : public ImageViewerWidget
  {
  public:
    ImageViewerWidget3DPlane(QWidget *parent);
    virtual ~ImageViewerWidget3DPlane();
    virtual void SetImage(const RPData *rp);
  };

  class ImageViewer2DTeleoperationWidget : public ImageViewerWidget
  {
  protected:
    vtkSmartPointer < vtkTextActor > m_textActor1;
    vtkSmartPointer < vtkTextActor > m_textActor2;
    vtkSmartPointer < vtkPointPicker > m_pointPicker;
    vtkSmartPointer < vtkImageMapToColors > m_mapTransparency;
    vtkSmartPointer < vtkImageImport > m_maskImporter;
    vtkSmartPointer < vtkImageActor > m_maskActor;
    vtkSmartPointer < vtkImageFlip > m_maskFlip;
    vtkSmartPointer < vtkLookupTable > m_lookUpTable;
    IplImage *m_mask;
    
    bool m_initTeleop;
    Teleoperation2DWidget *m_teleoperationWidget;

  public:
    ImageViewer2DTeleoperationWidget(QWidget *parent);
    virtual ~ImageViewer2DTeleoperationWidget();
    void SetImage(const RPData *rp);
    void SetTargetOverlay(int r, Vec2d px);
    void SetTextOverlay(Vec2d px, Vec3d wpt);
    void SetInstrOverlay(char* str);
    void SetTeleoperation2DWidget(Teleoperation2DWidget *widget);
    void getImageDim(int &w, int &h);
  };

}

#endif // IMAGEVIEWERWIDGET_H
