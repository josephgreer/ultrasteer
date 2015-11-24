#pragma once
#include "UICore.h"
#include "EMCalibrationWidget.h"
#include "RobotHardwareWidget.h"
#include "SaveDataWidget.h"
#include "Calibration.h"
#include "cForceSensor.h"
#include <vtkAxesActor.h>
#include <vtkTextActor.h>
namespace Nf
{
  class ExperimentCalibrationData : public ParameterCollection
  {
  protected: 
    std::tr1::shared_ptr < PlaneCalibrator > m_planeCalibration;
    std::tr1::shared_ptr < EMNeedleTipCalibrator > m_tipCalibration;
    std::tr1::shared_ptr < EMNeedleTipCalibrator > m_stylusCalibration;

    arma::mat m_baseFrame;
    arma::vec m_baseOffset;
    arma::mat m_corners;
    arma::mat m_lastGPS2;

  public:
    ExperimentCalibrationData();
    ~ExperimentCalibrationData();

    //Parameters
    std::tr1::shared_ptr < Nf::BoolParameter > m_addCornerPoints;
    void onAddCornerPoints();
    CLASS_CALLBACK(onAddCornerPoints, ExperimentCalibrationData);

    std::tr1::shared_ptr < Nf::BoolParameter > m_clearCornerPoints;
    void onClearCornerPoints();
    CLASS_CALLBACK(onClearCornerPoints, ExperimentCalibrationData);

    std::tr1::shared_ptr < Nf::BoolParameter > m_determinePuncturePlane;
    void onDeterminePuncturePlane();
    CLASS_CALLBACK(onDeterminePuncturePlane, ExperimentCalibrationData);

    std::tr1::shared_ptr < Nf::BoolParameter > m_addNeedlePoints;
    void onAddNeedlePoints();
    CLASS_CALLBACK(onAddNeedlePoints, ExperimentCalibrationData);

    std::tr1::shared_ptr < Nf::IntParameter > m_cornerNumber;

    std::tr1::shared_ptr < Nf::BoolParameter > m_clearNeedlePoints;
    void onClearNeedlePoints();
    CLASS_CALLBACK(onClearNeedlePoints, ExperimentCalibrationData);

    std::tr1::shared_ptr < Nf::BoolParameter > m_determineNeedleOffset;
    void onDetermineNeedleOffset();
    CLASS_CALLBACK(onDetermineNeedleOffset, ExperimentCalibrationData);
    
    std::tr1::shared_ptr < Nf::BoolParameter > m_saveCalibrationData;
    void SaveCalibrationData();
    CLASS_CALLBACK(SaveCalibrationData, ExperimentCalibrationData);

    void addFrame(RPData rp);
    void SetBaseFrameData(arma::mat baseFrame, arma::mat baseOffset);
    void SetStylusCalibration(std::tr1::shared_ptr < EMNeedleTipCalibrator > stylusCalib);
    void SetPlaneCalibration(std::tr1::shared_ptr < PlaneCalibrator > calib) { m_planeCalibration = calib; }
    void SetTipCalibration(std::tr1::shared_ptr < EMNeedleTipCalibrator > tipCalib) { m_tipCalibration = tipCalib; }
    void SetCorners(arma::mat corners) { m_corners = corners; }
    void SetLastGPS2(arma::mat gps2) { m_lastGPS2 = gps2; }
    std::tr1::shared_ptr < PlaneCalibrator > GetPlaneCalibration() { return m_planeCalibration; }
    std::tr1::shared_ptr < EMNeedleTipCalibrator > GetTipCalibration() { return m_tipCalibration; }
    arma::mat GetCorners() { return m_corners; }
  };

  class FSExperimentVisualization : public ParameterCollection
  {
  protected:
    std::tr1::shared_ptr < PointCloudVisualizer > m_cornerPointVis;
    std::tr1::shared_ptr < PlaneVisualizer > m_planeVis;
    vtkSmartPointer < vtkAxesActor > m_planeAxis;
    std::tr1::shared_ptr < SphereVisualizer > m_needleTipVis;
    std::tr1::shared_ptr < STLVisualizer > m_robotVis;
    vtkSmartPointer < vtkTextActor > m_forceText;
    std::tr1::shared_ptr < ExperimentCalibrationData > m_experimentCalib;
    EMCalibrationWidget *m_em;
    arma::mat m_lastGPS2;

  public:
    FSExperimentVisualization(EMCalibrationWidget *em, std::tr1::shared_ptr < ExperimentCalibrationData > experimentCalib);
    ~FSExperimentVisualization();

    RPData ResetDataRelativeToRobotFrame(const RPData &rp);

    void addFrame(RPData rp);
    
    std::tr1::shared_ptr < Nf::BoolParameter > m_loadCalib;
    void onLoadCalib();
    CLASS_CALLBACK(onLoadCalib, FSExperimentVisualization);
    
    std::tr1::shared_ptr < Nf::BoolParameter > m_alignCamToPlane;
    void onAlignCamToPlane();
    CLASS_CALLBACK(onAlignCamToPlane, FSExperimentVisualization);

  };

  class FSExperimentCalibrationFileWidget : public EMCalibrationFileWidget
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < ExperimentCalibrationData > m_experimentCalib;
    std::tr1::shared_ptr < FSExperimentVisualization > m_experimentVis;

  public:
    FSExperimentCalibrationFileWidget(QWidget *parent);
    virtual ~FSExperimentCalibrationFileWidget();
    virtual void addFrame(RPData rp);
  };

  class FSExperimentCalibrationStreamingWidget : public EMCalibrationStreamingWidget, public SphereContainer
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < cForceSensor > m_forceSensor;
    std::tr1::shared_ptr < ExperimentCalibrationData > m_experimentCalib;
    std::tr1::shared_ptr < SaveDataWidget > m_saveDataWidget;
    std::tr1::shared_ptr < RobotHardwareWidget > m_hwWidget;
    std::tr1::shared_ptr < FSExperimentVisualization > m_experimentVis;
    bool m_isInserting;

  public:
    FSExperimentCalibrationStreamingWidget(QWidget *parent);
    virtual ~FSExperimentCalibrationStreamingWidget();
    virtual void addFrame(RPData rp);
    virtual void SetSaveDataWidget(std::tr1::shared_ptr < SaveDataWidget > saveDataWidget) { m_saveDataWidget = saveDataWidget; }
    virtual void onCalibrateRobot();
    virtual void onCalibrateStylus();
    virtual void SetRobotHW(std::tr1::shared_ptr < RobotHardwareWidget > hwWidget) { m_hwWidget = hwWidget; }

    std::tr1::shared_ptr < Nf::IntParameter > m_insVel;
    std::tr1::shared_ptr < Nf::BoolParameter > m_insertNeedle;
    void onInsertNeedle();
    CLASS_CALLBACK(onInsertNeedle, FSExperimentCalibrationStreamingWidget);
    
    std::tr1::shared_ptr < Nf::BoolParameter > m_forceSensorInitialized;
    void onInitializeForceSensor();
    CLASS_CALLBACK(onInitializeForceSensor, FSExperimentCalibrationStreamingWidget);

    std::tr1::shared_ptr < Nf::FloatParameter > m_maxInsertion;
    std::tr1::shared_ptr < Nf::IntParameter > m_numberInsertionsLeft;
    std::tr1::shared_ptr < Nf::FileParameter > m_saveBaseName;

    
    std::tr1::shared_ptr < Nf::BoolParameter > m_zeroForceSensor;
    void onZeroForceSensor();
    CLASS_CALLBACK(onZeroForceSensor, FSExperimentCalibrationStreamingWidget);
  };

  class FSExperimentFileWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT

  protected:
    std::tr1::shared_ptr < FSExperimentCalibrationFileWidget > m_calibWidget;
    std::tr1::shared_ptr < QGridLayout > m_layout;

  public:
    FSExperimentFileWidget(QWidget *parent);
    virtual ~FSExperimentFileWidget();
    std::vector < QVTKWidget * > GetChildWidgets();
    virtual void UpdateSize(QSize sz);
  };

  class FSExperimentStreamingWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT

  protected:
    std::tr1::shared_ptr < RobotHardwareWidget > m_hwWidget;
    std::tr1::shared_ptr < SaveDataWidget > m_saveDataWidget;
    std::tr1::shared_ptr < QGridLayout > m_bottomRow;
    std::tr1::shared_ptr < QGridLayout > m_layout;
    std::tr1::shared_ptr < FSExperimentCalibrationStreamingWidget > m_calibWidget;
    NeedleSteeringRobot *m_robot;

  public:
    FSExperimentStreamingWidget(QWidget *parent);
    virtual ~FSExperimentStreamingWidget();

    virtual void SetRobot(NeedleSteeringRobot *robot);
    std::vector < QVTKWidget * > GetChildWidgets();
    virtual void UpdateSize(QSize sz);
  };

}