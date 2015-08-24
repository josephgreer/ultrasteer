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
#include "RPProcess.h"
#include "ImageViewerWidget.h"
#include <QtDebug>
#include "NeedleSteeringRobot.h"
#include "ControlAlgorithms.h"


namespace Nf
{
  class Teleoperation2DWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr <ImageViewer2DTeleoperationWidget> m_imageViewer;
    RPData m_data;
    QGridLayout *m_layout;
    QVBoxLayout *m_rightSubLayout;
    QVBoxLayout *m_leftSubLayout;
    QPushButton *m_scanButton;
    QPushButton *m_teleoperateButton;
    NeedleSteeringRobot* m_robot;
    ControlAlgorithms* m_control;
    std::tr1::shared_ptr < QTimer > m_preScanTimer;
    QElapsedTimer m_scanTimer;

  public:
    Teleoperation2DWidget(QWidget *parent, NeedleSteeringRobot* robot, ControlAlgorithms* control);
    virtual ~Teleoperation2DWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();
    //void setRobot(NeedleSteeringRobot* robot);
    //void setControl(ControlAlgorithms* control);
    std::vector < QVTKWidget * > GetChildWidgets();
    void displayScanTimeRemaining();

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

  class Teleoperation2DStreamingWidget : public Teleoperation2DWidget
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < RPUlteriusProcessManager > m_rpReaders;
    std::tr1::shared_ptr < QTimer > m_tick;

  public:
    Teleoperation2DStreamingWidget(QWidget *parent, NeedleSteeringRobot* robot, ControlAlgorithms* control);
    virtual ~Teleoperation2DStreamingWidget();

    //IP
    std::tr1::shared_ptr < Nf::StringParameter > m_rpIp;

    //Stream
    std::tr1::shared_ptr < Nf::BoolParameter > m_init;
    void onInitializeToggle();
    CLASS_CALLBACK(onInitializeToggle, Teleoperation2DStreamingWidget);

    //Add Frames
    std::tr1::shared_ptr < Nf::BoolParameter > m_addFrames;
    void onAddFramesToggle();
    CLASS_CALLBACK(onAddFramesToggle, Teleoperation2DStreamingWidget);

    //Framerate
    std::tr1::shared_ptr < Nf::IntParameter > m_framerate;
    void onFramerateChanged();
    CLASS_CALLBACK(onFramerateChanged, Teleoperation2DStreamingWidget);

    //MPP
    std::tr1::shared_ptr < Nf::FloatParameter > m_mpp;

    //Origin
    std::tr1::shared_ptr < Nf::Vec2dParameter > m_origin;

  public slots:
    void onTick();

  };
}

#endif //TELEOPERATION2DWIDGET_H