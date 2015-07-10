#ifndef RPWIDGET_H
#define RPWIDGET_H

#include "UICore.h"
#include <QGroupBox>
#include <QGridLayout>
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "USVisualizerWidget.h"
#include "RPFileReader.h"
#include "RPProcess.h"

namespace Nf
{
  class RPWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < ImageViewerWidget> m_imageViewer;
    std::tr1::shared_ptr < USVisualizer > m_usVis;
    std::tr1::shared_ptr < Image3DImagePlaneWidget > m_planeVis;
    RPData m_data;
    QGridLayout *m_layout;
    QtEnums::RPWidgetVisMode m_prevVisMode;

  public:
    RPWidget(QWidget *parent, USVisualizer *vis);
    ~RPWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();

    //visualizaiton mode
    std::tr1::shared_ptr < Nf::EnumParameter > m_visMode;
    void onSetVisMode();
    CLASS_CALLBACK(onSetVisMode, RPWidget);

    std::vector < QVTKWidget * > GetChildWidgets();
  };

  class RPFileWidget : public RPWidget
  {
    Q_OBJECT 

  protected:
    RPFileReaderCollection *m_rpReaders;

  public:
    RPFileWidget(QWidget *parent, USVisualizer *vis = NULL);
    virtual ~RPFileWidget();

    //Filename
    std::tr1::shared_ptr < Nf::FileParameter > m_rpFile;
    void onUpdateFile();
    CLASS_CALLBACK(onUpdateFile, RPFileWidget);

    //Frame index
    std::tr1::shared_ptr < Nf::IntParameter > m_frame;
    void onUpdateFrame();
    CLASS_CALLBACK(onUpdateFrame, RPFileWidget);
  };

  class RPStreamingWidget : public RPWidget
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < RPUlteriusProcessManager > m_rpReaders;
    std::tr1::shared_ptr < QTimer > m_tick;

  public:
    RPStreamingWidget(QWidget *parent, USVisualizer *vis = NULL);
    virtual ~RPStreamingWidget();

    //IP
    std::tr1::shared_ptr < Nf::StringParameter > m_rpIp;

    //Stream
    std::tr1::shared_ptr < Nf::BoolParameter > m_init;
    void onInitializeToggle();
    CLASS_CALLBACK(onInitializeToggle, RPStreamingWidget);

    //Add Frames
    std::tr1::shared_ptr < Nf::BoolParameter > m_addFrames;
    void onAddFramesToggle();
    CLASS_CALLBACK(onAddFramesToggle, RPStreamingWidget);

    //Framerate
    std::tr1::shared_ptr < Nf::IntParameter > m_framerate;
    void onFramerateChanged();
    CLASS_CALLBACK(onFramerateChanged, RPStreamingWidget);

    //MPP
    std::tr1::shared_ptr < Nf::FloatParameter > m_mpp;

    //Origin
    std::tr1::shared_ptr < Nf::Vec2dParameter > m_origin;

  public slots:
    void onTick();
  };
}

#endif // RPWIDGET_H
