#ifndef TELEOPERATION2DWIDGET_H
#define TELEOPERATION2DWIDGET_H

#include "UICore.h"
#include <QGroupBox>
#include <QGridLayout>
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "RPFileReader.h"
#include "RPProcess.h"


namespace Nf
{
  class Teleoperation2DWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr <ImageViewer2DTeleoperationWidget> m_imageViewer;
    RPData m_data;
    QGridLayout *m_layout;

  public:
    Teleoperation2DWidget(QWidget *parent);
    virtual ~Teleoperation2DWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();

    std::vector < QVTKWidget * > GetChildWidgets();

  };

  class Teleoperation2DFileWidget : public Teleoperation2DWidget
  {
    Q_OBJECT 

  protected:
    RPFileReaderCollection *m_rpReaders;

  public:
    Teleoperation2DFileWidget(QWidget *parent);
    virtual ~Teleoperation2DFileWidget();

    //Filename
    std::tr1::shared_ptr < Nf::FileParameter > m_rpFile;
    void onUpdateFile();
    CLASS_CALLBACK(onUpdateFile, Teleoperation2DFileWidget);

    //Frame index
    std::tr1::shared_ptr < Nf::IntParameter > m_frame;
    void onUpdateFrame();
    CLASS_CALLBACK(onUpdateFrame, Teleoperation2DFileWidget);

  };

  class Teleoperation2DStreamingWidget : public Teleoperation2DWidget
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < RPUlteriusProcessManager > m_rpReaders;
    std::tr1::shared_ptr < QTimer > m_tick;

  public:
    Teleoperation2DStreamingWidget(QWidget *parent);
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