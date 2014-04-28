#include "stdafx.h"

#include <vnl/vnl_cross.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_quaternion.h>

#define VNL_INCLUDE
#include "Propello.h"
#include "SaveDlg.h"
#include <QtGui>
#include "SegmentCore.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>


#define DEGFRM_ROW 1
#define FRMVOL_ROW 2
#define FOV_ROW    3
#define MSTART_ROW 5
#define MLOCAT_ROW 6
#define BDEPTH_ROW 8
#define FOCUS_ROW  9
#define DOPMOD_ROW 11
#define DOPPRF_ROW 12
#define DOPWF_ROW  13
#define DOPTX_ROW  14
#define BOXTOP_ROW 16
#define BOXBOT_ROW 17
#define BOXLFT_ROW 18
#define BOXRHT_ROW 19
#define MICRX_ROW 21
#define MICRY_ROW 22
#define CTRX_ROW 24
#define CTRY_ROW 25

#ifndef FIRMWARE_PATH
#define FIRMWARE_PATH "D:/Users/Joey Greer/ultra/trunk/fw/"
#endif

#ifndef SETTINGS_PATH
#define SETTINGS_PATH "D:/Users/Joey Greer/ultra/trunk/dat/"
#endif

#define PRESETS_PATH (SETTINGS_PATH "presets/imaging/")

#define MSG_TIMEOUT  3000    // ms
#define CINE_SIZE    512     // MB

namespace Nf
{
	class ProbeImageCoordTransform : ImageCoordTransform
	{
	private:
		s32 m_frameNum;
		s32 m_fpv;
		s32 m_steps;
		probeInfo m_nfo;

	public:
		ProbeImageCoordTransform(s32 framenum, s32 fpv, s32 steps, probeInfo nfo)
			: m_frameNum(framenum)
			, m_fpv(fpv)
			, m_steps(steps)
			, m_nfo(nfo)
		{
		}

		//adapted from RobotControl.cpp in original version of incremental ultrasteer
		Vec3d Transform(const Vec2d &image) const
		{
			// Determine the incremental frame position in number of frames
			s32 position = m_frameNum % (2*m_fpv);

			// Calculate degrees between frames
			f64 degPerStep = (f64)m_nfo.motorFov / (f64)m_nfo.motorSteps / 1000.0;
			f64 degPerFrm = f64(m_steps)*degPerStep;

			// Calculate angular position in degrees; 0 deg is straight down
			f64 theta;
			s32 middleFrm = ceil(f64(m_fpv)/2.0); 
			//are we sweeping forward or back?  that will dictate theta.
			if (position < m_fpv)
				theta = (position - middleFrm)*degPerFrm; 
			else
				theta = (middleFrm - (position-m_fpv) )*degPerFrm;

			// Get origin and pixel size for image
			s32 mX,mY,oX,oY;
			portaGetMicronsPerPixel(0,mX,mY);
			//this image coordinate corresponds to the middle top pixel of the utlrasound data
			portaGetPixelCoordinates(0, 64, 1, oX, oY, 0);

			Vec3d res;
			res.x = (image.x-oX)*f64(-mX/1000.0);
			f64 imgY_mm = (image.y-oY)*f64(mY/1000.0);
			f64 probeRadius = m_nfo.motorRadius / 1000.0;
			res.y = (probeRadius + imgY_mm)*cos(theta*PI/180);
			res.z = (probeRadius + imgY_mm)*sin(theta*PI/180);

			return res;
		}
	};
}

static IplImage * QImageToIplImage(QImage *im)
{
	IplImage *res = cvCreateImage(cvSize(im->width(), im->height()), IPL_DEPTH_8U, im->depth()/8);
	u8 *psrc = im->scanLine(0);
	for(s32 r=0; r<im->height(); r++) {
		u8 *dst = (u8 *)res->imageDataOrigin+res->widthStep*r;
		u8 *src = psrc+r*im->bytesPerLine();

		memcpy(dst, src, im->width()*(im->depth()/8));
	}
	return res;
}

static QImage * IplImageToQImage(IplImage *im)
{
	QImage *res = NULL;
	if(im->nChannels == 4) {
		res = new QImage(im->width, im->height, QImage::Format::Format_RGB32);
	} else if(im->nChannels == 1) {
		res = new QImage(im->width, im->height, QImage::Format::Format_Indexed8);
		//set up grayscale indexing to identity map
		for(s32 i=0; i<256; i++)
			res->setColor(i, qRgb(i,i,i));
	} else {
		Nf::NTrace("Unrecognized IplImage type\n");
		ASSERT(0); 
	}

	u8 *pdst = res->scanLine(0);
	for(s32 r=0; r<im->height; r++) {
		u8 *dst = pdst+r*res->bytesPerLine();
		u8 *src = (u8 *)im->imageDataOrigin+r*im->widthStep;
		memcpy(dst, src, im->width*im->nChannels);
	}
	return res;
}

Propello::Propello(QWidget* parent) 
	: QMainWindow(parent)
	, m_robotcontrol(this)
	, m_ns(NULL)
{
	setupUi(this);
	setupControls();

	m_firmwarePath = FIRMWARE_PATH;
	m_settingsPath = SETTINGS_PATH;
	m_cineSize = CINE_SIZE;
	m_motorstart = 0;
	m_motorpos = 0.0;
	m_needleScanning = false;
	m_closedLoopSteering = false;
	m_rollInit = false;
	m_insInit = false;
	enableRobotControls(false);
	m_robotcontrol.setTargetX(target_x->value());
	m_robotcontrol.setTargetY(target_y->value());
	m_robotcontrol.setTargetZ(target_z->value());

	connect(this, SIGNAL(showInfo(int)), this, SLOT(onShowInfo(int)));
	connect(wBImage, SIGNAL(tick(int)), this, SLOT(onTick(int)));

	wBImage->setMutex(&m_mutex);

	m_porta = (porta *)0xFFFFFFFF;  //m_porta is never assigned, yet it is checked for some reason
}

Propello::~Propello()
{
	portaShutdown();
	if(m_ns)
		delete m_ns;
}

// create action groups so menus can be checkable
void Propello::setupControls()
{
	QActionGroup* agMain = new QActionGroup(this);
	agMain->addAction(mMain2);
	agMain->addAction(mMain3);
	agMain->addAction(mMain4);

	QActionGroup* agPCI = new QActionGroup(this);
	agPCI->addAction(mPCI2);
	agPCI->addAction(mPCI3);

	QActionGroup* agStorage = new QActionGroup(this);
	agStorage->addAction(mStoreAll);
	agStorage->addAction(mStoreForward);
	agStorage->addAction(mStoreBackward);
}

// allow user to change firmware path
void Propello::getFirmwarePath()
{
	bool ok = false;
	QString text = QInputDialog::getText(this, tr("Enter Firmware Path"), tr("Firmware Path:"), QLineEdit::Normal, m_firmwarePath, &ok);

	if (ok && !text.isEmpty())
	{
		m_firmwarePath = text;
	}
}

// allow user to change settings path
void Propello::getSettingsPath()
{
	bool ok = false;
	QString text = QInputDialog::getText(this, tr("Enter Settings Path"), tr("Settings Path:"), QLineEdit::Normal, m_settingsPath, &ok);

	if (ok && !text.isEmpty())
	{
		m_settingsPath = text;
	}
}

// allow user to change cine size
void Propello::setCineSize()
{
	bool ok = false;
	int cine = QInputDialog::getInt(this, tr("Enter Cine Size"), tr("Cine Size (MB):"), m_cineSize, 32, 512, 1, &ok);

	if (ok)
	{
		m_cineSize = cine;
	}
}

// display the system ID for licensing
void Propello::showSystemID()
{
	char sysid[255] = "";
	portaGetSystemId(sysid, 255);

	QMessageBox mb;
	mb.setWindowTitle(tr("System ID"));
	mb.setText((QString("%1").arg(sysid)));
	mb.exec();
}

void Propello::onTick(int framenum)
{
#if 0
	if (header && m_probeInfo.motorized && m_probeInfo.motorHomeSensor)
	{
		showInfo(header);
	}
#endif

#if 1
#endif

	// In needle scanning we only want to capture a single volume, so stop the probe if 
	// this is the case.
	int disp = 0;
	if (m_needleScanning && portaGetDisplayFrameCount(disp) == portaGetParam(prmMotorFrames))
	{
		stopScanningNeedle();
	}

	// In closed loop steering we have to process each frame, and run control loop after 
	// a full volume has been captured
	if( m_closedLoopSteering )
	{
		vnl_vector<double> z(6);
		bool zUpdateAvail, steeringComplete; 

		zUpdateAvail = false;
		if(m_mutex.tryLock() && m_ns) {

			//QImageToIplImage creates a deep copy of bmode/doppler image data
			//therefore we can unlock after these functions are called.
			IplImage *bmode = QImageToIplImage(wBImage->getBmode());
			IplImage *doppler = QImageToIplImage(wBImage->getDoppler());

			m_mutex.unlock();

			probeInfo nfo; portaGetProbeInfo(nfo);
			s32 fpv = portaGetParam(prmMotorFrames);
			Nf::ProbeImageCoordTransform transform(framenum, fpv, portaGetParam(prmMotorSteps), nfo);
			Nf::PolyCurve model;
			IplImage *dis = m_ns->UpdateModel(&model, doppler, bmode, (Nf::ImageCoordTransform *)&transform);
			wBImage->SetDisplayImage(IplImageToQImage(dis));


			//For now, update is available every sweep.
			zUpdateAvail = framenum > 0 && ((framenum%fpv)==0);

			//Translate tip estimate to vector of form
			//[tipPos tipOrientation] where tipOrientation is axis-angle representation
			if(zUpdateAvail)
				z = PolyCurveToNeedleTip(&model);

			cvReleaseImage(&bmode);
			cvReleaseImage(&doppler);
		} else if(!m_ns){
			wBImage->SetDisplayImage(wBImage->getDoppler());
			m_mutex.unlock();
		}

		if( zUpdateAvail )
		{
			// Run the control loop (includes UKF and control loop)
			steeringComplete = m_robotcontrol.runControlLoop(z);

			// If the controller reports steering is complete, stop closed-loop steering
			if(steeringComplete)
			{
				stopScanningNeedle();
				m_robotcontrol.stopClosedLoopSteering();
				DisplayErrorBox(1);
			}
		}
	}
}

void Propello::onShowInfo(int header)
{
	int extraInfo = (header & ~0x0000FFFF) >> 16;
	int endStop = ((extraInfo & 0x00000001) > 0 ? 1 : 0);
	int ccwBar = ((extraInfo & 0x00000002) > 0 ? 1 : 0);
	int stepCount = (extraInfo & 0x0000FFFC);

	wStatus->showMessage(QString(tr("Motor EndStop: %1, CCWBar: %2, Stepcount: %3")).arg(endStop).arg(ccwBar).arg(stepCount));
}

// initialize the ultrasound engine, populate probes, load lists
void Propello::initHardware()
{
	int usm = mMain2->isChecked() ? 2 : (mMain3->isChecked() ? 3 : 4);
	int pci = mPCI2->isChecked() ? 2 : 3;
	int channels = (usm == 2) ? 32 : 64;

	if (!portaInit(m_cineSize * 1024 * 1024, m_firmwarePath.toAscii(), m_settingsPath.toAscii(), "D:/Ultrasonix Settings/", "D:/LUTS/",
		usm, pci, 0, 1, channels))
	{
		QMessageBox mb;
		mb.setText(tr("Propello could not initialize ultrasound"));
		mb.exec();
	}
	else
	{
		wStatus->showMessage(tr("Successfully initialized ultrasound"), MSG_TIMEOUT);
		wDetect->setEnabled(true);
	}

	//uncheck the probe selection button
	wDetect->setChecked(false);
}

// start automatic imaging
void Propello::runImage()
{
	bool running = portaIsImaging();
	if (!running)
	{
		if (portaIsConnected() && portaRunImage())
		{
			double vps = (double)portaGetFrameRate() / (double)portaGetParam(prmMotorFrames);
			wStatus->showMessage(QString(tr("Imaging Running @ FPS: %1Hz, VPS: %2Hz")).arg(portaGetFrameRate()).arg(vps));
		}
	}
}

// stop automatic imaging
void Propello::stopImage()
{
	if (portaIsConnected() && portaStopImage())
	{
		int disp = 0, vols = portaGetDisplayFrameCount(disp) / portaGetParam(prmMotorFrames);
		wStatus->showMessage(QString(tr("Imaging Stopped. Acquired %1 frames, %2 volumes.")).arg(portaGetDisplayFrameCount(disp)).arg(
			vols));

		wCine->setRange(0, portaGetFrameCount(0));
		wCine->setValue(wCine->maximum());

	}
}

// start imaging for single scan
void Propello::scanNeedle()
{
	bool scanningStarted = startScanningNeedle();
	if( scanningStarted )
		m_needleScanning = true;
}

// start imaging 
bool Propello::startScanningNeedle()
{
	bool running = portaIsImaging();
	if(!running)
	{
		// Activate scanning
		if (portaIsConnected() && portaRunImage())
		{
			// Activate vibration for Doppler scan
			m_robotcontrol.setVibration(true);
			double vps = (double)portaGetFrameRate() / (double)portaGetParam(prmMotorFrames);
		}
		return true;
	}
	else
	{
		return false;
	}
}



// after a single scan has been completed, process and apply control law
void Propello::stopScanningNeedle()
{
	// Deactivate vibration 
	m_robotcontrol.setVibration(false);
	// Deactivate scanning
	portaStopImage();
	wCine->setRange(0, portaGetFrameCount(0));
	wCine->setValue(wCine->maximum());
	m_needleScanning = false;
	m_closedLoopSteering = false;
	int disp = 0, vols = portaGetDisplayFrameCount(disp) / portaGetParam(prmMotorFrames);
}

// detect a motorized probe
void Propello::onDetect()
{
	int code;
	char name[80];
	
	if (m_porta && portaIsConnected())
	{
		code = (char)portaGetProbeID(0);

		// select the code read, and see if it is motorized
		if (portaSelectProbe(code) && portaGetProbeInfo(m_probeInfo) && m_probeInfo.motorized)
		{
			if (portaGetProbeName(name, 80, code))
			{
				wDetect->setText(name);
				wProbeAttributes->setEnabled(true);
			}

			// the 3D/4D probe is always connected to port 0
			int rv = portaActivateProbeConnector(0);

			// find and load master preset
			if (loadMasterPreset())
			{
				wCaptureMethod->setEnabled(true);
				wDataSelect->setEnabled(true);
				onCaptureChange(0);
				onDataSelection(0);
			}
		}
		// if probe is motorless
		else
		{
			wDetect->setText(tr("No Motor!"));
		}
	}
}

// load master for current probe
bool Propello::loadMasterPreset()
{
	int i, probe1 = -1, probe2 = -1, probe3 = -1;
	QDir dir;
	QString file, master, path = SETTINGS_PATH;
	path.append("presets/imaging/");
	dir.setPath(path);

	QStringList filters;
	filters << "*.xml";
	dir.setNameFilters(filters);

	QStringList files = dir.entryList(QDir::Files);

	for (i = 0; i < files.size(); i++)
	{
		file = dir.path() + "/" + files[i];

		portaGetPresetProbeID(file.toAscii(), probe1, probe2, probe3);
		if (probe1 == portaGetCurrentProbeID() || probe2 == portaGetCurrentProbeID() || probe3 == portaGetCurrentProbeID())
		{
			if (portaIsMasterPreset(file.toAscii()))
			{
				master = file;
				break;
			}
		}
	}

	if (master.length())
	{
		if (portaLoadPreset(master.toAscii()))
		{
			return loadMode(BMode, 0);
		}
	}

	return false;
}

// user changed capture method (auto vs manual)
void Propello::onCaptureChange(int index)
{
	bool running = portaIsImaging();

	// ensure we are stopped
	if (running)
	{
		portaStopImage();
	}

	s32 width, height;
	if(!portaGetDisplayDimensions(0, width, height)) {
		Nf::NTrace("Error getting display dimensions\n");
		ASSERT(0);
	}

	if(m_ns) 
		delete m_ns;
	m_ns = new Nf::NeedleSegmenter(width, height);

	portaSetParamI(prmMotorStatus, index == 0 ? 1 : 0);
	wStoreVolumes->setEnabled(true);
	wHomeMotor->setEnabled(index == 1);

	if (running)
	{
		portaRunImage();
	}
}

// user selected a new mode
void Propello::onDataSelection(int index)
{
	mode = BMode;
	int rfmode = 0;

	if (index >= 0 && index < 3)
	{
		mode = BMode;
	}
	else if (index == 3)
	{
		mode = MMode;
	}
	else if (index == 4)
	{
		mode = ColourMode;
	}

	loadMode(mode, 0);
}

// initialize the imaging mode
bool Propello::loadMode(int mode, int rfmode)
{
	// select best settings
	portaSetParamI(prmBLineDensity, 128);

	// create mode
	if (!portaInitMode((imagingMode)mode))
	{
		QMessageBox mb;
		mb.setText(tr("Porta could not initialize the imaging mode"));
		mb.exec();
		return false;
	}

	wBImage->init(0);

	if (mode = ColourMode)
	{
		wBImage->loadColorMap(":/res/map.bmp");
	}

	portaSetParamI(prmBFocusCount,1);

	refreshParams();

	return true;
}

// show probe details
void Propello::onProbeAttributes()
{
	QString msg;
	probeInfo nfo;

	if (m_porta && portaGetProbeInfo(nfo))
	{
		msg = QString("Base(Radius:%1°, Elems: %2, Pitch: %3µm) Motor(FOV: %4°, Radius: %5°, Steps: %6, Deg/Step: %7°)")
			.arg(nfo.radius / 1000.0).arg(nfo.elements).arg(nfo.pitch).arg(nfo.motorFov / 1000.0).arg(nfo.motorRadius / 1000.0).arg(
			nfo.motorSteps).arg((double)nfo.motorFov / (double)nfo.motorSteps / 1000.0);

		wStatus->showMessage(msg);
	}
}

// increment parameter
void Propello::incParam()
{
	QString prm;
	int max = 0;
	probeInfo nfo;

	if (m_porta)
	{
		portaGetProbeInfo(nfo);
		max = nfo.motorFov / 1000;
	}

	if (!wParams->currentItem())
	{
		return;
	}

	portaRect Col_Box, B_Box;

	switch (wParams->currentItem()->row())
	{
	case DEGFRM_ROW: prm = prmMotorSteps;
		break;
	case FRMVOL_ROW: prm = prmMotorFrames;
		break;
	case MSTART_ROW: m_motorstart = (m_motorstart < max) ? m_motorstart + 1 : max;
		break;
	case MLOCAT_ROW: stepMotor(true);
		break;
	case BDEPTH_ROW: prm = prmBImageDepth;
		break;
	case DOPMOD_ROW: prm = prmColor_Mode;
		break;
	case DOPPRF_ROW: prm = prmColor_PRP;
		break;
	case DOPWF_ROW: prm = prmColor_WF;
		break;
	case DOPTX_ROW: prm = prmColorTxFreq;
		break;
	case FOCUS_ROW: prm = prmBFocusDepth;
		break;
	case BOXBOT_ROW:
		portaGetParamR(prmBImageRect,B_Box);
		portaGetParamR(prmColorBox,Col_Box);
		Col_Box.bottom = (Col_Box.bottom < B_Box.bottom - 19) ? Col_Box.bottom + 10 : B_Box.bottom - 10;
		portaSetParamR(prmColorBox,Col_Box);
		break;
	case BOXTOP_ROW:
		portaGetParamR(prmColorBox,Col_Box);
		Col_Box.top = (Col_Box.top < Col_Box.bottom - 29) ? Col_Box.top + 10: Col_Box.bottom - 20;
		portaSetParamR(prmColorBox,Col_Box);
		break;
	case BOXLFT_ROW:
		portaGetParamR(prmColorBox,Col_Box);
		Col_Box.left = Col_Box.left + 10;
		portaSetParamR(prmColorBox,Col_Box);
		break;
	case BOXRHT_ROW:
		portaGetParamR(prmColorBox,Col_Box);
		Col_Box.right = Col_Box.right + 10; 
		portaSetParamR(prmColorBox,Col_Box);
		break;
	}

	if (!prm.isEmpty())
	{
		portaCycleParam(prm.toAscii().constData(), true);
	}

	refreshParams();
}

// decrement parameter
void Propello::decParam()
{
	QString prm;

	if (!wParams->currentItem())
	{
		return;
	}

	portaRect Col_Box, B_Box;

	switch (wParams->currentItem()->row())
	{
	case DEGFRM_ROW: prm = prmMotorSteps;
		break;
	case FRMVOL_ROW: prm = prmMotorFrames;
		break;
	case MSTART_ROW: m_motorstart = (m_motorstart > 0) ? m_motorstart - 1 : 0;
		break;
	case MLOCAT_ROW: stepMotor(false);
		break;
	case BDEPTH_ROW: prm = prmBImageDepth;
		break;
	case DOPMOD_ROW: prm = prmColor_Mode;
		break;
	case DOPPRF_ROW: prm = prmColor_PRP;
		break;
	case DOPWF_ROW: prm = prmColor_WF;
		break;
	case DOPTX_ROW: prm = prmColorTxFreq;
		break;
	case FOCUS_ROW: prm = prmBFocusDepth;
		break;
	case BOXTOP_ROW:
		portaGetParamR(prmBImageRect,B_Box);
		portaGetParamR(prmColorBox,Col_Box);
		Col_Box.top = (Col_Box.top > B_Box.top + 19) ? Col_Box.top - 10 : B_Box.top + 10;
		portaSetParamR(prmColorBox,Col_Box);
		break;
	case BOXBOT_ROW:
		portaGetParamR(prmColorBox,Col_Box);
		Col_Box.bottom = (Col_Box.bottom > Col_Box.top + 29) ? Col_Box.bottom - 10 : Col_Box.top + 20;
		portaSetParamR(prmColorBox,Col_Box);
		break;
	case BOXLFT_ROW:
		portaGetParamR(prmColorBox,Col_Box);
		Col_Box.left = Col_Box.left - 10;
		portaSetParamR(prmColorBox,Col_Box);
		break;
	case BOXRHT_ROW:
		portaGetParamR(prmColorBox,Col_Box);
		Col_Box.right = Col_Box.right - 10;
		portaSetParamR(prmColorBox,Col_Box); 
		break;
	}    

	if (!prm.isEmpty())
	{
		portaCycleParam(prm.toAscii().constData(), false);
	}

	refreshParams();
}

// applies parameter when user has edited (currently motor home only)
void Propello::onApply()
{
	m_motorstart = wParams->item(5, 0)->text().toInt();

	// if in manual mode and motor start option is highlighted
	// move the motor to start position and update the motor current location
	if (wParams->currentItem()->row() == 5 && (wCaptureMethod->currentIndex() == 1))
	{
		m_motorpos = m_motorstart;
		onHomeMotor();
	}
}

// homes the motor
void Propello::onHomeMotor()
{
	bool running = portaIsImaging();

	if (running)
	{
		portaStopImage();
	}

	m_motorpos = portaGoToPosition(m_motorstart);

	if (running)
	{
		portaRunImage();
	}

	refreshParams();
}

// steps the motor manually
void Propello::stepMotor(bool fwd)
{
	if (wCaptureMethod->currentIndex() == 1)
	{
		bool running = portaIsImaging();
		double stepamt;

		// use the step amount from the automatic acquisition
		int steps = portaGetParam(prmMotorSteps);

		if (running)
		{
			portaStopImage();
		}

		stepamt = portaStepMotor(!fwd, steps);

		if (running)
		{
			portaRunImage();
		}

		m_motorpos += stepamt;
	}
}

// refreshes table parameters
void Propello::refreshParams()
{
	probeInfo nfo;
	portaGetProbeInfo(nfo);

	int steps = portaGetParam(prmMotorSteps);
	double degPerStep = (double)nfo.motorFov / (double)nfo.motorSteps / 1000.0;

	int mx, my, cx, cy;

	if(portaGetPixelCoordinates(0, 64, 1, cx, cy, 0))
	{
		wParams->item(CTRX_ROW, 0)->setText(QString("%1").arg(cx));
		wParams->item(CTRY_ROW, 0)->setText(QString("%1").arg(cy));
	}
	if (portaGetMicronsPerPixel(0, mx, my))
	{
		wParams->item(MICRX_ROW, 0)->setText(QString("%1").arg(mx));
		wParams->item(MICRY_ROW, 0)->setText(QString("%1").arg(my));
	}
	wParams->item(DEGFRM_ROW, 0)->setText(QString("%1").arg((double)steps * degPerStep));
	wParams->item(FRMVOL_ROW, 0)->setText(QString("%1").arg(portaGetParam(prmMotorFrames)));
	wParams->item(FOV_ROW, 0)->setText(QString("%1").arg((double)portaGetParam(prmMotorFrames) * degPerStep * (double)steps));
	wParams->item(MSTART_ROW, 0)->setText(QString("%1").arg(m_motorstart));
	wParams->item(MLOCAT_ROW, 0)->setText(QString("%1").arg(m_motorpos));
	wParams->item(BDEPTH_ROW, 0)->setText(QString("%1").arg((double)portaGetParam(prmBImageDepth)));
	wParams->item(FOCUS_ROW, 0)->setText(QString("%1").arg((double)portaGetParam(prmBFocusDepth)/1000.0));

	if(mode == ColourMode)
	{
		double PRF_Hz, WF_Hz, Tx_kHz;
		wParams->item(DOPMOD_ROW, 0)->setText(getParamStringValue(prmColor_Mode));
		PRF_Hz = 1000000/portaGetParam(prmColor_PRP);
		WF_Hz = portaGetParam(prmColor_WF)*PRF_Hz/1000.0;
		Tx_kHz = portaGetParam(prmColorTxFreq)/1000.0;
		wParams->item(DOPPRF_ROW, 0)->setText(QString("%1").arg(PRF_Hz));
		wParams->item(DOPWF_ROW, 0)->setText(QString("%1").arg(WF_Hz));
		wParams->item(DOPTX_ROW, 0)->setText(QString("%1").arg(Tx_kHz));
		portaRect m_ColorBox;
		portaGetParamR(prmColorBox,m_ColorBox);
		wParams->item(BOXTOP_ROW,0)->setText(QString("%1").arg(m_ColorBox.top));
		wParams->item(BOXBOT_ROW,0)->setText(QString("%1").arg(m_ColorBox.bottom));
		wParams->item(BOXLFT_ROW,0)->setText(QString("%1").arg(m_ColorBox.left));
		wParams->item(BOXRHT_ROW,0)->setText(QString("%1").arg(m_ColorBox.right));
	}
	else
	{
		wParams->item(DOPMOD_ROW,0)->setText("N/A");
		wParams->item(DOPPRF_ROW,0)->setText("N/A");
		wParams->item(DOPWF_ROW,0)->setText("N/A");
		wParams->item(DOPTX_ROW,0)->setText("N/A");
		wParams->item(BOXTOP_ROW,0)->setText("N/A");
		wParams->item(BOXBOT_ROW,0)->setText("N/A");
		wParams->item(BOXLFT_ROW,0)->setText("N/A");
		wParams->item(BOXRHT_ROW,0)->setText("N/A");
	}
}

QString Propello::getParamStringValue(QString prm)
{
	prm = prm.trimmed();
	portaVarType type = pVariableUnknown;

	if (portaGetParamType(prm.toAscii(), type))
	{
		char val[100];
		int intVal;
		portaGainCurve gVal;
		portaCurve cVal;
		portaRect rVal;
		portaPoint pVal;
		std::vector< int > arrayVal(200);
		int arrayValCount = arrayVal.size();

		switch (type)
		{
		case pInteger:
			{
				if (portaGetParamI(prm.toAscii(), intVal))
				{
					return QString("%1").arg(intVal);
				}
			}
			break;
		case pString:
			{
				if (portaGetParamS(prm.toAscii(), val, 100))
				{
					return QString("%1").arg(val);
				}
			}
			break;
		case pRectangle:
			{
				if (portaGetParamR(prm.toAscii(), rVal))
				{
					return QString("[R] %1,%2,%3,%4").arg(rVal.left).arg(rVal.top).arg(
						rVal.right).arg(rVal.bottom);
				}
			}
			break;
		}

		return "n/a";
	}
}

// user scrolled cine
void Propello::cineScroll(int value)
{
	if (!portaIsImaging())
	{
		portaProcessCineImage(0, value);
	}
}

// store post-scan converted B volumes
void Propello::onStoreVolume()
{
	SaveDlg dlg(((wCaptureMethod->currentIndex() == 0) ? storeAutoVolume : storeManualVolume));
	dlg.exec();
}

// initializes roll stage of NS robot
void Propello::InitializeRoll(void)
{
	m_robotcontrol.InitializeRoll("COM6");
	m_rollInit = true;
	r_initRoll->setDisabled(true);
	if(m_insInit && m_rollInit)
		enableRobotControls(true);
}

// initializes insertion stage of NS robot
void Propello::InitializeInsertion(void)
{
	// Display a message box to warn that movement will occur
	QMessageBox msgBox;
	msgBox.setWindowTitle("HAZARD");
	msgBox.setText("Ensure linear slide is clear before continuing to calibration.");
	QPushButton *continueButton = msgBox.addButton("Calibrate", QMessageBox::ActionRole);
	QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);
	msgBox.exec();
	// Initialize the translation stage
	if (msgBox.clickedButton() == continueButton) 
	{
		m_robotcontrol.InitializeInsertion("COM7");
		m_insInit = true;
	}
	if(m_insInit && m_rollInit)
		enableRobotControls(true);
	r_initInsertion->setDisabled(true);
}

void Propello::DisplayErrorBox(int i)
{
	// Display a message box to indicate the target depth has been reached
	QMessageBox msgBox;
	switch (i){
	case 1:
		msgBox.setWindowTitle("Target Depth Reached");
		msgBox.setText("The needle tip has reached the z-axis depth of the target.");
	}
	QPushButton *continueButton = msgBox.addButton("OK", QMessageBox::ActionRole);
	QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);
	msgBox.exec();
}

void Propello::applyControl()
{	
	// Activate 3D imaging
	bool scanningStarted = startScanningNeedle();
	if( scanningStarted )
		m_closedLoopSteering = true;
	// Set robot to closed-loop steering
	m_robotcontrol.beginClosedLoopSteering();
}

void Propello::Insert2mm(void)
{
	m_robotcontrol.InsertIncremental(-10.0);
}

void Propello::Retract2mm(void)
{
	m_robotcontrol.InsertIncremental(10.0);
}

void Propello::Insert30rpm(void)
{
	m_robotcontrol.SetInsertionVelocity(-30);
}

void Propello::Retract30rpm(void)
{
	m_robotcontrol.SetInsertionVelocity(30);
}

void Propello::StopInsertion(void)
{
	m_robotcontrol.SetInsertionVelocity(0);
}

void Propello::SetMaxPosVelocity(void)
{
	m_robotcontrol.SetRotationVelocity(10000);
}

void Propello::SetMaxNegVelocity(void)
{
	m_robotcontrol.SetRotationVelocity(-10000);
}

void Propello::StopRoll(void)
{
	m_robotcontrol.SetRotationVelocity(0);
}

void Propello::RotPosInc(void)
{
	m_robotcontrol.RotateIncremental(10.0);
}

void Propello::RotNegInc(void)
{
	m_robotcontrol.RotateIncremental(-10.0);
}

void Propello::homeRoll(void)
{
	m_robotcontrol.homeRoll();
}

void Propello::homeInsertion(void)
{
	m_robotcontrol.homeInsertion();
}

void Propello::enableRobotControls(bool enable)
{
	if(enable)
	{
		r_stopRoll->setEnabled(true);
		r_homeRoll->setEnabled(true);
		r_maxRollNeg->setEnabled(true);
		r_maxRollPos->setEnabled(true);
		r_incRollPos->setEnabled(true);
		r_incRollNeg->setEnabled(true);
		r_stopIns->setEnabled(true);
		r_homeIns->setEnabled(true);
		r_incInsNeg->setEnabled(true);
		r_incInsPos->setEnabled(true);
		r_maxInsNeg->setEnabled(true);
		r_maxInsPos->setEnabled(true);
		r_scanNeedle->setEnabled(true);
		r_applyControl->setEnabled(true);
	}
	else
	{
		r_stopRoll->setDisabled(true);
		r_homeRoll->setDisabled(true);
		r_maxRollNeg->setDisabled(true);
		r_maxRollPos->setDisabled(true);
		r_incRollPos->setDisabled(true);
		r_incRollNeg->setDisabled(true);
		r_stopIns->setDisabled(true);
		r_homeIns->setDisabled(true);
		r_incInsNeg->setDisabled(true);
		r_incInsPos->setDisabled(true);
		r_maxInsNeg->setDisabled(true);
		r_maxInsPos->setDisabled(true);
		r_scanNeedle->setDisabled(true);
		r_applyControl->setDisabled(true);
	}
}

void Propello::setTargetX(int X)
{
	m_robotcontrol.setTargetX( X );
}

void Propello::setTargetY(int Y)
{
	m_robotcontrol.setTargetY( Y );
}

void Propello::setTargetZ(int Z)
{
	m_robotcontrol.setTargetZ( Z );
}


