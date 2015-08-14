#ifndef EMCALIBRATIONWIDGET_H
#define EMCALIBRATIONWIDGET_H

#include "UICore.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QElapsedTimer>
#include <QPushButton>
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "RPFileReader.h"
#include "RPProcess.h"
#include "ImageViewerWidget.h"
#include <QtDebug>
#include "NeedleSteeringRobot.h"
#include "ControlAlgorithms.h"
#include "vtkVertexGlyphFilter.h"
#include <vtkProperty.h>

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

    RPData m_data;
    QGridLayout *m_layout;
    bool m_stylusInit;
    bool m_inRobotCalibration;
    vtkSmartPointer < vtkPoints > m_stylusPoints;
    vtkSmartPointer < vtkPolyData > m_polyData;
    vtkSmartPointer<vtkVertexGlyphFilter> m_glyphFilter;
    vtkSmartPointer < vtkPolyDataMapper > m_mapper;
    vtkSmartPointer < vtkActor > m_actor;
    vtkSmartPointer < vtkRenderer > m_renderer;
    vtkSmartPointer < vtkImageFlip > m_flip;
    vtkSmartPointer < vtkRenderWindowInteractor > m_interactor;
    
    arma::mat A;
    arma::mat b;

  public:
    EMCalibrationWidget(QWidget *parent);
    virtual ~EMCalibrationWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();
    std::vector < QVTKWidget * > GetChildWidgets();
    void addFrame();
    void initializeStylusCal();
    void resetView();

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

  class EMCalibrationStreamingWidget : public EMCalibrationWidget
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < RPUlteriusProcessManager > m_rpReaders;
    std::tr1::shared_ptr < QTimer > m_tick;

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

    public slots:
      void onTick();

  };

}

 

#endif //EMCALIBRATIONWIDGET_H