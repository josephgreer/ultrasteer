#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H

#include "UICore.h"
#include "CubeVisualizer.h"
#include "ControlAlgorithms.h"
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
  //class ControlAlgorithms;    // Empty to avoid circular dependency

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
    bool m_useTrackball;

  public:
    ImageViewerWidget(QWidget *parent);
    virtual ~ImageViewerWidget();
    virtual void SetImage(const RPData *rp);
  };

  
  class Image3DImagePlaneWidget : public ImageViewerWidget, public SphereContainer
  {
  protected:
    Matrix44d m_cal;        //Calibration Matrix for transducer (maps image coords to world coords)

  public:
    Image3DImagePlaneWidget(QWidget *parent);
    virtual ~Image3DImagePlaneWidget();
    virtual void SetImage(const RPData *rp);

    //update Frame boundaries
    void UpdateFrameBoundaries();

    //showFrameBoundaries
    std::tr1::shared_ptr < Nf::BoolParameter > m_showFrameBoundaries;

    void onShowExtrasChanged();
    CLASS_CALLBACK(onShowExtrasChanged, Image3DImagePlaneWidget);

    //Frame Boundaries
    std::tr1::shared_ptr < Nf::CubeVisualizer > m_frameBoundaries;

    void SetUSVisView(s32 axis1, s32 axis2);

    //setViewXY
    std::tr1::shared_ptr < Nf::BoolParameter > m_setViewXY;
    void onSetViewXY();
    CLASS_CALLBACK(onSetViewXY, Image3DImagePlaneWidget);

    //setViewXZ
    std::tr1::shared_ptr < Nf::BoolParameter > m_setViewXZ;
    void onSetViewXZ();
    CLASS_CALLBACK(onSetViewXZ, Image3DImagePlaneWidget);

    //setViewYZ
    std::tr1::shared_ptr < Nf::BoolParameter > m_setViewYZ;
    void onSetViewYZ();
    CLASS_CALLBACK(onSetViewYZ, Image3DImagePlaneWidget);
  };


  class ImageViewer2DTeleoperationWidget : public ImageViewerWidget
  {
  protected:
    vtkSmartPointer < vtkTextActor > m_textActor1;
    vtkSmartPointer < vtkTextActor > m_textActor2;
    vtkSmartPointer < vtkTextActor > m_textActor3;
    vtkSmartPointer < vtkTextActor > m_textActor4;
    vtkSmartPointer < vtkPointPicker > m_pointPicker;
    vtkSmartPointer < vtkImageMapToColors > m_mapTransparency;
    vtkSmartPointer < vtkImageImport > m_maskImporter;
    vtkSmartPointer < vtkImageActor > m_maskActor;
    vtkSmartPointer < vtkImageFlip > m_maskFlip;
    vtkSmartPointer < vtkLookupTable > m_lookUpTable;
    IplImage *m_mask;
    
    bool m_initTeleop;
    Teleoperation2DWidget *m_teleoperationWidget;
    ControlAlgorithms *m_control;

  public:
    ImageViewer2DTeleoperationWidget(QWidget *parent);
    virtual ~ImageViewer2DTeleoperationWidget();
    void SetImage(const RPData *rp);
    void DrawTargetIcon(Vec3d t);
    void DrawTipIcon(Vec3d p, Vec3d pz, Vec3d py);
    void SetTargetText(Vec2d px, Vec3d wpt);
    void SetSegmentationText(Matrix33d R, Vec3d p);
    void SetInstructionText(char* str);
    void SetVarianceText(Vec3d var);
    void SetTeleoperation2DWidget(Teleoperation2DWidget *widget);
    void getImageDim(int &w, int &h);
    void setControl(ControlAlgorithms* control);
    void onUpdateOverlay();
  };

}

#endif // IMAGEVIEWERWIDGET_H
