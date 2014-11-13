/// class:		ArticulationDevice
/// brief:		Encapsulates the functionality of the needle tip articulation device. 
///				Inherits functionality from MotionController class.
///
///	author:		Troy Adebar
///	date:		November 13, 2014

#pragma once

#include "MotionController.h"

class ArticulationDevice :	public MotionController
{
public: //Methods
	ArticulationDevice(void);
	~ArticulationDevice(void);
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
	bool m_isArticulationLocked;
};
