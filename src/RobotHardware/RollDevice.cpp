#include "RollDevice.h"
//#include "Utilities.h"
#include  <math.h>
#include "qstring.h"
#include "qdebug.h"

/********************** Hardware Specific Parameters ******************************/
#define ROLL_DEVICE_ID			0						//Network ID of the roll MCDC	
#define CORRIDOR				20						//Default corridor value
#define FAULHABER_ONE_ROTATION	400						//# of encoder ticks in one full rotation (without factoring in the gear ratio) 
#define SPUR_GEAR_RATIO			(1539.0/65.0*4.0/0.5)	//Gear ratio for the maxon motor gearhead + pinion gears
#define MIN_ANGLE_LIMIT			-90						//Minimum encoder limit that the motor will not be able to travel past
#define MAX_ANGLE_LIMIT			-90						//Maximum encoder limit that the motor will not be able to travel past
#define CURRENT_LIMIT			3000					//Faulhaber motor peak current limit of 3000mA 	
#define MAX_VELOCITY			377						//Roll device velocity when it is not moving in no increments mode
/**********************************************************************************/

bool RollDevice::m_created = false;

/// \brief		Constructor: Initializes roll device ID and sets current and position limits.	
RollDevice::RollDevice(void) : 
MotionController(ROLL_DEVICE_ID),
m_currentAngle(NULL),
m_isRollerLocked(false)
{
	if(m_created)
	{
		qDebug() << "ERROR: Only one instance of RollDevice is allowed";
	}

	m_created = true;
	
}

/// \brief		Destructor: Stops motion if there happens to be any	
RollDevice::~RollDevice(void)
{
	MotionController::Stop();	
	m_isRollerLocked = false;
	m_created = false;
}

/// \brief		Initialize MCDC with comPort param and set limits of MCDC
///	\pre		comPort is a valid port number and the roll device is connected to that port
///	\post		The com port is initialized and limits are set
void RollDevice::Init(QString comPort)
{
	MotionController::InitializeDevice(comPort);
	MotionController::SetCorridorValue(CORRIDOR);
	MotionController::ClearExistingData();
	//SetDefaultVelocity();
}

/// \brief		Re-enable roll device motor 
/// \pre		Roll device is still connected
///	\post		Limits are set again and the MCDC is re-enabled
void RollDevice::ReEnable(void)
{
	SetLimits();						//Set limits again just to be safe
	MotionController::EnableDevice();	//"EN" command
	MotionController::ResetEncoder();	//Reset encoder ticks to zero
}

///	\brief		Set Limits on Device
/// \pre		MCDC is still connected
/// \post		Current and Position Limits are Set
void RollDevice::SetLimits(void)
{
	SetPeakCurrentLimit(CURRENT_LIMIT);
		SetPositionLimits( ConvertAngleToPosition(MIN_ANGLE_LIMIT),
		ConvertAngleToPosition(MAX_ANGLE_LIMIT) );
	EnablePositionLimits(true);
}

/// \brief		Change max travelling velocity back to default value
void RollDevice::SetDefaultVelocity(void)
{
	MotionController::ChangeMaxMotorVelocity(MAX_VELOCITY);
}

/// \brief		Convert angle (in degrees) into encoder ticks for the motion controller
/// \pre		Angle is a valid angle for the roll device
/// \post		The postion (in encoder ticks) is returned 
int RollDevice::ConvertAngleToPosition(float angle)
{
	float pos = angle * (float)FAULHABER_ONE_ROTATION * (float)SPUR_GEAR_RATIO / 360.0;
	return (int)(pos+0.5);	//Return rounded-up int value of position
}

/// \brief		Convert position, in encoder ticks, to its associated angle (in degrees)
/// \pre		Position is in encoder ticks and is a valid number from the MCDC
/// \post		The angle (in degrees) is returned 
float RollDevice::ConvertPositionToAngle(long position)
{
	float angle = ( (float)position * 360.0 ) / ( (float)FAULHABER_ONE_ROTATION * (float)SPUR_GEAR_RATIO);
	return angle;
}

/// \brief		Get the current angle of the roll motor
///	\details	Get the encoder position and then covert it to an angle
/// \pre		MCDC is still connected
///	\post		Current angle of the roll motor is returned
float RollDevice::GetAngle()
{
	m_currentAngle = ConvertPositionToAngle( this->GetPosition() );
	return m_currentAngle;
}

/// \brief		Set the digital output of the MCDC
///	\details	Sets the FAULT pin to either high-impedance or GND
/// \pre		MCDC DIGOUT is set to true
///	\post		FAULT output pin is set based on OnOff
void RollDevice::setVibration(bool OnOff)
{
	MotionController::setDigitalOutput(OnOff);
}