#ifndef EMCALIBRATIONWIDGET_H
#define EMCALIBRATIONWIDGET_H

#include "UICore.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QElapsedTimer>
#include <QPushButton>
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "RPUlterius.h"
#include "ImageViewerWidget.h"
#include <QtDebug>
#include "NeedleSteeringRobot.h"
#include "ControlAlgorithms.h"
#include "vtkVertexGlyphFilter.h"
#include <vtkProperty.h>
#include <QtGui/QFileDialog>
#include <vtkAxesActor.h>

namespace Nf
{
  class EMCalibrationWidget : public ResizableQVTKWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:

    //In stylus calibration
    std::tr1::shared_ptr < Nf::BoolParameter > m_inStylusCalibration;
    void onCalibrateStylus();
    CLASS_CALLBACK(onCalibrateStylus, EMCalibrationWidget);

    //In robot calibration
    std::tr1::shared_ptr < Nf::BoolParameter > m_inRobotCalibration;
    void onCalibrateRobot();
    CLASS_CALLBACK(onCalibrateRobot, EMCalibrationWidget);

    //Robot fiducial number
    std::tr1::shared_ptr < Nf::IntParameter > m_robotFiducialNumber;
    void onUpdateFiducial();
    CLASS_CALLBACK(onUpdateFiducial, EMCalibrationWidget);
    
    RPData m_data;
    QGridLayout *m_layout;
    bool m_viewportInit;
    bool m_robotCalibrationComplete;
    vtkSmartPointer < vtkPoints > m_targetPoints;
    vtkSmartPointer < vtkPolyData > m_targetPolyData;
    vtkSmartPointer<vtkVertexGlyphFilter> m_targetGlyphFilter;
    vtkSmartPointer < vtkPolyDataMapper > m_targetMapper;
    vtkSmartPointer < vtkActor > m_targetActor;

    vtkSmartPointer < vtkPoints > m_stylusPoints;
    vtkSmartPointer < vtkPolyData > m_polyData;
    vtkSmartPointer<vtkVertexGlyphFilter> m_glyphFilter;
    vtkSmartPointer < vtkPolyDataMapper > m_mapper;
    vtkSmartPointer < vtkActor > m_actor;
    
    vtkSmartPointer < vtkAxesActor > m_robotAxes;
    vtkSmartPointer < vtkAxesActor > m_EMrobotAxes;
    vtkSmartPointer < vtkAxesActor > m_stylusAxes;

    vtkSmartPointer < vtkRenderer > m_renderer;
    vtkSmartPointer < vtkImageFlip > m_flip;
    vtkSmartPointer < vtkRenderWindowInteractor > m_interactor;
    
    // stylus calibration object
    StylusCalibration *m_stylusCalibration;

    // robot calibration variables
    arma::mat m_fiducialMeasurements;
    arma::mat m_fiducialCoordinates;
    arma::mat m_currentFiducialMeasurements;
    arma::mat m_T_em2robot;

  public:
    EMCalibrationWidget(QWidget *parent);
    virtual ~EMCalibrationWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();
    std::vector < QVTKWidget * > GetChildWidgets();
    void addFrame(RPData &rp);
    void initializeStylusCal();
    void resetView();
    void RenderTargetPoints(bool, arma::mat = arma::mat());
    void RenderMeasuredPoints(bool, arma::mat = arma::mat());
    void initViewport();
    void solveRobotRegistration();

  //public slots:
  //  void onStartManualNeedleScan();
  };

  class EMCalibrationFileWidget : public EMCalibrationWidget
  {
    Q_OBJECT 

  protected:
    RPFileReaderCollection *m_rpReaders;

  public:
    EMCalibrationFileWidget(QWidget *parent);
    virtual ~EMCalibrationFileWidget();

    //Filename
    std::tr1::shared_ptr < Nf::FileParameter > m_rpFile;
    void onUpdateFile();
    CLASS_CALLBACK(onUpdateFile, EMCalibrationFileWidget);

    //Frame index
    std::tr1::shared_ptr < Nf::IntParameter > m_frame;
    CLASS_CALLBACK(onUpdateFrame, EMCalibrationFileWidget);
    void onUpdateFrame();    
  };

  class EMCalibrationStreamingWidget : public EMCalibrationWidget, public RPCallbackReceiver
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < RPUlteriusReaderCollectionPush > m_rpReaders;
    QMutex m_lock;

  public:
    EMCalibrationStreamingWidget(QWidget *parent);
    virtual ~EMCalibrationStreamingWidget();

    //IP
    std::tr1::shared_ptr < Nf::StringParameter > m_rpIp;

    //Stream
    std::tr1::shared_ptr < Nf::BoolParameter > m_init;
    void onInitializeToggle();
    CLASS_CALLBACK(onInitializeToggle, EMCalibrationStreamingWidget);

    //Add Frames
    std::tr1::shared_ptr < Nf::BoolParameter > m_addFrames;
    void onAddFramesToggle();
    CLASS_CALLBACK(onAddFramesToggle, EMCalibrationStreamingWidget);

    //Framerate
    std::tr1::shared_ptr < Nf::IntParameter > m_framerate;
    void onFramerateChanged();
    CLASS_CALLBACK(onFramerateChanged, EMCalibrationStreamingWidget);

    //MPP
    std::tr1::shared_ptr < Nf::FloatParameter > m_mpp;

    //Origin
    std::tr1::shared_ptr < Nf::Vec2dParameter > m_origin;

    virtual void Callback(const RPData *rp);

    public slots:
      virtual void onFrame();

  signals:
    void frameSignal();

  };

}

 

#endif //EMCALIBRATIONWIDGET_H