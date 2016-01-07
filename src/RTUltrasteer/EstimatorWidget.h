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
#include "ParticleFilter.h"
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
    EFS_ESTIMATE_SCAN,
  };

  enum EstimatorResultsAvailable
  {
    ERA_NONE = 0,
    ERA_NEEDLE_TIP_CALIB = 0x1,
    ERA_NEEDLE_CURVATURE_CALIB = 0x2,
    ERA_NEEDLE_MEASUREMENT_POINTS = 0x4,
  };

  struct PFData
  {
    Measurement m;
    NSCommand u;
    TipState t;
    Vec2d mpp;
    TipState est;
    std::vector < arma::mat33 > particleRs;
    arma::mat particlePos;
    arma::mat w;
  };

  class ParticleFilterVisualizer : public ParameterCollection, public Updateable
  {
  protected:
    std::tr1::shared_ptr < Nf::ParticleFilter > m_pf;
    std::tr1::shared_ptr < PointCloudVisualizer > m_pfPoints;
    vtkSmartPointer < vtkAxesActor > m_pfExpectedOrientation;
    std::tr1::shared_ptr < SphereVisualizer > m_pfExpectedPos;
    std::tr1::shared_ptr < NeedleSegmenter > m_segmenter;
    std::map < s32, PFData > m_pfFramesProcessed;
    std::tr1::shared_ptr < PointCloudVisualizer > m_measurementPoints;
    std::tr1::shared_ptr < PFParams > m_pfParams;
    std::tr1::shared_ptr < PFParams > m_pfParamsMarg;
    std::string m_basePath;

    Updateable *m_update;
    s32 m_lastFrame;
    bool m_init;

    std::map < s32, TipState > m_estTipStates;
    std::map < s32, TipState > m_trueTipStates;

  public:
    ParticleFilterVisualizer(Updateable *update);
    ~ParticleFilterVisualizer();
    void AddActorsToRenderer(vtkSmartPointer < vtkRenderer > renderer);
    void SetVisiblity(bool visible);
    virtual void DoSegmentation(RPData *rp, NeedleFrame &doppler, NeedleFrame &bmode);
    virtual void Update(RPData *rp, s32 frame); 
    virtual void Initialize(const char *basePath);
    virtual s32 NumberOfMeasurementsUpToAndIncludingFrame(s32 frame);
    virtual void UpdateVisualizations(s32 frame);
    std::tr1::shared_ptr < PFParams > GetParams(s32 frame);
    std::vector < TipState > AssembleTipHistory(s32 frame);
    std::vector < Measurement > AssembleMeasurements(s32 frame);
    std::vector < Measurement > AssembleAllMeasurements(s32 frame);
    std::vector < NSCommand > AssembleCommands(s32 frame);
    std::vector < NSCommand > AssembleAllCommands(s32 frame);
    arma::vec AssembleDts(s32 frame);
    arma::vec AssembleAllDts(s32 frame);
    void SaveParticleFilterResults(s32 frame, const char *directory);
    void SetBasePath(const char *basePath) { m_basePath = std::string(basePath); }

    //Parameters
    std::tr1::shared_ptr < Nf::FloatParameter > m_roc;

    std::tr1::shared_ptr < Nf::BoolParameter > m_clearEstimatorData;
    void onClearEstimatorData();
    CLASS_CALLBACK(onClearEstimatorData, ParticleFilterVisualizer);

    std::tr1::shared_ptr < Nf::IntParameter > m_nParticles;
    void onNumParticlesChanged();
    CLASS_CALLBACK(onNumParticlesChanged, ParticleFilterVisualizer);

    std::tr1::shared_ptr < Nf::EnumParameter > m_pfMethod;
    void onPFMethodChanged();
    CLASS_CALLBACK(onPFMethodChanged, ParticleFilterVisualizer);

    std::tr1::shared_ptr < Nf::IntParameter > m_nVisSkip;
    void onNVisSkipChanged();
    CLASS_CALLBACK(onNVisSkipChanged, ParticleFilterVisualizer);

    std::tr1::shared_ptr < Nf::BoolParameter > m_showParticlePos;
    std::tr1::shared_ptr < Nf::BoolParameter > m_showExpectedPos;
    std::tr1::shared_ptr < Nf::BoolParameter > m_showExpectedOrientation;
    std::tr1::shared_ptr < Nf::BoolParameter > m_showMeasurements;
    void onVisibilityChanged();
    CLASS_CALLBACK(onVisibilityChanged, ParticleFilterVisualizer);

    std::tr1::shared_ptr < Nf::BoolParameter > m_collectMeasurements;

    virtual void onUpdate();
    virtual void onRepaint();
  };

  class EstimatorFileWidget : public RPFileWidget, public Updateable
  {
    Q_OBJECT 

  protected:
    EstimatorFileState m_state;
    u32 m_resultsAvailable;
    std::tr1::shared_ptr < ParticleFilterVisualizer > m_pfVisualizer;
    std::tr1::shared_ptr < PointCloudVisualizer > m_calibrationPointsTip;
    std::tr1::shared_ptr < PointCloudVisualizer > m_calibrationPointsCurvature;

    s32 m_lastFrame;
    
    EMNeedleTipMultiCalibrator m_ntCalibrator;
    NeedleCurvatureCalibrator m_ncCalibrator;

    std::tr1::shared_ptr < SphereVisualizer > m_calibTip;
    vtkSmartPointer < vtkAxesActor > m_calibTipFrame;
    std::tr1::shared_ptr < ScreenWriter > m_screenWriter;

  public:
    EstimatorFileWidget(QWidget *parent, const char *name = "Estimator File Widget");
    virtual ~EstimatorFileWidget();

    std::tr1::shared_ptr < Nf::FileParameter > m_tipCalibPath;

    std::tr1::shared_ptr < Nf::FileParameter > m_pointsDataPath;

    std::tr1::shared_ptr < Nf::BoolParameter > m_addNewFrame;
    
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

    //Do Calibration
    std::tr1::shared_ptr < Nf::BoolParameter > m_doNeedleCalib;
    void onDoNeedleCalibrationPushed();
    CLASS_CALLBACK(onDoNeedleCalibrationPushed, EstimatorFileWidget);

    std::tr1::shared_ptr < Nf::BoolParameter > m_clearCalibrationData;
    void onClearCalibrationData();
    CLASS_CALLBACK(onClearCalibrationData, EstimatorFileWidget);

    std::tr1::shared_ptr < Nf::BoolParameter > m_clearTipCalibration;
    void onClearTipCalibration();
    CLASS_CALLBACK(onClearTipCalibration, EstimatorFileWidget);

    virtual void onUpdate();
    virtual void onRepaint();

    virtual void onUpdateFile();
    virtual void UpdateCalibTipVis();

    virtual void onPointPushed(Vec2d point);

    signals:
    void updateFrame();

    public slots:
    virtual void onUpdateFrame();



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

  public:
    EstimatorStreamingWidget(QWidget *parent, const char *name = "Estimator Streaming Widget");
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
