/// class		InsertionDevice
/// brief		Encapsulates the functionality of the insertion stage. 
///				Inherits functionality from MotionController class.
///
///	author		Troy Adebar
///	date		March 25, 2013


#pragma once

#include "MotionController.h"

class InsertionDevice :	public MotionController
{
public: //Methods
	InsertionDevice(void);
	~InsertionDevice(void);
	void Init(QString comPort);
	void ReEnable(void);
	int ConvertMMToPosition(float angle);
	float ConvertPositionToMM(long position);
	float GetMM(void);	
	void SetDefaultVelocity(void);
	float GetPitch(void);
	
private:	//Methods
	void SetLimits(void);

private:	//Attributes
	static bool m_created;
	float m_min;
	float m_max;
	float m_incr;
	float m_currentMM;
	bool m_isInsertionLocked;
};
