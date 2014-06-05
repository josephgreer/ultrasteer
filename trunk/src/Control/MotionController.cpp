
#include "MotionController.h"
#include "MotionControllerException.h"
#include <cctype>
#include <iostream>
#include "qstring.h"
#include "qdebug.h"
#include <Windows.h>

/// \brief	Default constructor
MotionController::MotionController(int deviceID) : 
	DEVICE_ID(deviceID),
	m_limitsEnabled(false),			//Limits are initialy disabled
	m_isCalibrated(false),			//Initially not at home location
	m_velNotifyCounter(0),			//Set velocity and position counter to zero, increment by one for each 
	m_posNotifyCounter(0)			// associated notification request
{
	m_currentState = NotInitialized;	//Initially uninitialized	
	m_currentAnswerMode = NotKnown;
}

/// \brief		Destructor 
/// \details	Uninitialize serial port and set current state to unknown
MotionController::~MotionController(void)
{
	// Try to uninitialize the serial port
	if( !SUCCEEDED(m_serial.UnInit()) )
	{
		throw MotionControllerException("COM port failed to uninitialize");
	}
	
	m_currentState = Unknown;
}

/// \brief		Checks to see is COM port has already been initialized
/// \pre		none
/// \post		Indicates whether serial port has been previously initialized [i.e Call to Init()]
bool MotionController::IsInitialized(void)
{
	return m_serial.IsConnection();
}

/// \brief		Initialize all MCDC devices
/// \details	To initialize: Kill all devices (just in case they're still moving), re-enable them
///				and then reset potion encoder to 0. Also set current limits.
///	\pre		comPort is a valid com port and the devices are powered up and have been connected via serial
///	\post		MCDC device is initialized and are ready to receive motion profile commands
void MotionController::InitializeDevice(QString comPort)
{
	////std::string my_str = (LPCTSTR)(comPort.utf16());
	//std::string my_str = comPort.toStdString(); 
	//std::string th_str = "COM6";
	// Initialize the serial port with these settings: 115200 Baud, 8 data bits, 1 stop bit and NO parity
	if( m_serial.Init(comPort.toStdString(), 115200, 0, 1, 8) != S_OK )	
	{
		qDebug() << "MotionController : Serial port initialization to MCDC failed | comPort:" << comPort;
		throw MotionControllerException("COM port failed to initialize");
	}

	//Remove any old chars from the serial buffers before starting
	m_serial.Purge();	
	
	//"Start" the serial port 
	m_serial.Start();

	//Allow 'v' or 'p' chars to be sent back from the MCDC device when notification is requested
	EnableNotification(PermitAsyncResponses);

	m_currentState = Initialized;
}

/// \brief		Enable MCDC (Send "EN" command)
///	\pre		MCDC is still connected on the given COM port
///	\post		Motor is Enabled
void MotionController::EnableDevice(void)
{
	//Enable the motion controller
	m_command = QString("%1EN\r").arg(QString::number(DEVICE_ID));
	m_serial.Write( (LPCTSTR)m_command.toStdString().c_str());
}

/// \brief		Disable MCDC (Send "DI" command)
///	\pre		MCDC is still connected on the given COM port
///	\post		Motor is Disabled
void MotionController::DisableDevice(void)
{
	//Disable the motion controller
	m_command = QString("%1DI\r").arg(DEVICE_ID);
	m_serial.Write(m_command.toStdString().c_str());
}

/// \brief		Reset MCDC encoder tick value
/// \details	Use "HO" command to reset internal encoder tick value of MCDC to either 0 if no param is given
///				or to a specific amount if newValue is set
///	\pre		MCDC is still connected on the given COM port
///	\post		Motor is Enabled
void MotionController::ResetEncoder(long newValue/*=0*/)
{
	//Disable the motion controller
	m_command = QString("%1HO%2\r").arg(DEVICE_ID,newValue);
	m_serial.Write(m_command.toStdString().c_str());
}

/// \brief		Lock or unlock the motor based on the input boolean parameter (defaults to lock)
/// \details	Use the kill command ("DI") followed by the enable command ("EN") to lock the motor
///	\pre		MCDC is still connected on the given COM port
///	\post		Motor is locked in its current position or unlocked depending on value of boolean "lock"
void MotionController::LockMotor(bool lock/*=true*/)
{
	if(!lock) DisableDevice();
	else EnableDevice();
}

/// \brief		Set corridor window
/// \details	Uses the 'CORRIDOR' command to set the corridor window. Notifcation for position/velocity 
///				will be sent whenever the motor enters inside the corridor window (i.e. speeds up or slows down
///				notification). This functionality greatly speeds up the maximum bandwidth of the vibration motor
///	\pre		MCDC is still connected on the given COM port
///	\post		Corridor value is set for the particular MCDC
void MotionController::SetCorridorValue(int corridor)
{
	if( FAILED(CanProcess()) ) return;
	
	m_command = QString("%1CORRIDOR%2\r").arg(DEVICE_ID,corridor);
	m_serial.Write(m_command.toStdString().c_str());
}

/// \brief		Set current limit of MCDC device
/// \details	Uses the 'LPC' command to set the maximum allowable MCDC drive current
/// \warning	Be very CAREFUL how you change the safety checks in this function if you ever do.
///				Each motor connected to the MCDC has a maximum allowable current and if exceed for 
///				long periods of time, the motor can be damaged.
///	\pre		MCDC is still connected on the given COM port
///	\post		Current limit set for the particular MCDC
void MotionController::SetPeakCurrentLimit(int currentLimit)
{
	if( FAILED(CanProcess()) ) return;

	//Set Current Limit
	/*** WARNING: Make sure the current limit is not set too high ***/
	if(currentLimit < 0 || currentLimit > 3000)	//If greater than 2A it may be unsafe to operate (i.e damage the motor)
												//Even 2A could be too high for smaller motors so be carefull
	{
		throw MotionControllerException("Current limit may be set too high, re-check value");
		return;
	}

	m_command = QString("%1LPC%2\r").arg(DEVICE_ID,currentLimit);
	m_serial.Write(m_command.toStdString().c_str());
}

/// \brief		Set position limits on MCDC device
/// \details	Uses the 'LL' command to set the minimum and maximum encoder values that the
///				motor can travel. To enable or disable the limits call enablePositionLimits().
///	\pre		MCDC is still connected on the given COM port
///	\post		Position limits enforced or removed depending on the boolean value of enable
void MotionController::SetPositionLimits(int min, int max)
{
	if( FAILED(CanProcess()) ) return;

	m_command = QString("%1LL%2\r").arg(DEVICE_ID,min);
	m_serial.Write(m_command.toStdString().c_str());
	
	m_command = QString("%1LL%2\r").arg(DEVICE_ID,max);
	m_serial.Write(m_command.toStdString().c_str());
}

/// \brief		Enable position limits on MCDC device
/// \details	Uses the 'APL' command to either allow or dis-allow the MCDC device to move past
///				a specific encoder value. The specific encoder values can be changed with setPositionLimits().
///	\pre		MCDC is still connected on the given COM port
///	\post		Position limits enforced or removed depending on the boolean value of enable
void MotionController::EnablePositionLimits(bool enable)
{
	if( FAILED(CanProcess()) ) return;

	m_limitsEnabled = enable;
	int en = 0;
	if(enable) 
	{
		en = 1;
	}
	
	m_command = QString("%1APL%2\r").arg(DEVICE_ID).arg(en);
	m_serial.Write(m_command.toStdString().c_str());
}

/// \brief		Enable MCDC notification mechanism
/// \details	When enabled the MCDC will send back a char for certian events (see ANSW in manual for more info)
/// \pre		MCDC is still connected on the given COM port
/// \post		The MCDC ANSW verification is set to a particular mode (None, Partial or Full)
void MotionController::EnableNotification(AnswerMode mode)
{
	if(mode != NotKnown)
	{
		m_command = QString("%1ANSW%2\r").arg(DEVICE_ID).arg(mode);
		m_serial.Write(m_command.toStdString().c_str());
	}
	
	// set the current answer mode
	m_currentAnswerMode = mode;
}

///// \brief		Load a programing sequence into MCDC device from a text file
///// \pre		MCDC is still connected on the given COM port, Valid text file and file path
///// \post		returns true if reading the text and sending it through serial port was successful
//void MotionController::LoadTextFile(QString filePath)
//{
//	CArray<CString> fileData;
//	Utilities::ReadDataFile(filePath, fileData);
//
//	for(int i=0; i < fileData.GetSize(); i++)
//	{
//		//m_command = fileData.GetAt(i);
//		m_command.Format("%s\r", fileData.GetAt(i));
//		if(m_command.Find('\r') == -1)
//		{
//			// if the command string from the file does not contain the ending character
//			// is a problem and we should throw an exception
//			throw MotionControllerException("Command does not have necessary ending character");
//		}
//		m_serial.Write( (LPCTSTR)m_command );
//		Sleep(10);
//	}
//}
//
///// \brief		Send custom commands to MCDC device
///// \pre		MCDC is still connected on the given COM port
///// \post		command is sent to MCDC motor
//void MotionController::SendCustomCommand(QString SerialCommand)
//{
//	m_command = QString("%1%2\r").arg(DEVICE_ID,SerialCommand);
//	m_serial.Write( (LPCTSTR)m_command.utf16());
//
//}

/// \brief		Spin the motor at a given speed (RPM) and request notification if desired
/// \details	If the notify parameter is set to 'true' then the notificationValue parameter should also be set
///	\pre		MCDC is still connected on the given COM port
///	\post		Motor spins at and maintains a constant velocity (and may be notified when it reaches that particular velocity)
bool MotionController::SetVelocity(int rpm, DeviceNotification *notify/*=NULL*/)
{
	if( FAILED(CanProcess()) ) return false;

	if(m_limitsEnabled) 
	{
		qDebug() << "ERROR: Cannot drive motor in velocity mode when limits are enabled";	
		return false;
	}

	if( notify != NULL )
	{
		RequestVelocityNotification(notify);
	}

	m_command = QString("%1V%2\r").arg(QString::number(DEVICE_ID)).arg(QString::number(rpm));
	m_serial.Write(m_command.toStdString().c_str());

	m_currentState = Moving;

	return true;
}

/// \brief		Starts the motor homing sequence
/// \details	The motor moves until a limit switch is hit, at which point the position is set to zero
///	\pre		The homing velocity and limit behaviors have been previously saved in the MCDC 
///	\post		The motor is stopped and position is set to 0
void MotionController::HomingSequence(void)
{
	if( FAILED(CanProcess()) ) throw MotionControllerException("Cannot perform homing");;
	// Construct the string to send with both the LA and the M commands embedded in a single transmission
	m_command = QString("%1GOHOSEQ\r").arg(DEVICE_ID);
	m_serial.Write(m_command.toStdString().c_str());

	m_currentState = Stopped;
}

/// \brief		Request verification (send back a 'v' char) when a particular velocity is reached
/// \details	If  the 'notify' param does not have a target, then only notify when motor reaches its target velocity
///	\pre		MCDC is still connected on the given COM port
///	\post		MCDC is given a request to notify at a specific velocity when overshot or when the target velocity is reached 
bool MotionController::RequestVelocityNotification(DeviceNotification *notify)
{
	if( FAILED(CanProcess()) ) return false;
		
	if( notify->HasNotifyValue() )
	{
		//Notify when motor overshoots/reaches the 'GetNotifyValue()' position
		m_command = QString("%1NV%2\r").arg(DEVICE_ID).arg(notify->GetNotifyValue());
	
	}
	else
	{
		//Only notify when motor reaches its TARGET position
		m_command = QString("%1NV\r").arg(DEVICE_ID);
	}

	m_serial.Write(m_command.toStdString().c_str());
	++m_velNotifyCounter;
	
	return true;
}

/// \brief		Verify whether the intended velocity was achieved
/// \details	The MCDC velocity notify command returns the character 'v' when the velocity is achieved
///	\pre		A corresponding call to SetVelocity() was made along with the notify parameter set to true
///	\post		Boolean value returned indicating whether or not the velocity, set in SetVelocity(), has been reached
bool MotionController::IsVelocityVerified(long timeOut)
{
	bool success = false;
	
	if( FAILED(CanProcess()) ) return false;

	if(m_velNotifyCounter < 1)
	{
		qDebug() << "MotionController : m_velNotifyCounter=" << m_velNotifyCounter;
		throw MotionControllerException("Velocity verified counter out of sync");
	}

	if( ReadCharsAndCompare("v\r\n", timeOut) )	// "v\r\n"
	{
		success = true;
		--m_velNotifyCounter;
		m_currentState = Verified;
	}

	return success;
}

/// \brief		Rotate the motor to a given position (in encoder tick format) and request notification if necessary
/// \details	If the notify parameter is set to 'true' then the notificationValue parameter should also be set
///	\pre		MCDC is still connected on the given COM port
///	\post		Motor spins to and maintains a given position (i.e. maintains its location at a specific encoder tick) 
bool MotionController::SetPosition(int pos, DeviceNotification *notify/*=NULL*/)
{
	if( FAILED(CanProcess()) ) return false;
	
	if( notify != NULL)
	{
		RequestPositionNotification(notify);
	}
	
	//Construct the string to send with both the LA and the M commands embedded in a single transmission
	m_command = QString("%1LA%2\r%3M\r").arg(DEVICE_ID).arg(pos).arg(DEVICE_ID);
	m_serial.Write(m_command.toStdString().c_str());

	m_currentState = Moving;

	return true;
}

/// \brief		Request verification (send back a 'p' char) when a particular position is reached
/// \details	If  the 'notify' param does not have a target, then only notify when motor reaches its target position
///	\pre		MCDC is still connected on the given COM port
///	\post		MCDC is given a request to notify at a specific position when overshot or when the target position is reached 
bool MotionController::RequestPositionNotification(DeviceNotification *notify)
{
	if( FAILED(CanProcess()) ) return false;
	
	if( notify->HasNotifyValue() )
	{
		//Notify when motor overshoots/reaches the 'GetNotifyValue()' position
		m_command = QString("%1NP%2\r").arg(DEVICE_ID).arg(notify->GetNotifyValue());
	}
	else
	{
		//Only notify when motor reaches its TARGET position
		m_command = QString("%1NP\r").arg(DEVICE_ID);
	}

	m_serial.Write(m_command.toStdString().c_str());
	++m_posNotifyCounter;
		
	return true;
}

/// \brief		Verify whether the intended position was achieved
/// \details	The MCDC position notify command returns the character 'p' when the position is achieved
///	\pre		A corresponding call to SetPosition() was made along with the notify parameter set to true
///	\post		Boolean value returned indicating whether or not the position, set in SetPosition(), has been reached
bool MotionController::IsPositionVerified(long timeOut)
{
	bool success = false;
	
	if( FAILED(CanProcess()) ) 
	{
		return false;
	}

	if(m_posNotifyCounter < 1)
	{
		qDebug() << "MotionController : m_posNotifyCounter=" << m_posNotifyCounter;
		throw MotionControllerException("Position verified counter out of sync");
	}

	// If DEBUG_NO_ROBOT is defined as a preprocessor definition DO NOT call ReadCharsAndCompare() method
	// as there are no characters to receive (just set to true).
	bool equal = true;
	#ifndef DEBUG_NO_ROBOT
		equal = ReadCharsAndCompare("p\r\n", timeOut);
	#endif

	if( equal )
	{
		success = true;
		--m_posNotifyCounter;
		m_currentState = Verified;
	}
	
	return success;
}

/// \brief		Read numCharsToRead from the serial port and compare it to comparison string
/// \details	??? Not sure exactly what -1 does (third param in Read_N), some sort of timeout value
///	\pre		numCharsToRead is the correct number of chars that will be available on the serial port
///	\post		True is returned if contents of 'comparison' string was read from serial port
bool MotionController::ReadCharsAndCompare(QString comparison, long alTimeOut)
{
	bool success = false;
	std::string szData;
	
	int numCharsToRead = comparison.size();

	//Maybe change 3rd parameter to INFINITE since it goes to WaitForSingleObject to get exactly that many chars
	m_serial.Read_N(szData, numCharsToRead, alTimeOut);
		
	QString str = QString::fromStdString(szData);
	if( comparison.compare(str) == 0 )
	{ 
		success = true;
	}
	else
	{
		success = false;
	}
	
	return success;	
}

/// \brief		Clear any data on the serial port
///	\pre		none
///	\post		Any data waiting to be read on the com port is discarded
void MotionController::ClearExistingData(void)
{
	if( FAILED(CanProcess()) ) 
	{
		throw MotionControllerException("Cannot clear data due to motion controller state");
	}

	if( m_serial.Purge() != S_OK )
	{
		throw MotionControllerException("Purging of serial buffer failed");
	}
}

/// \brief		Motion is stopped immediatley
/// \details	All motion with the MCDC device is killed using the "DI" command
///	\pre		MCDC is still connected on the given COM port
///	\post		Motor brakes heavily and stops spinning/moving.
void MotionController::Stop(void)
{
	DisableDevice();	//Send kill command to MCDC (stops the motor instantly)

	m_currentState = Stopped;
}

/// \brief	Change the maximum motor velocity that the motor can spin at
/// \pre	MCDC is still connected on the given COM port
/// \post	MCDC internal mac velocity parameter is set to maxspeed
bool MotionController::ChangeMaxMotorVelocity(int maxSpeed)
{
	if( FAILED(CanProcess()) ) return false;

	//Tell MCDC to change the maximum speed of travel to the given speed param
	m_command = QString("%1SP%2\r").arg(DEVICE_ID).arg(maxSpeed);
	m_serial.Write(m_command.toStdString().c_str());

	return true;
}

///	\brief		Get the current position value of the motor in encoder ticks
///	\details	Send the "pos" command followed by a read of the serial port to get the most current position of the motor
///	\pre		MCDC is still connected 
///	\post		The current position in encoder ticks is returned
long MotionController::GetPosition(void)
{
	if( FAILED(CanProcess()) ) return false;

	m_command = QString("%1POS\r").arg(DEVICE_ID);
	m_serial.Write(m_command.toStdString().c_str());

	
	std::string szRecvData;
	QString position = "";
	char recvChar = '|';
	bool breakFromLoop = false;
	bool CR_recv = false;	//Bool to indicate if Carriage return was received

	while(true)
	{
		while( !m_serial.IsInputAvailable() ) Sleep(1);
		
		//Input is now available...just read it	
		if(m_serial.Read_N(szRecvData, 1, -1) != S_OK)
		{
			throw MotionControllerException("Position read from serial port failed");
		}

		recvChar = *szRecvData.c_str();

		switch(recvChar)
		{
			case '|': break;	//No valid chars have come through the serial port yet
			
			case '\r':	//CR char (0D in Hex) is the second last char to be received
				CR_recv = true;	
				break;
			
			case '\n':	//LF char (0A in Hex) is that last char to be received before breaking from loop
				breakFromLoop = true;	//All chars have now been recv'd correctly
				break;
			
			case '-':		//Minus sign may exist in the first char only, if its doesn't assert
				position.append(recvChar);
				break;
			
			default:
				if( isdigit(recvChar) )
					position.append(recvChar);
				break;
		}

		if(breakFromLoop) break;	//Break from loop if ending char was recv'd
	}
	
	bool ok;
	long pos = position.toLong(&ok, 10); 
	return pos;
}

///	\brief		Get the current/amperage value of the motor in milli-amps (mA) 
///	\details	Send the "grc" command followed by a read of the serial port to get the current amperage of the motor
///	\pre		MCDC is still connected
///	\post		The current in mA is returned
int MotionController::GetCurrent(void)
{
	if( FAILED(CanProcess()) ) return false;

	m_command = QString("%1GRC\r").arg(DEVICE_ID);
	m_serial.Write(m_command.toStdString().c_str());

	std::string szRecvData;
	QString current = "";
	char recvChar = '|';
	bool breakFromLoop = false;
	bool CR_recv = false;	//Bool to indicate if Carriage return was received

	while(true)
	{
		while( !m_serial.IsInputAvailable() ) Sleep(1);

		if(m_serial.Read_N(szRecvData, 1, -1) != S_OK)
		{
			throw MotionControllerException("Current read from serial port failed");
		}

		recvChar = *szRecvData.c_str();

		switch(recvChar)
		{
			case '|': break;	//No valid chars have come through the serial port yet
			
			case '\r':	//CR char (0D in Hex) is the second last char to be received
				CR_recv = true;	
				break;
			
			case '\n':	//LF char (0A in Hex) is that last char to be received before breaking from loop
				breakFromLoop = true;	//All chars have now been recv'd correctly
				break;
			
			default:
				if( isdigit(recvChar) )
					current.append( recvChar );
				break;
		}

		if(breakFromLoop) break;	//Break from loop if ending char was recv'd
	}

	bool ok;
	long amperage = current.toInt(&ok, 10); 
	return amperage;
}

/// \brief		Return whether or not the object is in an approapriate place to continue with processing
///	\pre		none
///	\post		Either a failed or okay stat is returned (used FAILED() or SUCCEED()
HRESULT MotionController::CanProcess(void)
{
	switch ( m_currentState ) 
	{
		case Unknown : return E_FAIL;
		case NotInitialized : return E_FAIL;
		case Verified : return S_OK;
		case Moving : return S_OK;
		case Initialized : return S_OK;
		case Stopped : return S_OK;
		default: throw MotionControllerException("Current state should not occur");
	}

	return E_FAIL;
}

/// \brief		Set the digital output of the MCDC
///	\details	Sets the FAULT pin to either high-impedance or GND
/// \pre		MCDC DIGOUT is set to true
///	\post		FAULT output pin is set based on OnOff
void  MotionController::setDigitalOutput(bool OnOff)
{
	if(!IsInitialized())
		return;

	if( FAILED(CanProcess()) ) throw MotionControllerException("Cannot control output");;
	
	// Construct the string to send with the appropriate command
	if( OnOff)
		m_command = QString("%1SO\r").arg(DEVICE_ID);
	else
		m_command = QString("%1CO\r").arg(DEVICE_ID);
	
	m_serial.Write(m_command.toStdString().c_str());
}
	
	

