#ifndef SERVOVALVE_H
#define SERVOVALVE_H

#include <Servo.h>
#include "Common.h"

class ServoValve 
{
protected:
  Servo m_servo;
  f64 m_closeAngle = 90;
  f64 m_openAngle = 70;

public:
  ServoValve(s32 pin);

  // 1 = closed 0 = open
  void SetPosition(f64 pos);
};

#endif
