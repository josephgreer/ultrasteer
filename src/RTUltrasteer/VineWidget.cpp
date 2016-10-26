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
  {
    m_imageViewer = std::tr1::shared_ptr<ImageViewerWidget>(new ImageViewerWidget(parent));
		m_tapeWidget = std::tr1::shared_ptr<TapeRobotWidget>(new TapeRobotWidget(parent));

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
		connect(this, SIGNAL(stopSerialReceiveThread()), m_cameraThread.get(), SLOT(stopThread()));

    ADD_CHILD_COLLECTION(m_imageViewer.get());
    ADD_OPEN_FILE_PARAMETER(m_videoFile, "Video File",CALLBACK_POINTER(SetupVideoInput, VineWidget), this, BASE_PATH_CAT("VineVideos/Vine1.mov"), "(*.mov)");
		ADD_BOOL_PARAMETER(m_useWebcam, "Use Webcam", CALLBACK_POINTER(SetupVideoInput, VineWidget), this, false);
		ADD_BOOL_PARAMETER(m_run, "Run", CALLBACK_POINTER(SetupVideoInput, VineWidget), this, false);
		ADD_VEC3D_PARAMETER(m_lowerBounds, "Lower Bounds", NULL, this, Vec3d(255,255,255), Vec3d(0,0,0), Vec3d(179,255,255), Vec3d(1,1,1));
		ADD_VEC3D_PARAMETER(m_upperBounds, "Upper Bounds", NULL, this, Vec3d(0,0,0), Vec3d(0,0,0), Vec3d(179,255,255), Vec3d(1,1,1));
		ADD_BOOL_PARAMETER(m_showMask, "Show Mask", NULL, this, false);
  }

  VineWidget::~VineWidget()
  {
  }

	void VineWidget::SetImage(bool resetView)
	{

		m_cameraMutex.lock();
		if(validCoords(m_imCoords, m_data.color)) {
			cvDrawCircle(m_data.color, cvPoint((s32)(m_imCoords.x+0.5), (s32)(m_imCoords.y+0.5)), 5, cvScalar(1, 0, 0), 5);
		}

		if(m_showMask->GetValue()) {
			m_imageViewer->SetImage(&m_data, RPF_BPOST8);
		} else {
			m_imageViewer->SetImage(&m_data, RPF_COLOR);
		}

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
		m_lowerBounds->SetValue(Vec3d(val(0)-20,val(1)-70,val(2)-30));
		m_upperBounds->SetValue(Vec3d(val(0)+20,val(1)+70,val(2)+30));
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
	SerialReceiveThread::SerialReceiveThread(VineWidget *vine)
		: BasicThread(vine)
	{
	}

	void SerialReceiveThread::execute()
	{
		s32 x = 0;
	}
	//////////////////////////////////////////////////////
	/// END SerialReceiveThread
	//////////////////////////////////////////////////////

	
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

	void CameraThread::execute()
	{
		cv::Mat raw,frame;
		if(m_vineWidget->m_cap->isOpened()) {
			(*m_vineWidget->m_cap) >> raw;

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
			
			cv::Mat mask;
			cv::Mat hsv;
			cv::cvtColor(cv::cvarrToMat(rp.color), hsv, CV_BGR2HSV);
			Vec3d lb,ub; lb = m_vineWidget->m_lowerBounds->GetValue(); ub = m_vineWidget->m_upperBounds->GetValue();
			cv::inRange(hsv, cv::Scalar(lb.x, lb.y, lb.z), cv::Scalar(ub.x, ub.y, ub.z), mask);

			IplImage mmask = mask;
			rp.b8 = &mmask;
			std::vector < Squarei > rects = findExpandAndClusterContours(rp.b8, 1,false,true);

			cv::Rect bestRect(0,0,0,0);
			for(s32 i=0; i<rects.size(); i++) {
				if(rects[i].Area() > bestRect.area()) {
					bestRect = SquareiToCvRect(rects[i]);
				}
			}

			cv::rectangle(cv::cvarrToMat(rp.b8), bestRect, cvScalar(255), 5);
			cv::rectangle(cv::cvarrToMat(rp.color), bestRect, cvScalar(255,0,0), 5);

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