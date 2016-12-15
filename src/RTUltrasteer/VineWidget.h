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

namespace Nf
{
	class BasicThread;
	class SerialReceiveThread; 
	class CameraThread;

	enum TAPE_ROBOT_STATE {
		TRS_FREE_GROWING = 0,
		TRS_AVOIDING_OBSTACLE = 1,
	};

	enum TAPE_ROBOT_ACTION {
		TRA_DO_NOTHING = 0,
		TRA_TURN_POS = 1,
		TRA_TURN_NEG = 2,
	};

#define MIN_DISTANCE_TO_OBSTACLE 60

	struct ObstacleAvoidanceParams
	{
		TAPE_ROBOT_ACTION turnDir;														// Direction we turned to avoid obstacle
		s32 beginIndex;																				// Actuator index at which avoidance was initiated
	};

  class VineWidget : public ResizableQWidget, public ParameterCollection, public vtkInteractorStyleImage
  {
    Q_OBJECT 

		friend class BasicThread;
		friend class CameraThread;
		friend class SerialReceiveThread;

	public:
    vtkTypeMacro(VineWidget, vtkInteractorStyleImage);

  protected:
    std::tr1::shared_ptr < ImageViewerWidget> m_imageViewer;
    std::tr1::shared_ptr < TapeRobotWidget > m_tapeWidget;
    QGridLayout *m_layout;

		s32 m_oddDirection;																		// Direction odd actuators move you in the image
		s32 m_actuatorIndex;																	// Next actuator to be released
		s32 m_obstacleAvoidanceActuatorStartIndex;						// Actuator index at which obstacle avoidance maneuvar began
		Vec2d m_growingDirection;															// The direction the vine is growing in the image
		TAPE_ROBOT_STATE m_controlState;											// What state are we controling the robot in
		TAPE_ROBOT_ACTION m_queuedAction;											// What action do we want the robot to take when the robot reaches its next tape
		bool m_pop;																						// Whether to pop the tape or not. Based on m_queuedAction and m_oddDirection. Used for monitoring only.
		ObstacleAvoidanceParams m_obstacleAvoidanceParams;		// Parameters of obstacle avoidance maneuvar

		vtkSmartPointer < vtkPointPicker > m_pointPicker;

		std::tr1::shared_ptr < Nf::FileParameter > m_videoFile;
		std::tr1::shared_ptr < Nf::BoolParameter > m_useWebcam;
		std::tr1::shared_ptr < Nf::BoolParameter > m_run;
		std::tr1::shared_ptr < Nf::Vec3dParameter > m_lowerBounds;
		std::tr1::shared_ptr < Nf::Vec3dParameter > m_upperBounds;
		std::tr1::shared_ptr < Nf::Vec3dParameter > m_lowerBoundsObstacle;
		std::tr1::shared_ptr < Nf::Vec3dParameter > m_upperBoundsObstacle;
		std::tr1::shared_ptr < Nf::Vec3dParameter > m_calibratePadding;
		std::tr1::shared_ptr < Nf::IntParameter > m_comPort;
		std::tr1::shared_ptr < Nf::BoolParameter > m_serialInit;
		std::tr1::shared_ptr < Nf::EnumParameter > m_displayMode;
		std::tr1::shared_ptr < Nf::IntParameter > m_calibrateObject; // 0 = Robot, 1 = Obstacles

		CLASS_CALLBACK(SetupVideoInput, VineWidget);
		void SetupVideoInput();

		CLASS_CALLBACK(InitSerial, VineWidget);
		void InitSerial();
		
		std::tr1::shared_ptr < SerialReceiveThread > m_serialThread;
		std::tr1::shared_ptr < CameraThread > m_cameraThread;

		QMutex m_cameraMutex;

		std::tr1::shared_ptr < cv::VideoCapture > m_cap;
		std::tr1::shared_ptr < CSerial > m_serial;

		RPData m_data;
		Vec2d m_imCoords;

	signals:
		void stopCameraThread();
		void stopSerialReceiveThread();

	public slots:
		void SetImage(bool resetView);
		void HWButtonPushed();
		void ActuatorIncrement(s32 index);
		void UpdateText(QString text);
		void SetRobotAction(TAPE_ROBOT_ACTION action);

  public:
    VineWidget(QWidget *parent, const char *name = "Tape Robot Widget");
    ~VineWidget();
    virtual void UpdateSize(QSize sz);
    virtual void UpdateGeometry();

		virtual void OnLeftButtonDown();

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
		void incrementActuator(s32 index);
		void textUpdate(QString text);

	protected:
		char *m_data;
		void execute();
	};

	
	class CameraThread : public BasicThread
	{
		Q_OBJECT

	public:
		CameraThread(VineWidget *vine);

	signals:
		void SetMainImage(bool);
		void RobotActionSet(TAPE_ROBOT_ACTION action);

	public slots:
		void ActuatorIncrement(s32 idx);

	protected:
		bool m_firstTime; 

		std::pair < std::vector < Squarei >, cv::Mat > CalculateBoundingRects(const cv::Mat hsv, const Vec3d &lb, const Vec3d &ub);
		std::pair < Squarei, Vec2d > FindRobotParameters(const std::vector < Squarei > &rects, const cv::Mat mask);
		Vec2d FindClosestObstacleDelta(Vec2d headPos, const std::vector < Squarei > &rects);
		void DrawDetails(cv::Mat &im, const Vec2d &headCen, const std::vector < Squarei > &obstacleRects, const Squarei &robotRect);

		void setup();
		void execute();
	};
}

#endif // VINEWIDGET_H
