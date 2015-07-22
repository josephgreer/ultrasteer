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
  class EstimatorFileWidget : public RPFileWidget
  {
    Q_OBJECT 

  public:
    EstimatorFileWidget(QWidget *parent);
    virtual ~EstimatorFileWidget();
  };

  struct DataFrame
  {
    RPData rp;
    GPS_Data needleTip;
    NSCommand u;
  };

  enum EstimatorState {
    ES_READY = 0,
    ES_RECORDING,
    ES_ESTIMATING,
  };

  class EstimatorStreamingWidget : public RPStreamingWidget
  {
    Q_OBJECT 
      
  protected:
    std::tr1::shared_ptr < RobotHardwareWidget > m_hwWidget;
    std::tr1::shared_ptr < SaveDataWidget > m_saveDataWidget;
    std::tr1::shared_ptr < QGridLayout > m_bottomRow;
    std::vector < DataFrame > m_dataToSave;
    std::vector < DataFrame > m_unusedData;
    NeedleSteeringRobot *m_robot;
    EstimatorState m_state;      

  public:
    EstimatorStreamingWidget(QWidget *parent);
    virtual ~EstimatorStreamingWidget();
    virtual void onInitializeToggle();
    virtual void SetRobot(NeedleSteeringRobot *robot);

    
    virtual void UpdateSize(QSize sz);

public slots:
    void onSaveDataClicked();

  };
}

#endif // ESTIMATORWIDGET_H
