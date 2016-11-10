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
		, m_oddDirection(1)		
		, m_actuatorIndex(-1)
		, m_controlState(TRS_FREE_GROWING)
		, m_growingDirection(1,0)
		, m_obstacleEndIndex(-1)
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

		connect(m_serialThread.get(), SIGNAL(incrementActuator(int)), this, SLOT(ActuatorIncrement(int)));
		connect(m_serialThread.get(), SIGNAL(incrementActuator(int)), m_cameraThread.get(), SLOT(ActuatorInc(int)));
		connect(m_serialThread.get(), SIGNAL(textUpdate(QString)), this, SLOT(UpdateText(QString)));
		connect(m_cameraThread.get(), SIGNAL(RobotActionSet(int)), this, SLOT(SetRobotAction(int)));
		connect(m_cameraThread.get(), SIGNAL(LogUpdate(QString)), this, SLOT(UpdateLog(QString)));

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

	void VineWidget::UpdateLog(QString text)
	{
		m_tapeWidget->ui.textLog->moveCursor (QTextCursor::End);
		m_tapeWidget->ui.textLog->insertPlainText (text);
		m_tapeWidget->ui.textLog->moveCursor (QTextCursor::End);
	}

	void VineWidget::ActuatorIncrement(int index)
	{
		m_actuatorIndex = index;
		this->m_tapeWidget->ui.actIndex->setText(QString::number(m_actuatorIndex));

		const char *noTurnCmd = "t n";
		const char *turnCmd = "t y";

		if(m_actionQueue.find(index) != m_actionQueue.end() && m_actionQueue[index].valid) {
			if(m_actionQueue[index].pop) {
				m_serial->SendData(noTurnCmd, strlen(noTurnCmd)); 
			} else {
				m_serial->SendData(turnCmd, strlen(turnCmd));
			}
			m_actionQueue[index].valid = false;
			UpdateLog(QString("ActuatorIncrement Actuator = " + QString::number(index) + QString(" pop = ") + QString::number(m_actionQueue[index].pop) + QString("\n")));
		}
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
		m_imCoords.x = this->m_data.color->width-1-m_imCoords.x;

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

			s32 actIdx = 0;
			if(sscanf(m_data, "act %d", &actIdx) == 1) {
				incrementActuator(actIdx);
			} else {
				emit textUpdate(m_data);
			}

			memset(m_data, 0, rcvSz);
		} else {
			msleep(5);
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

	

#define MIN_DISTANCE_TO_OBSTACLE 100

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

	std::vector < Squarei > findExpandAndClusterContours(IplImage *colorMask, f32 expand, bool minArea = false, bool shrinkBack = false, f64 distThresh = 3.0);
	CvRect SquareiToCvRect(const Squarei &sq);

	std::pair < std::vector < Squarei >, cv::Mat > CameraThread::CalculateBoundingRects(const cv::Mat hsv, const Vec3d &lb, const Vec3d &ub)
	{
		cv::Mat mask;
		cv::inRange(hsv, cv::Scalar(lb.x, lb.y, lb.z), cv::Scalar(ub.x, ub.y, ub.z), mask);

		cv::Mat filteredMask;
		
		s32 morph_size = 3;
		cv::Mat element = getStructuringElement(0, cv::Size(2*morph_size + 1, 2*morph_size+1), cv::Point(morph_size, morph_size));
		//cv::morphologyEx(mask, filteredMask, cv::MORPH_DILATE, element); 
		cv::erode(mask, filteredMask, element);
		mask = filteredMask;

		IplImage mmask = mask;
		IplImage *tempIm = cvCloneImage(&mmask);
		std::vector < Squarei > rects = findExpandAndClusterContours(tempIm, 1, false,false, 30.0);
		cvReleaseImage(&tempIm);

		return std::pair < std::vector < Squarei >, cv::Mat > (rects, mask);
	}

	std::pair < Squarei, Vec2d > CameraThread::FindRobotParameters(const std::vector < Squarei > &rects, const cv::Mat mask)
	{
		Squarei bestRect(Vec2d(0,0),Vec2d(0,0));
		Squarei currRect;
		for(s32 i=0; i<rects.size(); i++) {
			currRect = rects[i];
			currRect.weight = 0;
			if(currRect.Area() > bestRect.Area()) {
				bestRect = rects[i];
			}
		}

		cv::Rect roi = clampRectToImage(cv::Rect(bestRect.lr.x-20, 0, 20, mask.size().height),mask);
		cv::Moments mom = cv::moments(mask(roi));
		Vec2d headCen(mom.m10/mom.m00+roi.tl().x, mom.m01/mom.m00+roi.tl().y);

		return std::pair < Squarei, Vec2d >(bestRect, headCen);
	}

	Vec2d CameraThread::FindClosestObstacleDelta(Vec2d headPos, const std::vector < Squarei > &rects)
	{
		f64 closestDist = 1e10;
		Vec2d smallestDelta;
		f64 deltaMag;
		Vec2d delta;
		for(s32 i=0; i<rects.size(); i++) {
			delta = Vec2d(rects[i].DeltaX(headPos.x),	rects[i].DeltaY(headPos.y));
			deltaMag = delta.magnitudeSquared();
			if(deltaMag < closestDist) {
				smallestDelta = delta;
				closestDist = deltaMag;
			}
		}
		return smallestDelta;
	}

	void CameraThread::ActuatorInc(int index)
	{	
		switch(m_vineWidget->m_controlState) {
			case TRS_FREE_GROWING:
				{
					break;
				}
			case TRS_AVOIDING_OBSTACLE:
				{
					if(index == m_vineWidget->m_obstacleEndIndex)
						m_vineWidget->m_controlState = TRS_FREE_GROWING;
					break;
				}
			}
	}

	void CameraThread::DrawDetails(DRAW_DETAILS details, cv::Mat &im, const Vec2d &headCen, const std::vector < Squarei > &obstacleRects, const Squarei &robotRect, const std::vector < Squarei > &robotRects)
	{
		if(details & DD_OBSTACLES) {
			for(s32 i=0; i<obstacleRects.size(); i++) {
				cv::rectangle(im, SquareiToCvRect(obstacleRects[i]), cvScalar(0,255,0), 5);
			}
		}

		if(details & DD_ROBOT) {
			for(s32 i=0; i<robotRects.size(); i++) {
				cv::rectangle(im, SquareiToCvRect(robotRects[i]), cvScalar(255,0,0), 5);
			}
		}

		cv::circle(im, cv::Point(headCen.x, headCen.y), 5, cv::Scalar(0,0,255), 5);
		cv::rectangle(im, SquareiToCvRect(robotRect), cvScalar(0,0,255), 5);

		cv::Point currPoint(10,20);
		cv::Size sz;
		s32 fontFace = CV_FONT_HERSHEY_SIMPLEX;
		f64 fontScale = 0.5;
		s32 thickness = 1;
		s32 baseLine;
		
		std::string txt = "Operation Mode: ";
		switch(m_vineWidget->m_controlState)
		{
		case TRS_FREE_GROWING:
			{
				txt += "Free Growing";
				break;
			}
		case TRS_AVOIDING_OBSTACLE:
			{
				txt += "Avoiding Obstacle";
				break;
			}
		}

		bool pop = true;

		for(s32 i=0; i<m_vineWidget->m_actuatorIndex; i++) {
			if(m_vineWidget->m_actionQueue.find(i) != m_vineWidget->m_actionQueue.end())
				pop = m_vineWidget->m_actionQueue[i].pop;
		}

		sz = cv::getTextSize(txt, fontFace, fontScale, thickness, &baseLine); 
		currPoint.y += sz.height;
		cv::putText(im, txt, currPoint, fontFace, fontScale, cvScalar(255,0,0), thickness);
		currPoint.y += sz.height;

		if(pop)
			txt = "Pop";
		else
			txt = "Don't pop";

		sz = cv::getTextSize(txt, fontFace, fontScale, thickness, &baseLine); 
		currPoint.y += sz.height;
		cv::putText(im, txt, currPoint, fontFace, fontScale, cvScalar(255,0,0), thickness);
		currPoint.y += sz.height;
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
			std::pair < std::vector < Squarei >, cv::Mat > obstacleRes = CalculateBoundingRects(hsv, m_vineWidget->m_lowerBoundsObstacle->GetValue(), m_vineWidget->m_upperBoundsObstacle->GetValue());
			Squarei robotRect = robotParams.first; Vec2d headCen = robotParams.second; 

			Vec2d deltaObstacle = FindClosestObstacleDelta(headCen, obstacleRes.first);

			switch(m_vineWidget->m_controlState) {
			case TRS_FREE_GROWING:
				{
					// if we're close to an obstacle and growing towards it, initiate pop sequence
					if(deltaObstacle.magnitude() < MIN_DISTANCE_TO_OBSTACLE && deltaObstacle.dot(m_vineWidget->m_growingDirection) > 0) {

						QueuedRobotAction action; action.pop = false; action.valid = true;
						m_vineWidget->m_actionQueue[m_vineWidget->m_actuatorIndex+1] = action;
						action.pop = true;
						m_vineWidget->m_actionQueue[m_vineWidget->m_actuatorIndex+2] = action;
						action.pop = false;
						m_vineWidget->m_actionQueue[m_vineWidget->m_actuatorIndex+4] = action;
						action.pop = true;
						m_vineWidget->m_actionQueue[m_vineWidget->m_actuatorIndex+5] = action;

						char stringDesc[200] = {0};
						sprintf(stringDesc, "Beginning obstacle avoidance, actuator %d, pop = %d, actuator %d, pop = %d, actuator = %d, pop = %d, actuator = %d pop = %d\n", 
							m_vineWidget->m_actuatorIndex+1,m_vineWidget->m_actionQueue[m_vineWidget->m_actuatorIndex+1].pop,
							m_vineWidget->m_actuatorIndex+2,m_vineWidget->m_actionQueue[m_vineWidget->m_actuatorIndex+2].pop,
							m_vineWidget->m_actuatorIndex+4,m_vineWidget->m_actionQueue[m_vineWidget->m_actuatorIndex+4].pop,
							m_vineWidget->m_actuatorIndex+5,m_vineWidget->m_actionQueue[m_vineWidget->m_actuatorIndex+5].pop);

						emit LogUpdate(QString(stringDesc));

						m_vineWidget->m_controlState = TRS_AVOIDING_OBSTACLE;
						m_vineWidget->m_obstacleEndIndex = m_vineWidget->m_actuatorIndex+5;
					}
					break;
				}
			case TRS_AVOIDING_OBSTACLE:
				{
					break;
				}
			}

			DrawDetails(DD_OBSTACLES, frame, headCen, obstacleRes.first, robotParams.first, robotRes.first);
			//DrawDetails(DD_ROBOT, robotRes.second, headCen, obstacleRes.first, robotParams.first, robotRes.first);
			//DrawDetails(DD_OBSTACLES, obstacleRes.second, headCen, obstacleRes.first, robotParams.first, robotRes.first);

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