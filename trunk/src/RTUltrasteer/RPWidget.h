#ifndef RPWIDGET_H
#define RPWIDGET_H

#include "UICore.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QTabWidget>
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "USVisualizerWidget.h"
#include "RPFileReader.h"
#include "RPUlterius.h"

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
    QTabWidget *m_visTab;
    Matrix44d m_cal;

  public:
    RPWidget(QWidget *parent, USVisualizer *vis);
    ~RPWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();
    
    //displayMode
    std::tr1::shared_ptr < Nf::EnumParameter > m_displayMode;
    virtual void onSetDisplayMode();
    CLASS_CALLBACK(onSetDisplayMode, RPWidget);

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
    virtual void onUpdateFile();
    CLASS_CALLBACK(onUpdateFile, RPFileWidget);

    //Frame index
    std::tr1::shared_ptr < Nf::IntParameter > m_frame;
    void onUpdateFrame();
    CLASS_CALLBACK(onUpdateFrame, RPFileWidget);

    virtual void onSetDisplayMode();
  };

  class RPStreamingWidget : public RPWidget, public RPCallbackReceiver
  {
    Q_OBJECT 

  protected:
    std::tr1::shared_ptr < RPUlteriusReaderCollectionPush > m_rpReaders;
    bool m_isInit;
    QMutex m_lock;

  public:
    RPStreamingWidget(QWidget *parent, USVisualizer *vis = NULL);
    virtual ~RPStreamingWidget();

    void InitializeAssets();

    //IP
    std::tr1::shared_ptr < Nf::StringParameter > m_rpIp;

    //Stream
    std::tr1::shared_ptr < Nf::BoolParameter > m_init;
    virtual void onInitializeToggle();
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

    //SOS
    std::tr1::shared_ptr < Nf::FloatParameter > m_sos;

    //Origin
    std::tr1::shared_ptr < Nf::Vec2dParameter > m_origin;
    void onFrameInfoChanged();
    CLASS_CALLBACK(onFrameInfoChanged, RPStreamingWidget);

    //Doppler
    std::tr1::shared_ptr < Nf::BoolParameter > m_rcvDoppler;
    void onDataToAcquireChanged();
    CLASS_CALLBACK(onDataToAcquireChanged, RPStreamingWidget);

    //GPS2
    std::tr1::shared_ptr < Nf::BoolParameter > m_rcvGps2;

    virtual void Callback(const RPData *rp);
    void HandleFrame(RPData &rp);


  public slots:
    virtual void onFrame();

  signals:
    void frameSignal();
  };
}

#endif // RPWIDGET_H
