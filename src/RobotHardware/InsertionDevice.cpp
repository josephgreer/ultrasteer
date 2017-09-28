#include "InsertionDevice.h"
#include "qstring.h"
#include "qdebug.h"
#include  <math.h>

/********************** Hardware Specific Parameters ******************************/
#define INSERTION_DEVICE_ID		1			//Network ID of the insertion MCDC	
#define CORRIDOR				20			//Default corridor value
#define FAULHABER_ONE_ROTATION	2000		//# of encoder ticks in one full rotation (without factoring in the gear ratio) 
#define SPUR_GEAR_RATIO			(5.9/1.0)	//Gear ratio for the maxon motor
#define SLIDE_PITCH				2.54		//MM travelled for one rotation of the linear slide
#define MIN_ENCODER_LIMIT		-30000		//Minimum encoder limit that the motor will not be able to travel past
#define MAX_ENCODER_LIMIT		30000		//Maximum encoder limit that the motor will not be able to travel past
#define CURRENT_LIMIT			3000		//Faulhaber motor peak current limit of 3000mA 	
#define MAX_VELOCITY			150		//Insertion device velocity when it is not moving in no increments mode
/**********************************************************************************/

bool InsertionDevice::m_created = false;

/// \brief		Constructor: Initializes roll device ID and sets current and position limits.	
InsertionDevice::InsertionDevice(void) : 
MotionController(INSERTION_DEVICE_ID),
	m_currentMM(NULL),
	m_isInsertionLocked(false)
{
	if(m_created)
	{
		qDebug() << "ERROR: Only one instance of InsertionDevice is allowed";
	}

	m_created = true;
}

/// \brief		Destructor: Stops motion if there happens to be any	
InsertionDevice::~InsertionDevice(void)
{
	MotionController::Stop();
	m_isInsertionLocked = false;
	m_created = false;
}

/// \brief		Initialize MCDC with comPort param and set limits of MCDC
///	\pre		comPort is a valid port number and the roll device is connected to that port
///	\post		The com port is initialized and limits are set
void InsertionDevice::Init(QString comPort)
{
	MotionController::InitializeDevice(comPort);
	MotionController::SetCorridorValue(CORRIDOR);
  MotionController::ChangeMaxMotorVelocity(MAX_VELOCITY);
}

/// \brief		Re-enable roll device motor 
/// \pre		Roll device is still connected
///	\post		Limits are set again and the MCDC is re-enabled
void InsertionDevice::ReEnable(void)
{
	SetLimits();						//Set limits again just to be safe
	MotionController::EnableDevice();	//"EN" command
	MotionController::ResetEncoder();	//Reset encoder ticks to zero
}

///	\brief		Set Limits on Device
/// \pre		MCDC is still connected
/// \post		Current and Position Limits are Set
void InsertionDevice::SetLimits(void)
{
	SetPeakCurrentLimit(CURRENT_LIMIT);
	//SetPositionLimits(MIN_ENCODER_LIMIT, MAX_ENCODER_LIMIT);
	//EnablePositionLimits(true);
}

/// \brief		Change max travelling velocity back to default value
void InsertionDevice::SetDefaultVelocity(void)
{
	MotionController::ChangeMaxMotorVelocity(MAX_VELOCITY);
}

/// \brief		Convert angle (in degrees) into encoder ticks for the motion controller
/// \pre		MM is a valid linear slide position in mm
/// \post		The postion (in encoder ticks) is returned 
int InsertionDevice::ConvertMMToPosition(float MM)
{
	float pos = MM * (float)FAULHABER_ONE_ROTATION / SLIDE_PITCH * SPUR_GEAR_RATIO;
	return (int)(pos+0.5);	//Return rounded-up int value of position
}

/// \brief		Convert position, in encoder ticks, to its associated angle (in degrees)
/// \pre		Position is in encoder ticks and is a valid number from the MCDC
/// \post		The linear slide position (in mm) is returned 
float InsertionDevice::ConvertPositionToMM(long position)
{
	float MM = ( (float)position / FAULHABER_ONE_ROTATION ) * SLIDE_PITCH / SPUR_GEAR_RATIO;
	return MM;
}

/// \brief		Get the current angle of the roll motor
///	\details	Get the encoder position and then covert it to an angle
/// \pre		MCDC is still connected
///	\post		Current angle of the roll motor is returned
float InsertionDevice::GetMM()
{
	long currentPos = this->GetPosition();
	m_currentMM = ConvertPositionToMM( currentPos );
	return m_currentMM;
}

/// \brief		Get the lead screw pitch
///	\details	Gives the number of mm travelled during one rotation of the insertion motor
/// \pre		None
///	\post		Leadscrew pitch is returned
float InsertionDevice::GetPitch()
{
	return SLIDE_PITCH/SPUR_GEAR_RATIO;
}

