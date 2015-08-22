#ifndef ESTIMATORWIDGET_H
#define ESTIMATORWIDGET_H

#include "UICore.h"
#include <QGroupBox>
#include <QGridLayout>
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "USVisualizerWidget.h"
#include "RPFileReader.h"
#include "RPProcess.h"
#include "PFVisualizerWidget.h"
#include "RobotHardwareWidget.h"
#include "SaveDataWidget.h"
#include "RPWidget.h"
#include "CubeVisualizer.h"
#include "Calibration.h"
#include <vtkAxesActor.h>

namespace Nf
{

  enum EstimatorFileState {
    EFS_READY = 0,
    EFS_PRIMED,
    EFS_NEEDLE_CALIB,
    EFS_NEEDLE_CALIBRATED,
  };

  class EstimatorFileWidget : public RPFileWidget
  {
    Q_OBJECT 

  protected:
    EstimatorFileState m_state;
    std::tr1::shared_ptr < PointCloudVisualizer > m_calibrationPoints;
    std::tr1::shared_ptr < SphereVisualizer > m_calibTip;
    vtkSmartPointer < vtkAxesActor > m_calibTipFrame;
    EMNeedleTipCalibrator m_ntCalibrator;

  public:
    EstimatorFileWidget(QWidget *parent);
    virtual ~EstimatorFileWidget();

    std::tr1::shared_ptr < Nf::BoolParameter > m_needleCalib;
    void onNeedleCalibrationPushed();
    CLASS_CALLBACK(onNeedleCalibrationPushed, EstimatorFileWidget);

    std::tr1::shared_ptr < Nf::BoolParameter > m_doNeedleCalib;
    void onDoNeedleCalibrationPushed();
    CLASS_CALLBACK(onDoNeedleCalibrationPushed, EstimatorFileWidget);

    std::tr1::shared_ptr < Nf::FileParameter > m_tipCalibPath;

    virtual void onUpdateFile();
    virtual void onUpdateFrame();
    virtual void UpdateCalibTipVis();

    virtual void onPointPushed(Vec2d point);
  };

  enum EstimatorState {
    ES_READY = 0,
    ES_PRIMED,
    ES_RECORDING1,
    ES_RECORDING2,
    ES_PAUSED,
  };

  class EstimatorStreamingWidget : public RPStreamingWidget
  {
    Q_OBJECT 
      
  protected:
    std::tr1::shared_ptr < RobotHardwareWidget > m_hwWidget;
    std::tr1::shared_ptr < SaveDataWidget > m_saveDataWidget;
    std::tr1::shared_ptr < QGridLayout > m_bottomRow;
    NeedleSteeringRobot *m_robot;
    EstimatorState m_state;      
    NSCommand m_u; 
    bool m_executeCommand;
    virtual void ExecuteCommand();
    EMNeedleTipCalibrator m_ntCalibrator;

  public:
    EstimatorStreamingWidget(QWidget *parent);
    virtual ~EstimatorStreamingWidget();
    virtual void onInitializeToggle();
    virtual void SetRobot(NeedleSteeringRobot *robot);

    
    virtual void UpdateSize(QSize sz);

    std::tr1::shared_ptr < Nf::BoolParameter > m_insertion;
    void onInsertionPushed();
    CLASS_CALLBACK(onInsertionPushed, EstimatorStreamingWidget);

    std::tr1::shared_ptr < Nf::BoolParameter > m_pauseInsertion;
    void onPauseInsertionPushed();
    CLASS_CALLBACK(onPauseInsertionPushed, EstimatorStreamingWidget);

    std::tr1::shared_ptr < Nf::BoolParameter > m_saveGPS;
    void onSaveGPS();
    CLASS_CALLBACK(onSaveGPS, EstimatorStreamingWidget);

    std::tr1::shared_ptr < Nf::FileParameter > m_tipCalibPath;
    void onTipCalibPathChanged();
    CLASS_CALLBACK(onTipCalibPathChanged, EstimatorStreamingWidget);

public slots:
    void onSaveDataClicked();
    virtual void onFrame();

  };
}

#endif // ESTIMATORWIDGET_H
