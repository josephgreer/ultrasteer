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
#include "RPWidget.h"

namespace Nf
{
  class EMCalibrationWidget : public ResizableQVTKWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:

    //In stylus calibration
    std::tr1::shared_ptr < Nf::BoolParameter > m_inStylusCalibration;
    virtual void onCalibrateStylus();
    CLASS_CALLBACK(onCalibrateStylus, EMCalibrationWidget);

    //In robot calibration
    std::tr1::shared_ptr < Nf::BoolParameter > m_inRobotCalibration;
    virtual void onCalibrateRobot();
    CLASS_CALLBACK(onCalibrateRobot, EMCalibrationWidget);

    //In stylus point selection
    std::tr1::shared_ptr < Nf::BoolParameter > m_inStylusPointSelection;
    void onStylusPointSelection();
    CLASS_CALLBACK(onStylusPointSelection, EMCalibrationWidget);

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
    vtkSmartPointer < vtkRenderWindowInteractor > m_interactor;
    vtkSmartPointer<vtkTextActor> m_textActor;
    
    // stylus calibration object
    StylusCalibration *m_stylusCalibration;

    // robot calibration variables
    arma::mat m_fiducialMeasurementsInFref;
    arma::mat m_fiducialsInFrobot;
    arma::mat m_currentFiducialMeasurements;
    arma::mat m_Tref2robot;

  public:
    EMCalibrationWidget(QWidget *parent, const char *name = "EMCalibration");
    virtual ~EMCalibrationWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();
    std::vector < QVTKWidget * > GetChildWidgets();
    virtual void addFrame(RPData rp);
    void initializeStylusCal();
    void resetView();
    void RenderTargetPoints(bool, arma::mat = arma::mat());
    void RenderMeasuredPoints(bool, arma::mat = arma::mat());
    void initViewport();
    void solveRobotRegistration();
    vtkSmartPointer < vtkRenderer > GetRenderer() { return m_renderer; }
    void SetRobotCalibration(arma::mat frame);
    void SetStylusCalibrationOffset(arma::vec offset);
    bool RobotCalibrationComplete() { return m_robotCalibrationComplete; }
    arma::mat GetRobotCalibration() { return m_Tref2robot; }

  //public slots:
  //  void onStartManualNeedleScan();
  };

  class EMCalibrationFileWidget : public EMCalibrationWidget
  {
    Q_OBJECT 

  protected:
    RPFileReaderCollection *m_rpReaders;

  public:
    EMCalibrationFileWidget(QWidget *parent, const char *name = "EMCalibrationFile");
    virtual ~EMCalibrationFileWidget();

    //Filename
    std::tr1::shared_ptr < Nf::FileParameter > m_rpFile;
    void onUpdateFile();
    CLASS_CALLBACK(onUpdateFile, EMCalibrationFileWidget);

    //Frame index
    std::tr1::shared_ptr < Nf::IntParameter > m_frame;
    CLASS_CALLBACK(onUpdateFrame, EMCalibrationFileWidget);
    virtual void onUpdateFrame();    
  };

  class EMCalibrationStreamingWidget : public EMCalibrationWidget, public RPFrameHandler
  {
    Q_OBJECT 

  protected:
    RPPushReceiver *m_receiver;

  public:
    EMCalibrationStreamingWidget(QWidget *parent, const char *name = "EMCalibrationStreamingWidget");
    virtual ~EMCalibrationStreamingWidget();

    virtual void HandleFrame(RPData &rp);
  };

}

 

#endif //EMCALIBRATIONWIDGET_H