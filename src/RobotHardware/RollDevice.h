/// class:		RollDevice
/// brief:		Encapsulates the functionality of the roll device. 
///				Inherits functionality from MotionController class.
///
///	author:		Troy Adebar
///	date:		March 25, 2013


#pragma once

#include "MotionController.h"

class RollDevice :	public MotionController
{
public: //Methods
	RollDevice(void);
	~RollDevice(void);
	void Init(QString comPort);
	void ReEnable(void);
	int ConvertAngleToPosition(float angle);
	float ConvertPositionToAngle(long position);
	float GetAngle(void);	
	void SetDefaultVelocity(void);
	void setVibration(bool);

	
private:	//Methods
	void SetLimits(void);

private:	//Attributes
	static bool m_created;
	float m_min;
	float m_max;
	float m_incr;
	float m_currentAngle;
	bool m_isRollerLocked;
};
