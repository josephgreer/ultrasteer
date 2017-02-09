#ifndef VINEWIDGET_H
#define VINEWIDGET_H

#include <QMutex>
#include <QThread>
#include <QGridLayout>
#include <QTimer>
#include <vtkInteractorStyleImage.h>
#include <vtkPointPicker.h>
#include "opencv2/highgui.hpp"
#include "UICore.h"
#include "Resizable.h"
#include "ImageViewerWidget.h"
#include "TapeRobotWidget.h"
#include "Serial.h"

#define CIRCULAR_BUFF_SZ 1024

namespace Nf
{
	class BasicThread;
	class SerialReceiveThread; 

	class CircularBuffer
	{
	protected:
		s8 m_data[CIRCULAR_BUFF_SZ];
		s8 m_dataReturn[CIRCULAR_BUFF_SZ];
		s32 m_headIndex;
		s32 m_tailIndex;

	public:
		CircularBuffer();
		void InsertData(const u8 *data, s32 dataSz);
		s32 GetLength();
		s8 GetElement(s32 index);
		s32 GetAvailableData(s8 **data);
		void DeleteElements(s32 elementCount);
	};

  class VineWidget : public ResizableQWidget, public ParameterCollection
  {
    Q_OBJECT 

		friend class BasicThread;
		friend class SerialReceiveThread;

  protected:
    std::tr1::shared_ptr < TapeRobotWidget > m_tapeWidget;
		std::tr1::shared_ptr < CircularBuffer > m_serialBuffers[2];
    QGridLayout *m_layout;

		std::tr1::shared_ptr < Nf::Vec2iParameter > m_comPorts;
		std::tr1::shared_ptr < Nf::BoolParameter > m_serialInit;

		CLASS_CALLBACK(InitSerial, VineWidget);
		void InitSerial();
		
		std::tr1::shared_ptr < SerialReceiveThread > m_serialThread;

		std::tr1::shared_ptr < CSerial > m_serials[2];

		f64 m_mainPressure;
		f64 m_actuatorPressures[3];


	signals:
		void stopSerialReceiveThread();

	public slots:
		void HWButtonPushed();
		void UpdateText(QString text);
		void UpdateLog(QString text);
		void UpdatePressures(double p1, double p2, double p3, double t);
		void UpdateExtensions(double e1, double e2, double t);

  public:
    VineWidget(QWidget *parent, const char *name = "Vine Robot Widget");
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
		virtual ~SerialReceiveThread();

signals:
		void textUpdate(QString text);
		void pressureUpdate(double p1, double p2, double p3, double t);
		void extensionUpdate(double e1, double e2, double t);

	protected:
		char *m_data;
		void execute();
	};
}

#endif // VINEWIDGET_H
