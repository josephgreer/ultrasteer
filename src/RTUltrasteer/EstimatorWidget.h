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
#include "Segment.h"
#include <vtkAxesActor.h>

namespace Nf
{

  enum EstimatorFileState {
    EFS_READY = 0,
    EFS_PRIMED,
    EFS_NEEDLE_TIP_CALIB,
    EFS_NEEDLE_CURVATURE_CALIB_GPS,
    EFS_NEEDLE_CURVATURE_CALIB_US,
    EFS_ESTIMATE,
  };

  enum EstimatorResultsAvailable
  {
    ERA_NONE = 0,
    ERA_NEEDLE_TIP_CALIB = 0x1,
    ERA_NEEDLE_CURVATURE_CALIB = 0x2,
    ERA_NEEDLE_MEASUREMENT_POINTS = 0x4,
  };

  class EstimatorFileWidget : public RPFileWidget, public Updateable
  {
    Q_OBJECT 

  protected:
    EstimatorFileState m_state;
    u32 m_resultsAvailable;
    std::tr1::shared_ptr < PointCloudVisualizer > m_calibrationPointsTip;
    std::tr1::shared_ptr < PointCloudVisualizer > m_calibrationPointsCurvature;
    std::tr1::shared_ptr < PointCloudVisualizer > m_measurementPoints;
    
    EMNeedleTipCalibrator m_ntCalibrator;
    NeedleCurvatureCalibrator m_ncCalibrator;

    std::tr1::shared_ptr < SphereVisualizer > m_calibTip;
    vtkSmartPointer < vtkAxesActor > m_calibTipFrame;

    std::tr1::shared_ptr < NeedleSegmenter > m_segmenter;

  public:
    EstimatorFileWidget(QWidget *parent);
    virtual ~EstimatorFileWidget();

    std::tr1::shared_ptr < Nf::FileParameter > m_tipCalibPath;

    std::tr1::shared_ptr < Nf::FileParameter > m_pointsDataPath;
    
    std::tr1::shared_ptr < Nf::FileParameter > m_tipCalibPathLoad;
    virtual void onTipCalibPathChanged();
    CLASS_CALLBACK(onTipCalibPathChanged, EstimatorFileWidget);
    
    std::tr1::shared_ptr < Nf::FileParameter > m_pointsDataPathLoad;
    virtual void onPointsDataPathChanged();
    CLASS_CALLBACK(onPointsDataPathChanged, EstimatorFileWidget);
    
    //CalibrationMode
    std::tr1::shared_ptr < Nf::EnumParameter > m_operationMode;
    virtual void onSetOperationMode();
    CLASS_CALLBACK(onSetOperationMode, EstimatorFileWidget);

    std::tr1::shared_ptr < Nf::BoolParameter > m_collectMeasurements;

    //Do Calibration
    std::tr1::shared_ptr < Nf::BoolParameter > m_doNeedleCalib;
    void onDoNeedleCalibrationPushed();
    CLASS_CALLBACK(onDoNeedleCalibrationPushed, EstimatorFileWidget);

    std::tr1::shared_ptr < Nf::BoolParameter > m_clearCalibrationData;
    void onClearCalibrationData();
    CLASS_CALLBACK(onClearCalibrationData, EstimatorFileWidget);

    std::tr1::shared_ptr < Nf::BoolParameter > m_clearEstimatorData;
    void onClearEstimatorData();
    CLASS_CALLBACK(onClearEstimatorData, EstimatorFileWidget);

    std::tr1::shared_ptr < Nf::BoolParameter > m_clearTipCalibration;
    void onClearTipCalibration();
    CLASS_CALLBACK(onClearTipCalibration, EstimatorFileWidget);

    virtual void onUpdate();

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
    std::tr1::shared_ptr < PointCloudVisualizer > m_tpHistory;

    NeedleSteeringRobot *m_robot;
    EstimatorState m_state;      
    NSCommand m_u; 
    bool m_executeCommand;
    virtual void ExecuteCommand();
    EMNeedleTipCalibrator m_ntCalibrator;

    std::vector < Vec3d > m_pastTipPoints;

    std::tr1::shared_ptr < NeedleSegmenter > m_segmenter;

  public:
    EstimatorStreamingWidget(QWidget *parent);
    virtual ~EstimatorStreamingWidget();
    virtual void Initialize(bool init);
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

    std::tr1::shared_ptr < Nf::FileParameter > m_tipHistoryPathLoad;
    void onTipHistoryPathLoadChanged();
    CLASS_CALLBACK(onTipHistoryPathLoadChanged, EstimatorStreamingWidget);

    std::tr1::shared_ptr < Nf::FileParameter > m_tipHistoryPathSave;
    void onTipHistoryPathSavedChanged();
    CLASS_CALLBACK(onTipHistoryPathSavedChanged, EstimatorStreamingWidget);

    std::tr1::shared_ptr < Nf::BoolParameter > m_showPastTipPoints;
    std::tr1::shared_ptr < Nf::BoolParameter > m_collectPastTipPoints;

    std::tr1::shared_ptr < Nf::BoolParameter > m_clearPastPoints;
    void onClearPastPoints();
    CLASS_CALLBACK(onClearPastPoints, EstimatorStreamingWidget);

    virtual void HandleExtras();
    virtual void HandleFrame(RPData &rp);

public slots:
    void onSaveDataClicked();
    virtual void onFrame();

  };
}

#endif // ESTIMATORWIDGET_H
