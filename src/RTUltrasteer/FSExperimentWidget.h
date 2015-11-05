#pragma once
#include "UICore.h"
#include "EMCalibrationWidget.h"
#include "RobotHardwareWidget.h"
#include "SaveDataWidget.h"
#include "cForceSensor.h"
namespace Nf
{
  class ExperimentCalibrationData : public ParameterCollection
  {
  protected: 
    arma::mat m_puncturePlanePoints;  // corner points of puncture plane [x1 x2 x3 x4] \in R^{3x4}
    arma::mat m_needlePoints;         // needle points in tracker coordinate system [x1 ... xm] \in R^{3xm}

    
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

    std::tr1::shared_ptr < Nf::BoolParameter > m_clearNeedlePoints;
    void onClearNeedlePoints();
    CLASS_CALLBACK(onClearNeedlePoints, ExperimentCalibrationData);

    std::tr1::shared_ptr < Nf::BoolParameter > m_determineNeedleOffset;
    void onDetermineNeedleOffset();
    CLASS_CALLBACK(onDetermineNeedleOffset, ExperimentCalibrationData);

    std::tr1::shared_ptr < Nf::BoolParameter > m_saveCalibrationData;
    void onSaveCalibrationData();
    CLASS_CALLBACK(onSaveCalibrationData, ExperimentCalibrationData);

    std::tr1::shared_ptr < Nf::FileParameter > m_calibSavePath;
  };

  class FSExperimentCalibrationFileWidget : public EMCalibrationFileWidget
  {
    Q_OBJECT 

  public:
    FSExperimentCalibrationFileWidget(QWidget *parent);
    virtual ~FSExperimentCalibrationFileWidget();
    virtual void addFrame(RPData &rp);
  };

  class FSExperimentCalibrationStreamingWidget : public EMCalibrationStreamingWidget
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < cForceSensor > m_forceSensor;

  public:
    FSExperimentCalibrationStreamingWidget(QWidget *parent);
    virtual ~FSExperimentCalibrationStreamingWidget();
    virtual void addFrame(RPData &rp);
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
    std::tr1::shared_ptr < ExperimentCalibrationData > m_experimentCalib;
    NeedleSteeringRobot *m_robot;


  public:
    FSExperimentStreamingWidget(QWidget *parent);
    virtual ~FSExperimentStreamingWidget();

    virtual void SetRobot(NeedleSteeringRobot *robot);
    std::vector < QVTKWidget * > GetChildWidgets();
    virtual void UpdateSize(QSize sz);

  };

}