/// \class		MotionController
/// \brief		Encapsulates the actual physical motion controller object (MCDC) and handles communication.
/// \details	Specific Faulhaber MDCD3006S commands and parameters are contained within this class along.
///				with motor and encoder specific parameters (whether Faulhaber or Maxon)
///	\author		Andrew Thompson
///	\version	1.0
///	\date		June 18, 2008
/// \warning	Class under construction

#pragma once

#include <string>
#include <cstring>
#include "qstring.h"
#include "SerialCommHelper.h"
#include "DeviceNotification.h"


/********************** Hardware Specific Parameters ******************************/

// Number of encoder ticks for one full rotation of the Maxon motor
#define MAXON_ONE_ROTATION	4000

/**********************************************************************************/

class MotionController 
{
public:

	//Public enums:
	
	/// \brief	The state of the motion controller at any point in time (changes in certain method calls)
	enum MotionControllerState
	{
		Unknown,
		NotInitialized,
		Initialized,
		Verified,
		Moving,
		Stopped
	};

	/// \brief	The mode that describes what responses (in char form) are sent back from the MCDC 
	enum AnswerMode
	{
		NoAsyncResponses = 0,			// No responses are sent
		PermitAsyncResponses = 1,		// Responses are sent for certian commands
		ConfirmAndAsyncResponses = 2,	// Confirmation and responses are sent
		NotKnown = 3					// Unknown due to internal MCDC configuration upon startup
	};

	//Public Members:
	MotionController(int deviceID);
	~MotionController(void);

	bool IsInitialized(void);
	void InitializeDevice(QString comPort);
	void EnableDevice(void);
	void ResetEncoder(long newValue = 0);
	void LockMotor(bool lock=true);

	void SetCorridorValue(int corridor);
	void SetPeakCurrentLimit(int currentLimit);
	void SetPositionLimits(int min, int max);
	void EnablePositionLimits(bool enable=true);
	void EnableNotification(AnswerMode mode);

	//void LoadTextFile(QString filePath);
	//void SendCustomCommand(QString SerialCommand);

	bool SetVelocity(int rpm, DeviceNotification *notify=NULL);
	bool RequestVelocityNotification(DeviceNotification *notify);
	bool IsVelocityVerified(long timeOut);
	bool ChangeMaxMotorVelocity(int maxSpeed);
	
	bool SetPosition(int pos, DeviceNotification *notify=NULL);
	bool RequestPositionNotification(DeviceNotification *notify);
	bool IsPositionVerified(long timeOut);

	long GetPosition(void);
	int GetCurrent(void);

	void SetCalibration(bool calibrated) { m_isCalibrated = calibrated; }
	bool IsCalibrated(void) { return m_isCalibrated; }

	void ClearExistingData(void);
	void Stop(void);

	void HomingSequence(void);
	void setDigitalOutput(bool);

private:
	//Private Members:
	void DisableDevice(void);
	bool ReadCharsAndCompare(QString comparison, long alTimeOut);
	HRESULT CanProcess(void);				//See if current state of object processing is okay for continuation

	//Private fields:
	QString m_command;						//Command string that can be freeley reused inside each method (for efficiency)
	MotionControllerState m_currentState;	//Current State of MCDC device
	const int DEVICE_ID;					//Specific communication ID set internally in MCDC
	CSerialCommHelper m_serial;				//Serial COM port
	bool m_limitsEnabled;					//Bool for whether or not position limits are enabled
	int m_velNotifyCounter;					//Velocity counter for number of times notify has been requested
	int m_posNotifyCounter;					//Position counter for number of times notify has been requested
	bool m_isCalibrated;					//Motor has been calibrated
	AnswerMode m_currentAnswerMode;			//Current answer mode set on the MCDC 
};
