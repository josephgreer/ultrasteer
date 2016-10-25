#include "VineWidget.h"

namespace Nf
{
	//////////////////////////////////////////////////////
	/// BEGIN VINEWIDGET
	//////////////////////////////////////////////////////
  VineWidget::VineWidget(QWidget *parent, const char *name)
    : Nf::ParameterCollection(name)
    , ResizableQWidget(parent, QSize(VIS_WIDTH,VIS_HEIGHT))
		, m_cap(NULL)
  {
    m_imageViewer = std::tr1::shared_ptr<ImageViewerWidget>(new ImageViewerWidget(parent));
		m_tapeWidget = std::tr1::shared_ptr<TapeRobotWidget>(new TapeRobotWidget(parent));

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
  }

  VineWidget::~VineWidget()
  {
  }

	void VineWidget::SetImage(bool resetView)
	{
		m_cameraMutex.lock();
		m_imageViewer->SetImage(&m_data, RPF_COLOR);
		if(resetView)
			m_imageViewer->ResetView();
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

	void CameraThread::execute()
	{
		cv::Mat frame;
		(*m_vineWidget->m_cap) >> frame;

		RPData rp;
		IplImage im = frame;
		rp.color = &im;
		rp.mpp = Vec2d(100,100);
		rp.gps.pose = Matrix44d::I().ToCvMat();
		rp.gps.pos = Vec3d(0,0,0);

		m_vineWidget->m_cameraMutex.lock();
		m_vineWidget->m_data.Release();
		m_vineWidget->m_data = rp.Clone();
		cvCvtColor(rp.color, m_vineWidget->m_data.color, CV_BGR2RGB);
		m_vineWidget->m_cameraMutex.unlock();

		emit SetMainImage(m_firstTime);

		m_firstTime = false;
	}
	//////////////////////////////////////////////////////
	/// BEGIN CameraThread
	//////////////////////////////////////////////////////
}