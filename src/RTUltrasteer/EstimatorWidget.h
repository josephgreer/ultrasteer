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

namespace Nf
{

  enum EstimatorFileState {
    EFS_READY = 0,
    EFS_PRIMED,
    EFS_NEEDLE_CALIB,
  };

  class EstimatorFileWidget : public RPFileWidget
  {
    Q_OBJECT 

  protected:
    EstimatorFileState m_state;

  public:
    EstimatorFileWidget(QWidget *parent);
    virtual ~EstimatorFileWidget();

    std::tr1::shared_ptr < Nf::BoolParameter > m_needleCalib;
    void onNeedleCalibrationPushed();
    CLASS_CALLBACK(onNeedleCalibrationPushed, EstimatorFileWidget);

    virtual void onUpdateFile();

    virtual void onPointPushed(f64 point[]);
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
    Vec3d m_tipOffset;

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

public slots:
    void onSaveDataClicked();
    virtual void onFrame();

  };
}

#endif // ESTIMATORWIDGET_H