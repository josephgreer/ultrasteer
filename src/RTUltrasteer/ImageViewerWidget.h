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
    vtkSmartPointer < vtkRenderWindowInteractor > m_interactor;
    RPData m_rp;
    IplImage *m_temp;
    bool m_init;
    bool m_useTrackball;
    Matrix44d m_cal;        //Calibration Matrix for transducer (maps image coords to world coords)

  public:
    ImageViewerWidget(QWidget *parent, const char *name = "Image Viewer Widget");
    virtual ~ImageViewerWidget();
    virtual void SetImage(const RPData *rp, RP_TYPE type);
    virtual void SetDataSpacing(const RPData *rp);
    virtual vtkSmartPointer < vtkRenderWindowInteractor > GetWindowInteractor() { return m_interactor; }
    virtual vtkSmartPointer < vtkRenderer > GetRenderer() { return m_renderer; }
    vtkSmartPointer <vtkImageData> GetImageData() { return m_importer->GetOutput(); }
    virtual void ResetView();
  };

  
  class Image3DImagePlaneWidget : public ImageViewerWidget, public SphereContainer, public AxisContainer
  {
  public:
    Image3DImagePlaneWidget(QWidget *parent);
    virtual ~Image3DImagePlaneWidget();
    virtual void SetImage(const RPData *rp, RP_TYPE type);

    //update Frame boundaries
    void UpdateFrameBoundaries();

    //showFrameBoundaries
    std::tr1::shared_ptr < Nf::BoolParameter > m_showFrameBoundaries;

    void onShowExtrasChanged();
    CLASS_CALLBACK(onShowExtrasChanged, Image3DImagePlaneWidget);

    //Frame Boundaries
    std::tr1::shared_ptr < Nf::CubeVisualizer > m_frameBoundaries;

    void SetUSVisView(s32 axis1, s32 axis2);
    void SetGPS2VisView(s32 axis1, s32 axis2);

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

    //setViewXY
    std::tr1::shared_ptr < Nf::BoolParameter > m_setViewXYGPS2;
    void onSetViewXYGPS2();
    CLASS_CALLBACK(onSetViewXYGPS2, Image3DImagePlaneWidget);

    //setViewXZ
    std::tr1::shared_ptr < Nf::BoolParameter > m_setViewXZGPS2;
    void onSetViewXZGPS2();
    CLASS_CALLBACK(onSetViewXZGPS2, Image3DImagePlaneWidget);

    //setViewYZ
    std::tr1::shared_ptr < Nf::BoolParameter > m_setViewYZGPS2;
    void onSetViewYZGPS2();
    CLASS_CALLBACK(onSetViewYZGPS2, Image3DImagePlaneWidget);
  };


  class ImageViewer2DTeleoperationWidget : public ImageViewerWidget
  {
  protected:
    vtkSmartPointer < vtkTextActor > m_targetTextActor;
    vtkSmartPointer < vtkTextActor > m_instructionTextActor;
    vtkSmartPointer < vtkTextActor > m_measurementTextActor;
    vtkSmartPointer < vtkTextActor > m_estimateTextActor;
    vtkSmartPointer < vtkPointPicker > m_pointPicker;
    vtkSmartPointer < vtkImageMapToColors > m_mapTransparency;
    vtkSmartPointer < vtkImageImport > m_maskImporter;
    vtkSmartPointer < vtkImageActor > m_maskActor;
    vtkSmartPointer < vtkLookupTable > m_lookUpTable;
    IplImage *m_mask;
    
    bool m_initTeleop;
    bool m_inCountdownToManualScan;
    Teleoperation2DWidget *m_teleoperationWidget;
    ControlAlgorithms *m_control;

  public:
    ImageViewer2DTeleoperationWidget(QWidget *parent, ControlAlgorithms* control, Teleoperation2DWidget* teleop);
    virtual ~ImageViewer2DTeleoperationWidget();
    void SetImage(const RPData *rp, RP_TYPE type);
    void DrawTargetIcon(Vec3d t);
    void DrawTipIcon(Vec3d p, Vec3d pz, Vec3d py);
    void SetTargetText(Vec2d px, Vec3d wpt);
    void SetEstimateText(Matrix44d x, Vec3d Sxyz);
    void SetInstructionText(char* str);
    void SetMeasurementText(Matrix44d z);
    void SetTeleoperation2DWidget(Teleoperation2DWidget *widget);
    void getImageDim(int &w, int &h);
    void setControl(ControlAlgorithms* control);
    void onUpdateOverlay();
    virtual void SetDataSpacing(const RPData *rp);    
    void SetInCountdownToManualScan(bool input);

    // text overlay
    std::tr1::shared_ptr < Nf::BoolParameter > m_showTextOverlay;
    void onShowTextOverlay();
    CLASS_CALLBACK(onShowTextOverlay, ImageViewer2DTeleoperationWidget);
  };
}

#endif // IMAGEVIEWERWIDGET_H
