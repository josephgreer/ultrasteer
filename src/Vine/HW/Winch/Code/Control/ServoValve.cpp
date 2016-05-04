#include "ServoValve.h"
#include "Arduino.h"

ServoValve::ServoValve(s32 pin) 
{
  m_servo.attach(pin);
  m_servo.write(m_closeAngle);
}

// 1 = closed 0 = open
void ServoValve::SetPosition(f64 pos)
{
  f64 angle = (m_closeAngle-m_openAngle)*pos+m_openAngle;
  Serial.println("setting servo valve to angle " + String(pos));
  m_servo.write(pos);
}
