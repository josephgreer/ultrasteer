#include "FSExperimentWidget.h"

namespace Nf
{
  ////////////////////////////////////////////////////////////////////////////////
  /// BEGIN ExperimentCalibrationData CLASS
  ////////////////////////////////////////////////////////////////////////////////
  ExperimentCalibrationData::ExperimentCalibrationData()
    : ParameterCollection("ExperimentCalibrationData")
  {
    ADD_ACTION_PARAMETER(m_addCornerPoints, "Add Corner Points", CALLBACK_POINTER(onAddCornerPoints, ExperimentCalibrationData), this, false);
    ADD_ACTION_PARAMETER(m_clearCornerPoints, "Clear Corner Points", CALLBACK_POINTER(onClearCornerPoints, ExperimentCalibrationData), this, false);
    ADD_ACTION_PARAMETER(m_determinePuncturePlane, "Calibrate Puncture Plane", CALLBACK_POINTER(onDeterminePuncturePlane, ExperimentCalibrationData), this, false);
    ADD_ACTION_PARAMETER(m_addNeedlePoints, "Add Needle Points", CALLBACK_POINTER(onAddNeedlePoints, ExperimentCalibrationData), this, false);
    ADD_ACTION_PARAMETER(m_clearNeedlePoints, "Clear Needle Points", CALLBACK_POINTER(onAddNeedlePoints, ExperimentCalibrationData), this, false);
    ADD_ACTION_PARAMETER(m_determineNeedleOffset, "Calibrate Needle Offset", CALLBACK_POINTER(onDetermineNeedleOffset, ExperimentCalibrationData), this, false);
    ADD_ACTION_PARAMETER(m_saveCalibrationData, "Save Calibration Data", CALLBACK_POINTER(onSaveCalibrationData, ExperimentCalibrationData), this, false);
    ADD_SAVE_FILE_PARAMETER(m_calibSavePath, "Calib Save Path", NULL, this, PATH_CAT("FSCal.m"), "*.m");
  }

  ExperimentCalibrationData::~ExperimentCalibrationData()
  {

  }
  
  void ExperimentCalibrationData::onAddCornerPoints()
  {
  }

  void ExperimentCalibrationData::onClearCornerPoints()
  {
  }

  void ExperimentCalibrationData::onDeterminePuncturePlane()
  {
  }

  void ExperimentCalibrationData::onAddNeedlePoints()
  {
  }

  void ExperimentCalibrationData::onClearNeedlePoints()
  {
  }

  void ExperimentCalibrationData::onDetermineNeedleOffset()
  {
  }

  void ExperimentCalibrationData::onSaveCalibrationData()
  {
  }
  ////////////////////////////////////////////////////////////////////////////////
  /// END ExperimentCalibrationData CLASS
  ////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////
  /// BEGIN FSExperimentCalibrationFILEWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////
  FSExperimentCalibrationFileWidget::FSExperimentCalibrationFileWidget(QWidget *parent) 
    : EMCalibrationFileWidget(parent, "FSExperimentCalibrationFile")
  {
  }

  FSExperimentCalibrationFileWidget::~FSExperimentCalibrationFileWidget()
  {
  }
  ////////////////////////////////////////////////////////////////////////////////
  /// END FSExperimentCalibrationFILEWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////

  
  ////////////////////////////////////////////////////////////////////////////////
  /// BEGIN FSExperimentCalibrationFILEWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////
  FSExperimentCalibrationStreamingWidget::FSExperimentCalibrationStreamingWidget(QWidget *parent)
    : EMCalibrationStreamingWidget(parent, "FSExperimentCalibrationFile") 
    , m_forceSensor(std::tr1::shared_ptr < cForceSensor > (new cForceSensor()))
  {
  }

  FSExperimentCalibrationStreamingWidget::~FSExperimentCalibrationStreamingWidget()
  {
  }
  ////////////////////////////////////////////////////////////////////////////////
  /// END FSExperimentCalibrationFILEWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////

  
  ////////////////////////////////////////////////////////////////////////////////
  /// BEGIN FSExperimentFileWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////
  FSExperimentFileWidget::FSExperimentFileWidget(QWidget *parent)
    : ParameterCollection("FSExperimentWidget")
    , m_layout(new QGridLayout(parent))
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
    , m_calibWidget(new FSExperimentCalibrationFileWidget(parent))
  {
    ADD_CHILD_COLLECTION((ParameterCollection *)m_calibWidget.get());
    m_layout->addWidget(m_calibWidget.get(), 0, 0);

    this->setLayout(m_layout.get());
  }

  FSExperimentFileWidget::~FSExperimentFileWidget()
  {
  }

  void FSExperimentFileWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
    m_calibWidget->UpdateSize(QSize(sz.width()-10, sz.height()));
  }
  
  std::vector < QVTKWidget * > FSExperimentFileWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    res.push_back(m_calibWidget.get());
    return res;
  }
  ////////////////////////////////////////////////////////////////////////////////
  /// END FSExperimentFileWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////

  
  ////////////////////////////////////////////////////////////////////////////////
  /// BEGIN FSExperimentStreamingWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////
  FSExperimentStreamingWidget::FSExperimentStreamingWidget(QWidget *parent)
    : m_hwWidget(new RobotHardwareWidget(parent))
    , ParameterCollection("FSExperimentStreamingWidget")
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
    , m_calibWidget(new FSExperimentCalibrationStreamingWidget(parent))
    , m_experimentCalib(new ExperimentCalibrationData())
    , m_saveDataWidget(new SaveDataWidget(parent))
    , m_layout(new QGridLayout(parent))
    , m_bottomRow(new QGridLayout(parent))
    , m_robot(NULL)
  {
    ADD_CHILD_COLLECTION((ParameterCollection *)m_experimentCalib.get());
    ADD_CHILD_COLLECTION((ParameterCollection *)m_calibWidget.get());
    ADD_CHILD_COLLECTION((ParameterCollection *)m_hwWidget.get());

    m_layout->addWidget(m_calibWidget.get(), 0, 0);
    
    m_layout->setContentsMargins(0,0,5,0);
    this->setLayout(m_layout.get());

    m_bottomRow->addWidget(m_hwWidget.get(), 0, 0);
    m_bottomRow->addWidget(m_saveDataWidget.get(), 0, 1);
    m_layout->addLayout(m_bottomRow.get(), 1, 0, 1, 2);
  }

  void FSExperimentStreamingWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
    m_calibWidget->UpdateSize(QSize(sz.width()-10, (3*sz.height())/4));
    m_hwWidget->UpdateSize(QSize(3*sz.width()/4-10, (sz.height())/4));
    m_saveDataWidget->UpdateSize(QSize(sz.width()/4-10, (sz.height())/4));
  }

  FSExperimentStreamingWidget::~FSExperimentStreamingWidget()
  {
    this->setLayout(NULL);
    m_bottomRow = NULL;
    m_layout = NULL;
  }

  void FSExperimentStreamingWidget::SetRobot(NeedleSteeringRobot *robot)
  {
    m_robot = robot;
    m_hwWidget->setRobot(robot);
  }
  

  std::vector < QVTKWidget * > FSExperimentStreamingWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    res.push_back(m_calibWidget.get());
    return res;
  }
  ////////////////////////////////////////////////////////////////////////////////
  /// END FSExperimentStreamingWidget CLASS
  ////////////////////////////////////////////////////////////////////////////////
}