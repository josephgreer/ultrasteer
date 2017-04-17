#include "VineWidget.h"
#include <vtkRendererCollection.h>
#include <armadillo>
#include <QKeyEvent>
#include <Windows.h>

namespace Nf
{

	static bool validCoords(Vec2d coords, const IplImage *im)
	{
		if(0 <= coords.x && coords.x < im->width && 0 <= coords.y && coords.y < im->height)
			return true;
		return false;
	}

	//////////////////////////////////////////////////////
	/// BEGIN VINEWIDGET
	//////////////////////////////////////////////////////
  VineWidget::VineWidget(QWidget *parent, const char *name)
    : Nf::ParameterCollection(name)
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
  {
		m_tapeWidget = std::tr1::shared_ptr<TapeRobotWidget>(new TapeRobotWidget(parent));
		m_serial = std::tr1::shared_ptr<CSerial>(new CSerial());
		m_serialBuffer = std::tr1::shared_ptr<CircularBuffer>(new CircularBuffer());

		m_mainPressure = 0;

    m_layout = new QGridLayout(parent);
    m_layout->addWidget((QWidget *)(m_tapeWidget.get()), 0, 0);
    m_layout->setContentsMargins(0,0,5,0);
    this->setLayout(m_layout);

		m_serialThread = std::tr1::shared_ptr < SerialReceiveThread >(new SerialReceiveThread(this));
		
		connect(this, SIGNAL(stopSerialReceiveThread()), m_serialThread.get(), SLOT(stopThread()));

		// HW Button Signals
		connect(this->m_tapeWidget->ui.posMode, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.posMode_2, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.velMode, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.velMode_2, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.throttleMode, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.throttleMode_2, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.increment, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.increment_2, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.decrement, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.decrement_2, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.incrementPressure, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.decPressure, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.pause, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.pause_2, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.pause_3, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.stop, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.stop_2, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.stop_3, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.reelInCamera, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.grow, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.regInc_1, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.regInc_2, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.regInc_3, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.regDec_1, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.regDec_2, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.regDec_3, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.beginSteering, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.openLoopSteering, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.posIntermittent, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.pauseSteering, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.straighten, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.calibrateJacobian, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.kdUp, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.kdDown, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.kpUp, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.kpDown, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.requestKp, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.decMinTension, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.incMinTension, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.normalSteering, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.closeSteering, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));

		connect(m_serialThread.get(), SIGNAL(textUpdate(QString)), this, SLOT(UpdateText(QString)));
		connect(m_serialThread.get(), SIGNAL(pressureUpdate(QVector < double >)), this, SLOT(UpdatePressures(QVector < double >)));

		ADD_INT_PARAMETER(m_comPort, "Com Port", CALLBACK_POINTER(InitSerial, VineWidget), this, 6, 1, 20, 1);
		ADD_BOOL_PARAMETER(m_serialInit, "Init Serial", CALLBACK_POINTER(InitSerial, VineWidget), this, false);
  }

  VineWidget::~VineWidget()
  {
  }

	void VineWidget::UpdateText(QString text)
	{
		m_tapeWidget->ui.textEdit->moveCursor (QTextCursor::End);
		m_tapeWidget->ui.textEdit->insertPlainText (text);
		m_tapeWidget->ui.textEdit->moveCursor (QTextCursor::End);
	}

#define SLIDER_POS(x)((s32)(99*(x)/0.15))
	
	// 0 = pressure_1, 1 = pressure_2, 2 = pressure_3
	// 3 = steering board rate, 4 = track_x, 5 = track_y, 6 = track_c 
	// 7 = act1, 8 = act2, 9 = act3, 
	// 10 = maxTrackX, 11 = maxTrackY, 12 = integralAngle
	void VineWidget::UpdatePressures(QVector < f64 > values)
	{
		QString text;

		m_tapeWidget->ui.actuatorAngle_1->display(text.sprintf("%.3f",values[0]));
		m_tapeWidget->ui.actuatorAngle_2->display(text.sprintf("%.3f",values[1]));
		m_tapeWidget->ui.track_x->display(text.sprintf("%.3f",values[2]));
		m_tapeWidget->ui.track_y->display(values[3]);
		m_tapeWidget->ui.track_c->display(values[4]);
		m_tapeWidget->ui.extensionAmount->display(values[5]);
		m_tapeWidget->ui.extensionVel->display(values[6]);
		m_tapeWidget->ui.extensionAmount_2->display(values[7]);
		m_tapeWidget->ui.extensionVel_2->display(values[8]);
	}


	bool VineWidget::eventFilter(QObject *obj, QEvent *event)
	{
		// standard event processing
		return QObject::eventFilter(obj, event);
	}

	void VineWidget::HWButtonPushed()
	{
		QPushButton *button = (QPushButton *)sender();

		if(!m_serial->IsOpened()) {
			NTrace("Serial is not opened\n");
			return;
		}

		std::string command;
		if(button == m_tapeWidget->ui.posMode) {
			const char *str = "m p\n";
			this->m_serial->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.velMode){
			const char *str = "m v\n";
			this->m_serial->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.throttleMode) {
			const char *str = "m t\n";
			this->m_serial->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.increment) {
			char str[20] = {0}; strcpy(str, "w ");
			strcat(str, m_tapeWidget->ui.incValue->text().toStdString().c_str());
			strcat(str, "\n");
			this->m_serial->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.decrement) {
			char str[20] = {0}; strcpy(str, "q ");
			strcat(str, m_tapeWidget->ui.decValue->text().toStdString().c_str());
			strcat(str, "\n");
			this->m_serial->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.incrementPressure) {
			m_mainPressure += atof(m_tapeWidget->ui.incPressureValue->text().toStdString().c_str());
			m_mainPressure = MAX(MIN(m_mainPressure,1),0);
			char str[20] = {0}; strcpy(str, "l ");
			strcat(str, std::to_string((long double)m_mainPressure).c_str());
			this->m_serial->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.decPressure) {
			m_mainPressure -= atof(m_tapeWidget->ui.incPressureValue->text().toStdString().c_str());
			m_mainPressure = MAX(MIN(m_mainPressure,1),0);
			char str[20] = {0}; strcpy(str, "l ");
			strcat(str, std::to_string((long double)m_mainPressure).c_str());
			this->m_serial->SendData(str, strlen(str));
		} else {
			NTrace("hmmm... ");
		}
	}

	void VineWidget::InitSerial()
	{
		if(m_serialThread->isRunning()) {
			emit stopSerialReceiveThread();
			m_serialThread->wait();
		}

		if(m_serial->IsOpened())
			m_serial->Close();

		if(m_serialInit->GetValue()) {
			bool rv = m_serial->Open(m_comPort->GetValue(), 57600);
			if(rv != true) {
				m_serialInit->SetValue(false);
				return;
			}
			m_serialThread->start();
		}
	}

  void VineWidget::UpdateGeometry()
  {
    this->updateGeometry();
  }

  void VineWidget::UpdateSize(QSize sz)
  {
    s32 ml, mr, mu, mb;
    ResizableQWidget::UpdateSize(sz);
    m_layout->getContentsMargins(&ml, &mu, &mr, &mb);
    m_tapeWidget->UpdateSize(QSize(sz.width()-10, sz.height()));
  }

  std::vector < QVTKWidget * > VineWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    return res;
  }
	//////////////////////////////////////////////////////
	/// END VINEWIDGET
	//////////////////////////////////////////////////////

	//////////////////////////////////////////////////////
	/// BEGIN CIRCULAR_BUFF
	//////////////////////////////////////////////////////
	CircularBuffer::CircularBuffer()
		: m_headIndex(-1)
		, m_tailIndex(0)
	{
		memset(m_data,0,CIRCULAR_BUFF_SZ);
	}

	void CircularBuffer::InsertData(const u8 *data, s32 dataSz)
	{
		s32 amountToCopy = dataSz;
		if(amountToCopy > 0 && m_headIndex < 0)
			m_headIndex = 0;
		const u8 *dataPtr = data;
		if(dataSz > CIRCULAR_BUFF_SZ) {
			dataPtr = data+dataSz-CIRCULAR_BUFF_SZ;
		}

		if(m_tailIndex >= m_headIndex) {
			s32 copyAmount = MIN(amountToCopy,CIRCULAR_BUFF_SZ-m_tailIndex);
			memcpy(m_data+m_tailIndex, dataPtr, copyAmount);
			m_tailIndex += copyAmount;
			if(m_tailIndex >= CIRCULAR_BUFF_SZ)
				m_tailIndex -= CIRCULAR_BUFF_SZ;
			amountToCopy -= copyAmount;
			dataPtr += copyAmount;
		}
		if(amountToCopy > 0) {
			s32 copyAmount = MIN(amountToCopy,CIRCULAR_BUFF_SZ-m_tailIndex);
			memcpy(m_data+m_tailIndex, dataPtr, copyAmount);
			m_headIndex += MAX(copyAmount-(m_headIndex-m_tailIndex),0);
			m_tailIndex += copyAmount;
			if(m_tailIndex >= CIRCULAR_BUFF_SZ)
				m_tailIndex -= CIRCULAR_BUFF_SZ;
		}
	}

	s32 CircularBuffer::GetLength()
	{
		if(m_headIndex < 0)
			return 0;
		if(m_headIndex < m_tailIndex)
			return m_tailIndex-m_headIndex;
		else
			return m_tailIndex+CIRCULAR_BUFF_SZ-m_headIndex;
	}

	s32 CircularBuffer::GetAvailableData(s8 **data)
	{
		s32 len = GetLength();
		*data = NULL;
		if(len == 0) {
			return 0;
		}
		*data = &m_dataReturn[0];
		s8 *dataPtr = &m_dataReturn[0];

		s32 amountToCopy = len;
		if(amountToCopy > CIRCULAR_BUFF_SZ-m_headIndex) {
			s32 copyAmount = CIRCULAR_BUFF_SZ-m_headIndex;
			memcpy(dataPtr, &m_data[m_headIndex], copyAmount);
			amountToCopy -= copyAmount;
			dataPtr += copyAmount;

			memcpy(dataPtr, &m_data[0], amountToCopy);
		} else {
			memcpy(dataPtr, &m_data[m_headIndex], amountToCopy);
		}

		return len;

	}

	s8 CircularBuffer::GetElement(s32 index)
	{
		if(index >= GetLength())
			assert(0);

		if(m_headIndex+index < CIRCULAR_BUFF_SZ)
			return m_data[m_headIndex+index];
		else
			return m_data[index-(CIRCULAR_BUFF_SZ-m_headIndex)];
	}

	void CircularBuffer::DeleteElements(s32 elementCount)
	{
		if(elementCount >= GetLength()) {
			m_headIndex = -1;
			m_tailIndex = 0;
			return;
		}

		if(elementCount >= CIRCULAR_BUFF_SZ-m_headIndex)
			m_headIndex = elementCount-CIRCULAR_BUFF_SZ-m_headIndex;
		else
			m_headIndex += elementCount;
	}

	//////////////////////////////////////////////////////
	/// END CIRCULAR_BUFF
	//////////////////////////////////////////////////////
	
	
	//////////////////////////////////////////////////////
	/// BEGIN BasicThread
	//////////////////////////////////////////////////////
	BasicThread::BasicThread(VineWidget *vine)
		: m_vineWidget(vine)
		, m_running(false)
	{
	}

	void BasicThread::run()
	{
		m_running = true;
		setup();
		while(m_running) {
			execute();
		}
	}

	void BasicThread::stopThread()
	{
		m_running = false;
	}
	//////////////////////////////////////////////////////
	/// END BasicThread
	//////////////////////////////////////////////////////

	//////////////////////////////////////////////////////
	/// BEGIN SerialReceiveThread
	//////////////////////////////////////////////////////
#define SERIAL_SZ 1024
	SerialReceiveThread::SerialReceiveThread(VineWidget *vine)
		: BasicThread(vine)
	{
		m_data = new char[SERIAL_SZ];
		memset(m_data, 0, SERIAL_SZ);
	}

	SerialReceiveThread::~SerialReceiveThread()
	{
		delete m_data;
		m_data = NULL;
	}

	void SerialReceiveThread::execute()
	{
		if(!m_vineWidget->m_serial->IsOpened()){
			msleep(100);
			return;
		}

		s32 rcvSz = this->m_vineWidget->m_serial->ReadData(m_data, SERIAL_SZ);

		if(rcvSz > 0) {
			m_vineWidget->m_serialBuffer->InsertData((u8 *)m_data, rcvSz);
			s8* cdata;
			s32 dataSz = m_vineWidget->m_serialBuffer->GetAvailableData(&cdata);
			bool isGood = false;
			for(s32 jj=0; jj<dataSz; jj++) {
				s32 pos;
				if(cdata[jj] == 'P') {
					f32 us[2] = {0};
					f32 trackPos[2] = {0};
					f32 cameraRotation = 0;
					f32 motorPos = 0; f32 motorVel = 0;
					f32 pressures[2] = {0};
					f32 time;
					if(sscanf(&cdata[jj], "P %f, %f, %f, %f, %f, %f, %f, %f, %f, %f%n", &time, &us[0], &us[1], &trackPos[0], &trackPos[1],
						&cameraRotation, &motorPos, &motorVel, &pressures[0], &pressures[1], &pos) == 10 && cdata[jj+pos] == ';') {
							QVector < f64 > values(10); 
							values[0] = us[0]; values[1] = us[1];
							values[2] = trackPos[0]; values[3] = trackPos[1]; 
							values[4] = cameraRotation;
							values[5] = motorPos; values[6] = motorVel;
							values[7] = pressures[0]; values[8] = pressures[1];
							emit pressureUpdate(values);
							isGood = true;
					}
					break;
				}
			}
			if(isGood)
				m_vineWidget->m_serialBuffer->DeleteElements(dataSz);
		}

		memset(m_data, 0, rcvSz);
	}
	//////////////////////////////////////////////////////
	/// END SerialReceiveThread
	//////////////////////////////////////////////////////
}