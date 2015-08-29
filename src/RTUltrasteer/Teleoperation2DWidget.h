#ifndef TELEOPERATION2DWIDGET_H
#define TELEOPERATION2DWIDGET_H

#include "UICore.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QElapsedTimer>
#include <QPushButton>
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "RPFileReader.h"
#include "RPUlterius.h"
#include "RPWidget.h"
#include "ImageViewerWidget.h"
#include <QtDebug>
#include "NeedleSteeringRobot.h"
#include "ControlAlgorithms.h"
#include "TeleoperationVisualizationWidget.h"

namespace Nf
{
  class Teleoperation2DWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < Nf::EnumParameter > m_transducerType;
    void onSetTransducerType();
    CLASS_CALLBACK(onSetTransducerType, Teleoperation2DWidget);

    std::tr1::shared_ptr <ImageViewer2DTeleoperationWidget> m_imageViewer;
    std::tr1::shared_ptr <TeleoperationVisualizationWidget> m_teleoperationVisualizer;
    QGridLayout *m_layout;
    QVBoxLayout *m_rightSubLayout;
    QHBoxLayout *m_leftSubLayout;
    QPushButton *m_scanButton;
    QPushButton *m_teleoperateButton;
    std::tr1::shared_ptr < QTimer > m_preScanTimer;
    QElapsedTimer m_scanTimer;
    
    NeedleSteeringRobot* m_robot;
    ControlAlgorithms* m_control;
    RPData m_data;
    Matrix44d m_Tref2robot;

  public:
    Teleoperation2DWidget(QWidget *parent, NeedleSteeringRobot* robot, ControlAlgorithms* control);
    virtual ~Teleoperation2DWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();
    std::vector < QVTKWidget * > GetChildWidgets();
    
    void displayScanTimeRemaining();
    void checkCalibrations();
    Matrix44d loadRobotEmCalibration();
    void updateTeleoperationVisualization();

  public slots:
    void onStartManualNeedleScan();
    void onStartStopTeleoperation();
    void onManualTimeout();
  };

  class Teleoperation2DFileWidget : public Teleoperation2DWidget
  {
    Q_OBJECT 

  protected:
    RPFileReaderCollection *m_rpReaders;
    QMutex m_lock;

  public:
    Teleoperation2DFileWidget(QWidget *parent, NeedleSteeringRobot* robot, ControlAlgorithms* control);
    virtual ~Teleoperation2DFileWidget();

    //Filename
    std::tr1::shared_ptr < Nf::FileParameter > m_rpFile;
    void onUpdateFile();
    CLASS_CALLBACK(onUpdateFile, Teleoperation2DFileWidget);

    //Frame index
    std::tr1::shared_ptr < Nf::IntParameter > m_frame;
    CLASS_CALLBACK(onUpdateFrame, Teleoperation2DFileWidget);
    void onUpdateFrame();    

  };

  class Teleoperation2DStreamingWidget : public Teleoperation2DWidget, public RPFrameHandler
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < RPUlteriusReaderCollectionPush > m_rpReaders;
    RPPushReceiver *m_receiver;

  public:
    Teleoperation2DStreamingWidget(QWidget *parent, NeedleSteeringRobot* robot, ControlAlgorithms* control);
    virtual ~Teleoperation2DStreamingWidget();

    //IP
    std::tr1::shared_ptr < Nf::StringParameter > m_rpIp;

    //Stream
    std::tr1::shared_ptr < Nf::BoolParameter > m_init;
    void onInitializeToggle();
    CLASS_CALLBACK(onInitializeToggle, Teleoperation2DStreamingWidget);

    //MPP
    std::tr1::shared_ptr < Nf::FloatParameter > m_mpp;

    //Origin
    std::tr1::shared_ptr < Nf::Vec2dParameter > m_origin;

    virtual void HandleFrame(RPData &rp);

  };
}

#endif //TELEOPERATION2DWIDGET_H