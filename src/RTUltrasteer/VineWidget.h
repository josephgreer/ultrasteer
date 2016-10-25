#ifndef VINEWIDGET_H
#define VINEWIDGET_H

#include <QMutex>
#include <QThread>
#include <QGridLayout>
#include <QTimer>
#include "UICore.h"
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "TapeRobotWidget.h"
#include "opencv2/highgui.hpp"

namespace Nf
{
	class BasicThread;
	class SerialReceiveThread; 
	class CameraThread;

  class VineWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT 

		friend class BasicThread;
		friend class CameraThread;

  protected:
    std::tr1::shared_ptr < ImageViewerWidget> m_imageViewer;
    std::tr1::shared_ptr < TapeRobotWidget > m_tapeWidget;
    QGridLayout *m_layout;

		std::tr1::shared_ptr < Nf::FileParameter > m_videoFile;
		std::tr1::shared_ptr < Nf::BoolParameter > m_useWebcam;
		std::tr1::shared_ptr < Nf::BoolParameter > m_run;

		CLASS_CALLBACK(SetupVideoInput, VineWidget);
		void SetupVideoInput();
		
		std::tr1::shared_ptr < SerialReceiveThread > m_serialThread;
		std::tr1::shared_ptr < CameraThread > m_cameraThread;

		QMutex m_cameraMutex;

		std::tr1::shared_ptr < cv::VideoCapture > m_cap;

		RPData m_data;

	signals:
		void stopCameraThread();
		void stopSerialReceiveThread();

	public slots:
		void SetImage(bool resetView);

  public:
    VineWidget(QWidget *parent, const char *name = "Tape Robot Widget");
    ~VineWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();

    std::vector < QVTKWidget * > GetChildWidgets();
  };

	class BasicThread : public QThread
	{
		Q_OBJECT 

	protected:
		VineWidget *m_vineWidget;
		bool m_running;

		virtual void setup() { return; }
		virtual void execute() = 0;

	public:
		BasicThread(VineWidget *vine);
		bool isRunning() { return m_running; }

	public slots:
		void stopThread();

	private:
		void run();
	};

	class SerialReceiveThread : public BasicThread
	{
		Q_OBJECT

	public:
		SerialReceiveThread(VineWidget *vine);

	protected:
		void execute();
	};

	
	class CameraThread : public BasicThread
	{
		Q_OBJECT

	public:
		CameraThread(VineWidget *vine);

	signals:
		void SetMainImage(bool);

	protected:
		bool m_firstTime; 

		void setup();
		void execute();
	};
}

#endif // VINEWIDGET_H
