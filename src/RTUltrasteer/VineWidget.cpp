#include "VineWidget.h"
#include <vtkRendererCollection.h>
#include <armadillo>
#include <QKeyEvent>

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
		for(s32 i=0; i<2; i++) {
			m_serials[i] = std::tr1::shared_ptr<CSerial>(new CSerial());
			m_serialBuffers[i] = std::tr1::shared_ptr<CircularBuffer>(new CircularBuffer());
		}

		m_mainPressure = 0;
		for(s32 i=0; i<3; i++)
			m_actuatorPressures[i] = 0;

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
		connect(this->m_tapeWidget->ui.stop, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.stop_2, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
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

		connect(m_serialThread.get(), SIGNAL(textUpdate(QString)), this, SLOT(UpdateText(QString)));
		connect(m_serialThread.get(), SIGNAL(pressureUpdate(QVector < double >)), this, SLOT(UpdatePressures(QVector < double >)));
		connect(m_serialThread.get(), SIGNAL(extensionUpdate(QVector < double >)), this, SLOT(UpdateExtensions(QVector < double >)));
		connect(m_serialThread.get(), SIGNAL(constantsUpdate(QVector < double >)), this, SLOT(UpdateConstants(QVector < double >)));

		ADD_VEC2I_PARAMETER(m_comPorts, "Com Ports", CALLBACK_POINTER(InitSerial, VineWidget), this, Vec2i(6,3), Vec2i(1,1), Vec2i(20,20), Vec2i(1,1));
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
		m_tapeWidget->ui.regulatorAmount_1->display(text.sprintf("%.3f",values[0]));
		m_tapeWidget->ui.regulatorAmount_2->display(text.sprintf("%.3f",values[1]));
		m_tapeWidget->ui.regulatorAmount_3->display(text.sprintf("%.3f",values[2]));
		m_tapeWidget->ui.steeringBoardRate->display(values[3]);
		m_tapeWidget->ui.track_x->display(values[4]);
		m_tapeWidget->ui.track_y->display(values[5]);
		m_tapeWidget->ui.track_c->display(values[6]);
		m_tapeWidget->ui.regVert_1->setSliderPosition(SLIDER_POS(values[0]));
		m_tapeWidget->ui.regVert_2->setSliderPosition(SLIDER_POS(values[1]));
		m_tapeWidget->ui.regVert_3->setSliderPosition(SLIDER_POS(values[2]));

		m_tapeWidget->ui.actuatorAngle_1->display(text.sprintf("%.3f",values[7]));
		m_tapeWidget->ui.actuatorAngle_2->display(text.sprintf("%.3f",values[8]));
		m_tapeWidget->ui.actuatorAngle_3->display(text.sprintf("%.3f",values[9]));
		
		m_tapeWidget->ui.visionData_1->display(text.sprintf("%.3f",values[10]));
		m_tapeWidget->ui.visionData_2->display(text.sprintf("%.3f",values[11]));
		m_tapeWidget->ui.visionData_3->display(text.sprintf("%.3f",values[12]));
	}

	void VineWidget::UpdateConstants(QVector < double > values)
	{
		QString text;
		m_tapeWidget->ui.kpAmount->display(text.sprintf("%f",values[0]));
		m_tapeWidget->ui.kdAmount->display(text.sprintf("%f",values[1]));
	}

	void VineWidget::UpdateExtensions(QVector < double > values)
	{
		m_tapeWidget->ui.extensionAmount->display(values[0]);
		m_tapeWidget->ui.extensionVel->display(values[1]);
		m_tapeWidget->ui.extensionAmount_2->display(values[2]);
		m_tapeWidget->ui.extensionVel_2->display(values[3]);
	}


	bool VineWidget::eventFilter(QObject *obj, QEvent *event)
	{
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			if(!m_serials[0]->IsOpened() || !m_serials[1]->IsOpened()) {
				return true;
			}
			if(keyEvent->key() == Qt::Key::Key_Up) {
				const char *str = "d u\n";
				this->m_serials[1]->SendData(str, strlen(str));
			} else if(keyEvent->key() == Qt::Key::Key_Down) {
				const char *str = "d d\n";
				this->m_serials[1]->SendData(str, strlen(str));
			} else if(keyEvent->key() == Qt::Key::Key_Left) {
				const char *str = "d l\n";
				this->m_serials[1]->SendData(str, strlen(str));
			} else if(keyEvent->key() == Qt::Key::Key_Right) {
				const char *str = "d r\n";
				this->m_serials[1]->SendData(str, strlen(str));
			} 
			return true;
		} else {
			// standard event processing
			return QObject::eventFilter(obj, event);
		}
	}

	void VineWidget::HWButtonPushed()
	{
		QPushButton *button = (QPushButton *)sender();

		if(!m_serials[0]->IsOpened() || !m_serials[1]->IsOpened()) {
			NTrace("Serial is not opened\n");
			return;
		}

		std::string command;
		if(button == m_tapeWidget->ui.posMode) {
			const char *str = "m 0 p\n";
			this->m_serials[0]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.posMode_2) {
			const char *str = "m 1 p\n";
			this->m_serials[0]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.velMode){
			const char *str = "m 0 v\n";
			this->m_serials[0]->SendData(str, strlen(str));
		}  else if(button == m_tapeWidget->ui.velMode_2){
			const char *str = "m 1 v\n";
			this->m_serials[0]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.throttleMode) {
			const char *str = "m 0 t\n";
			this->m_serials[0]->SendData(str, strlen(str));
		}  else if(button == m_tapeWidget->ui.throttleMode) {
			const char *str = "m 1 t\n";
			this->m_serials[0]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.increment) {
			char str[20] = {0}; strcpy(str, "w 0 ");
			strcat(str, m_tapeWidget->ui.incValue->text().toStdString().c_str());
			strcat(str, "\n");
			this->m_serials[0]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.increment_2) {
			char str[20] = {0}; strcpy(str, "w 1 ");
			strcat(str, m_tapeWidget->ui.incValue->text().toStdString().c_str());
			strcat(str, "\n");
			this->m_serials[0]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.decrement) {
			char str[20] = {0}; strcpy(str, "q 0 ");
			strcat(str, m_tapeWidget->ui.decValue->text().toStdString().c_str());
			strcat(str, "\n");
			this->m_serials[0]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.decrement_2) {
			char str[20] = {0}; strcpy(str, "q 1 ");
			strcat(str, m_tapeWidget->ui.decValue->text().toStdString().c_str());
			strcat(str, "\n");
			this->m_serials[0]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.pause) {
			const char *str = "p 0\n";
			this->m_serials[0]->SendData(str, strlen(str));
			std::string cmd = "c n\n";
			this->m_serials[1]->SendData(cmd.c_str(), cmd.length());
		} else if(button == m_tapeWidget->ui.pause_2) {
			const char *str = "p 1\n";
			this->m_serials[0]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.stop) {
			const char *str = "s 0\n";
			this->m_serials[0]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.stop_2) {
			const char *str = "s 1\n";
			this->m_serials[0]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.regInc_1) {
			m_actuatorPressures[1-1] += atof(m_tapeWidget->ui.regIncAmount_1->text().toStdString().c_str());
			command = "l 1 " + std::to_string((long double)m_actuatorPressures[1-1]) + "\n";
			this->m_serials[1]->SendData(command.c_str(), command.length());
		} else if(button == m_tapeWidget->ui.regInc_2) {
			m_actuatorPressures[2-1] += atof(m_tapeWidget->ui.regIncAmount_2->text().toStdString().c_str());
			command = "l 2 " + std::to_string((long double)m_actuatorPressures[2-1]) + "\n";
			this->m_serials[1]->SendData(command.c_str(), command.length());
		} else if(button == m_tapeWidget->ui.regInc_3) {
			m_actuatorPressures[3-1] += atof(m_tapeWidget->ui.regIncAmount_3->text().toStdString().c_str());
			command = "l 3 " + std::to_string((long double)m_actuatorPressures[3-1]) + "\n";
			this->m_serials[1]->SendData(command.c_str(), command.length());
		} else if(button == m_tapeWidget->ui.regDec_1) {
			m_actuatorPressures[1-1] -= atof(m_tapeWidget->ui.regIncAmount_1->text().toStdString().c_str());
			command = "l 1 " + std::to_string((long double)m_actuatorPressures[1-1]) + "\n";
			this->m_serials[1]->SendData(command.c_str(), command.length());
		} else if(button == m_tapeWidget->ui.regDec_2) {
			m_actuatorPressures[2-1] -= atof(m_tapeWidget->ui.regIncAmount_2->text().toStdString().c_str());
			command = "l 2 " + std::to_string((long double)m_actuatorPressures[2-1]) + "\n";
			this->m_serials[1]->SendData(command.c_str(), command.length());
		} else if(button == m_tapeWidget->ui.regDec_3) {
			m_actuatorPressures[3-1] -= atof(m_tapeWidget->ui.regIncAmount_3->text().toStdString().c_str());
			command = "l 3 " + std::to_string((long double)m_actuatorPressures[3-1]) + "\n";
			this->m_serials[1]->SendData(command.c_str(), command.length());
		} else if(button == m_tapeWidget->ui.beginSteering) {
			const char *str = "c p\n";
			this->m_serials[1]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.openLoopSteering) {
			const char *str = "c o\n";
			this->m_serials[1]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.pauseSteering) {
			const char *str = "c n\n";
			this->m_serials[1]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.straighten) {
			const char *str = "c h\n";
			this->m_serials[1]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.calibrateJacobian) {
			const char *str = "c c\n";
			this->m_serials[1]->SendData(str, strlen(str));
			std::string cmd = "p\n";
			this->m_serials[0]->SendData(cmd.c_str(), strlen(str));
		} else if(button == m_tapeWidget->ui.posIntermittent) {
			const char *str = "c u\n";
			this->m_serials[1]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.kpUp) {
			const char *str = "i ik\n";
			this->m_serials[1]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.kpDown) {
			const char *str = "i dk\n";
			this->m_serials[1]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.kdUp) {
			const char *str = "i id\n";
			this->m_serials[1]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.kdDown) {
			const char *str = "i dd\n";
			this->m_serials[1]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.requestKp) {
			const char *str = "i r\n";
			this->m_serials[1]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.incrementPressure) {
			m_mainPressure += atof(m_tapeWidget->ui.incPressureValue->text().toStdString().c_str());
			m_mainPressure = MAX(MIN(m_mainPressure,1),0);
			char str[20] = {0}; strcpy(str, "e ");
			strcat(str, std::to_string((long double)m_mainPressure).c_str());
			this->m_serials[1]->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.decPressure) {
			m_mainPressure -= atof(m_tapeWidget->ui.incPressureValue->text().toStdString().c_str());
			m_mainPressure = MAX(MIN(m_mainPressure,1),0);
			char str[20] = {0}; strcpy(str, "e ");
			strcat(str, std::to_string((long double)m_mainPressure).c_str());
			this->m_serials[1]->SendData(str, strlen(str));
		} else {
			NTrace("hmmm... ");
			assert(0);
		}
	}

	void VineWidget::InitSerial()
	{
		if(m_serialThread->isRunning()) {
			emit stopSerialReceiveThread();
			m_serialThread->wait();
		}

		for(s32 i=0; i<2; i++) {
			if(m_serials[i]->IsOpened())
				m_serials[i]->Close();
		}

		s32 comPorts[2] = {m_comPorts->GetValue().x, m_comPorts->GetValue().y};
		if(m_serialInit->GetValue()) {
			for(s32 i=0; i<2; i++) {
				bool rv = m_serials[i]->Open(comPorts[i], 57600);
				if(rv != true) {
					m_serialInit->SetValue(false);
					return;
				}
				m_serialThread->start();
			}
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
		if(!m_vineWidget->m_serials[0]->IsOpened() || !m_vineWidget->m_serials[1]->IsOpened()) {
			msleep(100);
			return;
		}

		for(s32 i=0; i<2; i++) {
			s32 rcvSz = this->m_vineWidget->m_serials[i]->ReadData(m_data, SERIAL_SZ);

			if(rcvSz > 0) {
				m_vineWidget->m_serialBuffers[i]->InsertData((u8 *)m_data, rcvSz);
				s8* cdata;
				s32 dataSz = m_vineWidget->m_serialBuffers[i]->GetAvailableData(&cdata);
				bool isGood = false;
				for(s32 jj=0; jj<dataSz; jj++) {
					s32 pos;
					if(cdata[jj] == 'P') {
						f32 ps[3] = {0}; f32 tt; f32 trackx, tracky,trackConf;
						f32 actValues[3] = {0}; f32 visionValues[3] = {0};
						if(sscanf(&cdata[jj], "P %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f%n", &ps[0], &ps[1], &ps[2],&tt,&trackx,&tracky,&trackConf,
							&actValues[0], &actValues[1], &actValues[2], &visionValues[0], &visionValues[1], &visionValues[2], &pos) == 13 && cdata[jj+pos] == ';') {
							QVector < f64 > values(13); 
							values[0] = ps[0]; values[1] = ps[1]; values[2] = ps[2];
							values[3] = tt; values[4] = trackx; values[5] = tracky; values[6] = trackConf;
							values[7] = actValues[0]; values[8] = actValues[1]; values[9] = actValues[2];
							values[10] = visionValues[0]; values[11] = visionValues[1]; values[12] = visionValues[2];
							emit pressureUpdate(values);
							isGood = true;
						}
						break;
					} else if(cdata[jj] == 'E') {
						f32 es[4] = {0};
						if(sscanf(&cdata[jj], "E %f, %f, %f, %f%n", &es[0], &es[1], &es[2], &es[3], &pos) == 4 && cdata[jj+pos] == ';') {
							QVector < double > values(4); 
							for(s32 jj=0; jj<4; jj++)
								values[jj] = es[jj];
							emit extensionUpdate(values);
						}
						isGood = true;
						break;
					} else if(cdata[jj] == 'J' && cdata[jj+1] == 'J') {
						f64 es[9] = {0};
						if(sscanf(&cdata[jj], "JJ=[%f %f %f;\n%f %f %f;\n%f %f %f%n];",&es[0],&es[1],&es[2],
							&es[3],&es[4],&es[5],&es[6],&es[7],&es[8],&pos) > 0) {
							cdata[jj+pos] = 0;
							emit textUpdate(QString(&cdata[jj]));
						}
						
					} else if(cdata[jj] == 'K') {
						f32 ks[2] = {0};
						if(sscanf(&cdata[jj], "K %f, %f%n", &ks[0], &ks[1],&pos) == 2 && cdata[jj+pos] == ';') {
							QVector < double > values(2); values[0] = ks[0]; values[1] = ks[1];
							emit constantsUpdate(values);
						}
						isGood = true;
					}
				}
				if(isGood)
					m_vineWidget->m_serialBuffers[i]->DeleteElements(dataSz);

				memset(m_data, 0, rcvSz);
			} else if(i==1) {
				return;
			}
		}
	}
	//////////////////////////////////////////////////////
	/// END SerialReceiveThread
	//////////////////////////////////////////////////////
}