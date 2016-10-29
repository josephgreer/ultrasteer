#include "VineWidget.h"
#include <vtkRendererCollection.h>

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
		, m_cap(NULL)
		, m_imCoords(-1,-1)
		, m_oddDirection(-1)		//Direction odd actuators move you in the image
		, m_actuatorIndex(1)
  {
    m_imageViewer = std::tr1::shared_ptr<ImageViewerWidget>(new ImageViewerWidget(parent));
		m_tapeWidget = std::tr1::shared_ptr<TapeRobotWidget>(new TapeRobotWidget(parent));
		m_serial = std::tr1::shared_ptr<CSerial>(new CSerial());

    m_pointPicker = vtkSmartPointer<vtkPointPicker>::New();
		m_pointPicker->SetTolerance(0.0);
		m_pointPicker->PickFromListOn();
		m_pointPicker->AddPickList(m_imageViewer->GetActor());
		m_imageViewer->GetWindowInteractor()->SetPicker(m_pointPicker);
		m_imageViewer->GetWindowInteractor()->SetInteractorStyle(this);

    m_layout = new QGridLayout(parent);
    m_layout->addWidget((QWidget *)(m_imageViewer.get()), 0, 0);
    m_layout->addWidget((QWidget *)(m_tapeWidget.get()), 0, 1);
    m_layout->setContentsMargins(0,0,5,0);
    this->setLayout(m_layout);

		m_serialThread = std::tr1::shared_ptr < SerialReceiveThread >(new SerialReceiveThread(this));
		m_cameraThread = std::tr1::shared_ptr < CameraThread >(new CameraThread(this));
		
		connect(this, SIGNAL(stopCameraThread()), m_cameraThread.get(), SLOT(stopThread()));
		connect(this, SIGNAL(stopSerialReceiveThread()), m_serialThread.get(), SLOT(stopThread()));

		// HW Button Signals
		connect(this->m_tapeWidget->ui.posMode, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.velMode, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.throttleMode, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.increment, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.decrement, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.incrementPressure, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.decPressure, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.setZeroPoint, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.turn, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.dontTurn, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));
		connect(this->m_tapeWidget->ui.pause, SIGNAL(clicked()), this, SLOT(HWButtonPushed()));

		connect(m_serialThread.get(), SIGNAL(incrementActuator(s32)), this, SLOT(ActuatorIncrement(s32)));
		connect(m_serialThread.get(), SIGNAL(textUpdate(QString)), this, SLOT(UpdateText(QString)));

    ADD_CHILD_COLLECTION(m_imageViewer.get());
    ADD_OPEN_FILE_PARAMETER(m_videoFile, "Video File",CALLBACK_POINTER(SetupVideoInput, VineWidget), this, BASE_PATH_CAT("VineVideos/Vine1.mov"), "(*.mov)");
		ADD_BOOL_PARAMETER(m_useWebcam, "Use Webcam", CALLBACK_POINTER(SetupVideoInput, VineWidget), this, false);
		ADD_BOOL_PARAMETER(m_run, "Run", CALLBACK_POINTER(SetupVideoInput, VineWidget), this, false);
		ADD_VEC3D_PARAMETER(m_lowerBounds, "Lower Bounds", NULL, this, Vec3d(255,255,255), Vec3d(0,0,0), Vec3d(179,255,255), Vec3d(1,1,1));
		ADD_VEC3D_PARAMETER(m_upperBounds, "Upper Bounds", NULL, this, Vec3d(0,0,0), Vec3d(0,0,0), Vec3d(179,255,255), Vec3d(1,1,1));
		ADD_VEC3D_PARAMETER(m_lowerBoundsObstacle, "Lower Bounds Obstacle", NULL, this, Vec3d(255,255,255), Vec3d(0,0,0), Vec3d(179,255,255), Vec3d(1,1,1));
		ADD_VEC3D_PARAMETER(m_upperBoundsObstacle, "Upper Bounds Obstacle", NULL, this, Vec3d(0,0,0), Vec3d(0,0,0), Vec3d(179,255,255), Vec3d(1,1,1));
		ADD_VEC3D_PARAMETER(m_calibratePadding, "Calibrate Padding", NULL, this, Vec3d(10,25,35), Vec3d(0,0,0), Vec3d(179,255,255), Vec3d(1,1,1));
		ADD_INT_PARAMETER(m_comPort, "Com Port", CALLBACK_POINTER(InitSerial, VineWidget), this, 6, 1, 20, 1);
		ADD_BOOL_PARAMETER(m_serialInit, "Init Serial", CALLBACK_POINTER(InitSerial, VineWidget), this, false);
    ADD_ENUM_PARAMETER(m_displayMode, "Display Modality", NULL, this, QtEnums::DisplayModality::DM_BPOST32, "DisplayModality");
		ADD_INT_PARAMETER(m_calibrateObject, "Calibrate Object", NULL, this, 0, 0, 1, 1);

		RPData rp;
		m_imageViewer->SetImage(&rp, RPF_COLOR);
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

	void VineWidget::ActuatorIncrement(s32 index)
	{
		m_actuatorIndex = index;
		this->m_tapeWidget->ui.actIndex->setText(QString::number(m_actuatorIndex));
	}

	void VineWidget::HWButtonPushed()
	{
		QPushButton *button = (QPushButton *)sender();

		if(!m_serial->IsOpened()) {
			NTrace("Serial is not opened\n");
			return;
		}

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
			char str[20] = {0}; strcpy(str, "l i ");
			strcat(str, m_tapeWidget->ui.incPressureValue->text().toStdString().c_str());
			this->m_serial->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.decPressure) {
			char str[20] = {0}; strcpy(str, "l d ");
			strcat(str, m_tapeWidget->ui.incPressureValue->text().toStdString().c_str());
			strcat(str, "\n");
			this->m_serial->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.setZeroPoint) {
			const char *str = "z\n";
			this->m_serial->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.turn) {
			const char *str = "t y\n";
			this->m_serial->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.dontTurn) {
			const char *str = "t n\n";
			this->m_serial->SendData(str, strlen(str));
		} else if(button == m_tapeWidget->ui.pause) {
			const char *str = "p\n";
			this->m_serial->SendData(str, strlen(str));
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

		if(m_serial->IsOpened())
			m_serial->Close();

		if(m_serialInit->GetValue()) {
			bool rv = m_serial->Open(m_comPort->GetValue(), 115200);
			if(rv != true) {
				m_serialInit->SetValue(false);
				return;
			}
			m_serialThread->start();
		}
	}

	void VineWidget::SetImage(bool resetView)
	{

		m_cameraMutex.lock();

		m_imageViewer->SetImage(&m_data, (RP_TYPE)m_displayMode->GetValue());

		m_desiredRow = m_data.color->height/2.0;
		if(resetView)
			m_imageViewer->ResetView();
		m_imageViewer->GetWindowInteractor()->SetPicker(m_pointPicker);
		m_imageViewer->GetWindowInteractor()->SetInteractorStyle(this);
		m_cameraMutex.unlock();
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
    m_imageViewer->UpdateSize(QSize(3*sz.width()/4-10, sz.height()));
    m_tapeWidget->UpdateSize(QSize(sz.width()/4-10, sz.height()));
  }

	void VineWidget::OnLeftButtonDown()
	{      
		vtkSmartPointer < vtkRenderWindowInteractor > interactor = m_imageViewer->GetWindowInteractor();
		m_pointPicker->Pick(interactor->GetEventPosition()[0], interactor->GetEventPosition()[1], 0,  // always zero.
			interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
		double ima[3];
		m_pointPicker->GetPickPosition(ima);

		//Spacing
		m_cameraMutex.lock();
		f64 *spacing = m_imageViewer->GetImageData()->GetSpacing();
		m_imCoords = Vec2d(-ima[0]/spacing[0], -ima[1]/spacing[1]);

		cv::Rect roi((s32)(m_imCoords.x+0.5)-2, (s32)(m_imCoords.y+0.5)-2, 4,4);
		cv::Mat subRegion = cv::cvarrToMat(m_data.color);
		subRegion = subRegion(roi);
		cv::Mat hsv;
		cv::cvtColor(subRegion, hsv, CV_RGB2HSV);
		cv::Scalar val = cv::mean(hsv);
		Vec3d padding = m_calibratePadding->GetValue();
		Vec3d lb(val(0)-padding.x,val(1)-padding.y,val(2)-padding.z);
		Vec3d ub(val(0)+padding.x,255,val(2)+padding.z);
		if(m_calibrateObject->GetValue() == 0) {
			m_lowerBounds->SetValue(lb);
			m_upperBounds->SetValue(ub);
		} else {
			m_lowerBoundsObstacle->SetValue(lb);
			m_upperBoundsObstacle->SetValue(ub);
		}
		m_cameraMutex.unlock();
	}

  std::vector < QVTKWidget * > VineWidget::GetChildWidgets()
  {
    std::vector < QVTKWidget * > res;
    res.push_back(m_imageViewer.get());
    return res;
  }

	void VineWidget::SetupVideoInput()
	{
		if(m_cameraThread->isRunning()) {
			emit stopCameraThread();
			m_cameraThread->wait();
		}

		if(m_cap != NULL) {
			if(m_cap->isOpened())
				m_cap->release();
			m_cap = NULL;
		}

		if(m_run->GetValue()) {
			if(this->m_useWebcam->GetValue()) {
				m_cap = std::tr1::shared_ptr < cv::VideoCapture > (new cv::VideoCapture(0));
			} else {
				m_cap = std::tr1::shared_ptr < cv::VideoCapture > (new cv::VideoCapture(this->m_videoFile->GetValue()));
			}
			if(m_cap->isOpened()) {
				m_cameraThread->start();
			} else {
				m_cap->release();
				m_cap = NULL;
				m_run->SetValue(false);
			}
		}
	}
	//////////////////////////////////////////////////////
	/// END VINEWIDGET
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
		if(!m_vineWidget->m_serial->IsOpened()) {
			msleep(100);
			return;
		}

		s32 rcvSz = this->m_vineWidget->m_serial->ReadData(m_data, SERIAL_SZ);

		if(rcvSz > 0) {
			emit textUpdate(m_data);

			s32 actIdx = 0;
			if(sscanf(m_data, "act %d", &actIdx) == 1) {
				incrementActuator(actIdx);
			}

			memset(m_data, 0, rcvSz);
		} else {
			msleep(10);
		}
	}
	//////////////////////////////////////////////////////
	/// END SerialReceiveThread
	//////////////////////////////////////////////////////

	static cv::Rect clampRectToImage(cv::Rect rect, cv::Mat mat)
	{
		Vec2i tl;
		Vec2i br;
		tl.x = MAX(MIN(rect.tl().x,mat.size().width-1), 0);
		tl.y = MAX(MIN(rect.tl().y,mat.size().height-1), 0);
		
		br.x = MAX(MIN(rect.br().x,mat.size().width-1), 0);
		br.y = MAX(MIN(rect.br().y,mat.size().height-1), 0);

		return cv::Rect(cv::Point(tl.x, tl.y), cv::Point(br.x, br.y));
	}

	//////////////////////////////////////////////////////
	/// BEGIN CameraThread
	//////////////////////////////////////////////////////
	CameraThread::CameraThread(VineWidget *vine)
		: BasicThread(vine)
		, m_firstTime(false)
	{
		connect(this, SIGNAL(SetMainImage(bool)), vine, SLOT(SetImage(bool)));
	}

	void CameraThread::setup()
	{
		m_firstTime = true;
	}

	std::vector < Squarei > findExpandAndClusterContours(IplImage *colorMask, f32 expand, bool minArea = false, bool shrinkBack = false);
	CvRect SquareiToCvRect(const Squarei &sq);

	std::pair < std::vector < Squarei >, cv::Mat > CameraThread::CalculateBoundingRects(const cv::Mat hsv, const Vec3d &lb, const Vec3d &ub)
	{
		cv::Mat mask;
		cv::inRange(hsv, cv::Scalar(lb.x, lb.y, lb.z), cv::Scalar(ub.x, ub.y, ub.z), mask);

		IplImage mmask = mask;
		IplImage *tempIm = cvCloneImage(&mmask);
		std::vector < Squarei > rects = findExpandAndClusterContours(tempIm, 1, false,true);
		cvReleaseImage(&tempIm);

		return std::pair < std::vector < Squarei >, cv::Mat > (rects, mask);
	}

	std::pair < Squarei, Vec2d > CameraThread::FindRobotParameters(const std::vector < Squarei > &rects, const cv::Mat mask)
	{
		Squarei bestRect(Vec2d(0,0),Vec2d(0,0));
		for(s32 i=0; i<rects.size(); i++) {
			if(rects[i].Area() > bestRect.Area()) {
				bestRect = rects[i];
			}
		}

		cv::Rect roi = clampRectToImage(cv::Rect(bestRect.lr.x-10, 0, 20, mask.size().height),mask);
		cv::Moments mom = cv::moments(mask(roi));
		Vec2d headCen(mom.m10/mom.m00+roi.tl().x, mom.m01/mom.m00+roi.tl().y);

		return std::pair < Squarei, Vec2d >(bestRect, headCen);
	}

	void CameraThread::execute()
	{
		cv::Mat raw,frame;
		if(m_vineWidget->m_cap->isOpened()) {
			(*m_vineWidget->m_cap) >> raw;

			if(raw.empty()) {
				msleep(100);
				return;
			}

			if(raw.size().width > 640) {
				f64 scale = 640.0/(f64)raw.size().width;
				cv::resize(raw, frame, cv::Size(640,raw.size().height*scale));
			} else {
				frame = raw;
			}

			RPData rp;
			IplImage im = frame;
			rp.color = &im;
			rp.mpp = Vec2d(1000,1000);
			rp.gps.pose = Matrix44d::I().ToCvMat();
			rp.gps.pos = Vec3d(0,0,0);

			cv::Mat hsv;
			cv::cvtColor(cv::cvarrToMat(rp.color), hsv, CV_BGR2HSV);

			std::pair < std::vector < Squarei >, cv::Mat > robotRes = CalculateBoundingRects(hsv, m_vineWidget->m_lowerBounds->GetValue(), m_vineWidget->m_upperBounds->GetValue());
			std::pair < Squarei, Vec2d > robotParams = FindRobotParameters(robotRes.first, robotRes.second);
			Squarei robotRect = robotParams.first; Vec2d headCen = robotParams.second; 

			cv::circle(cv::cvarrToMat(rp.color), cv::Point(headCen.x, headCen.y), 5, cv::Scalar(0,0,255), 5);
			cv::rectangle(cv::cvarrToMat(rp.color), SquareiToCvRect(robotRect), cvScalar(255,0,0), 5);

			std::pair < std::vector < Squarei >, cv::Mat > obstacleRes = CalculateBoundingRects(hsv, m_vineWidget->m_lowerBoundsObstacle->GetValue(), m_vineWidget->m_upperBoundsObstacle->GetValue());
			std::vector < Squarei > obstacleBBs = obstacleRes.first;
			for(s32 i=0; i<obstacleBBs.size(); i++) {
				cv::rectangle(cv::cvarrToMat(rp.color), SquareiToCvRect(obstacleBBs[i]), cvScalar(0,255,0), 5);
			}

			IplImage maskRobot = robotRes.second;
			IplImage maskObstacles = obstacleRes.second;
			rp.b8 = &maskRobot;
			rp.dis = &maskObstacles;

			m_vineWidget->m_cameraMutex.lock();
			m_vineWidget->m_data.Release();
			m_vineWidget->m_data = rp.Clone();
			cvCvtColor(rp.color, m_vineWidget->m_data.color, CV_BGR2RGB);
			m_vineWidget->m_cameraMutex.unlock();

			emit SetMainImage(m_firstTime);

			m_firstTime = false;
		}
	}
	//////////////////////////////////////////////////////
	/// BEGIN CameraThread
	//////////////////////////////////////////////////////
}