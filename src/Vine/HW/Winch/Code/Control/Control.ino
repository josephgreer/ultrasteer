// Includes
#include <TimerOne.h>
#include <Encoder.h>
#include <SoftwareSerial.h>
#include "Common.h"
//#include "ServoValve.h"
#include <math.h>
#include <Servo.h>
#include "JacobianControl.h"
//#define SLA_SERIAL

// Pin Declares
int pwmAPin = 5;
int dirAPin = 8;
int encoderPinA = 13;
int encoderPinB = 3;

int servoValvePin = 45;
f64 openAngle = 70;
f64 closeAngle = 90;


//s32 regulatorPins[N_TURN_ACT] = { 10, 11, 9 };
s32 regulatorPins[N_TURN_ACT] = { 9, 10, 11 };
JacobianControl *jc = (JacobianControl *)new JacobianBoxConstraintControl();
KFJacobianEstimator je;
bool setJacobian = false;

s32 pressurePin = 3;

#ifdef SLA_SERIAL
int slaSerialPinRx = 2;
int slaSerialPinTx = 13;

SoftwareSerial slaSerial(slaSerialPinRx, slaSerialPinTx);
#endif

// Program Scope Variables
Encoder encoder(encoderPinA, encoderPinB);
f64 countPerRev = 1024;
int counter = 0;

//ServoValve extensionValve(servoValvePin);
Servo extensionServo;

// --------------------------
// Main Loop
// --------------------------
enum CONTROL_MODE {
  CM_POS = 0,
  CM_VEL = 1,
  CM_THROTTLE = 2
};

enum STEERING_CONTROL_MODE {
  SCM_NOTHING = 0,
  SCM_CALIBRATE,
  SCM_CALIBRATE_PAUSE,
  SCM_POS,
  SCM_CALIBRATE_ON_THE_FLY,
  SCM_CALIBRATE_KIN
};

u32 lastTime = 0;

CONTROL_MODE motorControlMode = CM_POS;
STEERING_CONTROL_MODE steeringControlMode = SCM_NOTHING;

u8 unwindDir = 1;
bool invalidPos = true;


#define TRACE_COUNT_TRACKS 50
#define TRACE_COUNT_MOTORS 50
#define BUFFER_LEN 50
s8 input[BUFFER_LEN] = { 0 };
s32 nBytes = 0;

f64 currPressures[N_TURN_ACT] = { 0 };

f64 pressureSetPoints[N_TURN_ACT] = { 0 };
f64 pressurePs[N_TURN_ACT] = { 0.3, 0.3, 0.3 };
f64 pressureDs[N_TURN_ACT] = { 0.01,0.01,0.01 };//p 0.005, 0.005, 0.005 };
f64 pressureIs[N_TURN_ACT] = { 1e-4,1e-4,1e-4 };// 1e-3, 1e-3, 1e-3 };
f64 pressureActuatorLast[N_TURN_ACT] = { 0 };
f64 pressureLowPassError[N_TURN_ACT] = { 0 };
f64 pressureErrorLast[N_TURN_ACT] = { 0 };
f64 pressureIntegralError[N_TURN_ACT] = { 0 };
s32 pressureSensorPins[N_TURN_ACT] = { A1,A2,A5 };
u32 lastTimePressureVisited = 0;
f64 aveTimePerPressureLoop = 0;
u32 count = 0;

//Actuator effort
//Actuator effect = a+(b-a)/(1+10^(d*(c-x))
struct Sigmoid
{
  f64 a, b, c, d;
  Sigmoid(f64 a, f64 b, f64 c, f64 d)
    : a(a)
    , b(b)
    , c(c)
    , d(d)
  {
  }
  Sigmoid()
    : a(0)
    , b(0)
    , c(0)
    , d(0)
  {
  }
};

struct ActuatorCalibration
{
  Vecf64<2> offset;
  ActuatorCalibration()
    : offset(0,0)
  {
  }
  ActuatorCalibration(Vecf64<2> o)
    : offset(o)
  {
  }
};

ActuatorCalibration actuatorCalibs[N_TURN_ACT];
Matrixf64<2,N_TURN_ACT> JJ;

#define TRACK_WARMUP_COUNT_MAX 10

// Position Constants
f64 steeringKpp = 0.001;
f64 steeringKdp = 0.0008;
f64 steeringKip = 0;

Vecf64<2> errorLastTrack(0, 0);
Vecf64<2> lowPassError(0, 0);
Vecf64<2> integralErrorTrack(0, 0);
s32 trackWarmupCount = 0;

f64 totalCalibTime = 2;
f64 totalRestTime = 4;
f64 incrementalCalibTime = 0.3;
f64 incrementalCalibRestTime = 15;
f64 calibTime = 0;
f64 aveTimePerLoop = 0;
f64 timeBetweenOnTheFlyCalibrations = 10;
s8 currCalibAct = -1;
Vecf64<2> calibStartTrackPos(0, 0);
bool wroteCalibration = false;

f64 totalCalibTimeOnTheFly = 0.5;
f64 totalRestTimeOnTheFly = 0.5;

s32 outputCount = 0;

#define CALIB_AMNT 0.2

f64 calibAmntsOnTheFly[N_TURN_ACT] = { 0.2, 0.11, 0.2 };

Vecf64<2> trackPos;
Vecf64<2> trackPosLast;
Vecf64<2> scenePos;
Vecf64<2> desTrackPos(320, 240);
f64 trackConf = 0;

f64 savedPressureSetPoints[N_TURN_ACT] = { 0 };

u32 lastTimeSteeringVisited = 0;

u8 calibPWM = 0;
#define CALIB_BIN_DS 1
#define CALIB_BIN_DOWNSAMPLE 2.0

ActuatorMapper *mapper[N_TURN_ACT];

#define HOME_PRESSURE 0.5

#define ACTUATE_REGULATOR(r,amnt) (analogWrite(regulatorPins[r], mapper[r]->MapVal(amnt)))
#define REGULATE_PRESSURE(amnt) (analogWrite(pressurePin, (s32)(MIN((f64)255.0*amnt,128))))

s32 g_changeCount = 0;

void resetTracking()
{
  calibPWM = 0;
  calibStartTrackPos = Vecf64<2>(0, 0);
  calibTime = 0;
  currCalibAct = -1;
  errorLastTrack = Vecf64<2>(0, 0);
  lowPassError = errorLastTrack;
  integralErrorTrack = Vecf64<2>(0, 0);
  trackWarmupCount = 0;
}

bool hasOutput = false;
s32 doOutput = 0;
void controlSteering()
{
  f64 u = 0;
  f64 vel;
  invalidPos = true;

  //TIME_LOOP(controlSteering, 100);

  u32 currTime = micros();
  f64 dt = (f64)(currTime - lastTimeSteeringVisited) / (f64)1e6;
  aveTimePerLoop = dt*0.2 + 0.8*aveTimePerLoop;

  if (lastTimeSteeringVisited == 0)
    dt = 0;
  lastTimeSteeringVisited = currTime;

  switch (steeringControlMode)
  {
  case SCM_NOTHING:
  {
    break;
  }
  case SCM_CALIBRATE:
  {
    bool transition = false;
    bool doneResting = false;

    if (currCalibAct == -1) {
      transition = true;
    }

    calibTime += dt;
    if (calibTime >= totalCalibTime)
      transition = true;

    if (calibTime >= totalCalibTime + totalRestTime) {
      Serial.println("Done resting actuator " + String((s32)currCalibAct));
      doneResting = true;
    }

    if (transition) {
      Vecf64<2> delta = trackPos - calibStartTrackPos;

      if (currCalibAct >= 0 && !wroteCalibration) {
        actuatorCalibs[currCalibAct].offset = delta*(1.0 / CALIB_AMNT)*(aveTimePerLoop / totalCalibTime);
        for (s32 jj = 0; jj < N_TURN_ACT; jj++)
          pressureSetPoints[jj] = 0;
      }
      wroteCalibration = true;
    }
    if (doneResting) {
      wroteCalibration = false;
      calibTime = 0;
      currCalibAct++;
      if (currCalibAct >= N_TURN_ACT) {
        steeringControlMode = SCM_NOTHING;
        currCalibAct = -1;

        Serial.println("Done calibrating");
        for (s32 ii = 0; ii < N_TURN_ACT; ii++)
          Serial.println("Actuator " + String(ii) + " offset " + String(actuatorCalibs[ii].offset.x) + ", " + String(actuatorCalibs[ii].offset.y));

        Vecf64<2> offsets[N_TURN_ACT] = { 0 };
        for (s32 ii = 0; ii < N_TURN_ACT; ii++)
          offsets[ii] = actuatorCalibs[ii].offset;
        JJ = Matrixf64<2, N_TURN_ACT>::FromCols(offsets);
        JJ.Print("JJ");

        je.Initialize(JJ);
      }
      calibStartTrackPos = trackPos;
      if (currCalibAct >= 0)
        pressureSetPoints[currCalibAct] = CALIB_AMNT;
    }

    break;
  }
  case SCM_CALIBRATE_PAUSE:
  {
    calibStartTrackPos = trackPos;
    break;
  }
  case SCM_POS:
  {
    calibTime += dt;

    if (calibTime >= timeBetweenOnTheFlyCalibrations) {
      steeringControlMode = SCM_CALIBRATE_ON_THE_FLY;
      calibTime = 0;
      currCalibAct = -1;
    }

    //je.PrintState();
    if (trackPos.x == -1.0 && trackPos.y == -1.0) {
      resetTracking();
      break;
    }
    Vecf64<2> error = desTrackPos - trackPos;
    lowPassError = error*1 + lowPassError*0.0;
    Vecf64<2> derror = (lowPassError - errorLastTrack)*(1.0 / dt);
    errorLastTrack = lowPassError;

    Vecf64<2> z = trackPos - trackPosLast;
    bool trackPosChanged = (z.magnitude() > 1 && scenePos.magnitude() < z.magnitude() / 20.0);
    if (trackPosChanged) {
      doOutput++;
      trackWarmupCount = 0;
    }

    f64 steeringKdpAlpha = 1;// trackWarmupCount > 0 ? 1 : 0;// trackWarmupCount / (f64)(TRACK_WARMUP_COUNT_MAX);
    trackWarmupCount = CLAMP(trackWarmupCount + 1, 0, TRACK_WARMUP_COUNT_MAX);
    Vecf64<2> proportional = error*steeringKpp;
    Vecf64<2> deriv = derror*steeringKdp*steeringKdpAlpha;
    Vecf64<2> integral = integralErrorTrack*steeringKip;

    if (trackPosChanged)
      trackWarmupCount = 0;

    Vecf64<2> u = proportional + deriv + integral;
    Vecf64<N_TURN_ACT> qs, dqs;
    jc->Update(qs, dqs, u, JJ);
    if (false) {
      Serial.println("error=" + (String)(error)+"\n proprotional=" + String(proportional) + "\n deriv=" + String(deriv) + "\n integral=" +
        String(integral) + "\n u=" + String(u) + "\n dqs= " + String(dqs) + "\n qs= " + String(qs));
    }
    for (s32 ii = 0; ii < N_TURN_ACT; ii++) {
      pressureSetPoints[ii] = qs[ii];
      savedPressureSetPoints[ii] = qs[ii];
    }

    if (trackPosLast.x > 0 && trackPosLast.y > 0) {
      // Don't update if user manually specified track change
      if (!trackPosChanged) {
        if (setJacobian)
          JJ = je.Update(z);
        else
          Matrixf64 < 2, N_TURN_ACT > JJBean = je.Update(z);
      }
    }
    je.SetDq(dqs);
    trackPosLast = trackPos;
    //anti windup
    if (0 < qs[0] && qs[0] < 1 && 0 < qs[1] && qs[1] < 1 && 0 < qs[2] && qs[2] < 1) {
      integralErrorTrack.x += error.x;
      integralErrorTrack.y += error.y;
    }
    break;
  }
  case SCM_CALIBRATE_ON_THE_FLY:
  {
    bool transition = false;
    bool doneResting = false;

    if (currCalibAct == -1) {
      transition = true;
    }

    calibTime += dt;
    if (calibTime >= totalCalibTimeOnTheFly)
      transition = true;

    if (calibTime >= totalCalibTimeOnTheFly + totalRestTimeOnTheFly) {
      Serial.println("Done resting actuator " + String((s32)currCalibAct));
      doneResting = true;
    }

    if (transition) {
      Vecf64<2> delta = trackPos - calibStartTrackPos;

      if (currCalibAct >= 0 && !wroteCalibration) {
        actuatorCalibs[currCalibAct].offset = delta*(1.0 / calibAmntsOnTheFly[currCalibAct])*(aveTimePerLoop / totalCalibTimeOnTheFly);
        for (s32 jj = 0; jj < N_TURN_ACT; jj++)
          pressureSetPoints[jj] = savedPressureSetPoints[jj];
      }
      wroteCalibration = true;
    }
    if (doneResting) {
      wroteCalibration = false;
      calibTime = 0;
      currCalibAct++;
      if (currCalibAct >= N_TURN_ACT) {
        steeringControlMode = SCM_POS;
        currCalibAct = -1;

        Vecf64<2> offsets[N_TURN_ACT] = { 0 };
        for (s32 ii = 0; ii < N_TURN_ACT; ii++)
          offsets[ii] = actuatorCalibs[ii].offset;
        JJ = Matrixf64<2, N_TURN_ACT>::FromCols(offsets);
        JJ.Print("JJ");

        je.Initialize(JJ);
      }
      calibStartTrackPos = trackPos;
      if (currCalibAct >= 0)
        pressureSetPoints[currCalibAct] = calibAmntsOnTheFly[currCalibAct]+savedPressureSetPoints[currCalibAct];
    }

    break;
  }
  case SCM_CALIBRATE_KIN:
  {
    bool transition = false;
    bool doneResting = false;

    calibTime += dt;
    if (calibTime >= totalCalibTime)
      transition = true;

    if (calibTime >= totalCalibTime + totalRestTime) {
      doneResting = true;
    }

    if (transition  && !wroteCalibration) {
      Vecf64<3> pin;
      for (s32 ii = 0; ii < N_TURN_ACT; ii++)
        pin(ii) = analogRead(pressureSensorPins[ii]) / 1024.0;
      Serial.println("Position = " + String((s32)currCalibAct) + " [p0 p1 p2] = " + pin.ToString());
      for (s32 ii = 0; ii < N_TURN_ACT; ii++)
        pressureSetPoints[ii] = 0;
      REGULATE_PRESSURE(0.5);
      wroteCalibration = true;
    }
    if (doneResting) {
      wroteCalibration = false;
      currCalibAct++;
      calibTime = 0;
      if (currCalibAct >= 50) {
        steeringControlMode = SCM_NOTHING;
        currCalibAct = -1;
      }
      else {
        Vecf64<3> pp;
        if (currCalibAct == 0) {
          pp(0) = 0.3;
        }
        else if (currCalibAct == 1) {
          pp(1) = 0.3;
        }
        else if (currCalibAct == 2) {
          pp(2) = 0.3;
        }
        else {
          s32 minIdx = -1;
          f64 minVal = 1e6;
          for (s32 ii = 0; ii < N_TURN_ACT; ii++) {
            pp(ii) = 0.15 * ((f64)rand() / (f64)RAND_MAX) + 0.1;
            if (pp(ii) < minVal) {
              minVal = pp(ii);
              minIdx = ii;
            }
          }
          pp(minIdx) = 0;
        }

        pressureSetPoints[0] = pp(0);
        pressureSetPoints[1] = pp(1);
        pressureSetPoints[2] = pp(2);
      }

    }
    break;
  }
  }
}

f64 errorLastPosMotor = 0;
u8 dirLastMotor = 0;
f64 desPosMotor = 0;
f64 desVelMotor = 0;
f64 integralErrorMotor = 0;
f64 lastPosMotor = 0;
f64 lastVelMotor = 0;

// Position Constants
f64 motorKpp = 0.08;
f64 motorKdp = 0;
f64 motorKip = 0.00005;

// Velocity Constants
f64 motorKpv = 1;
f64 motorKdv = 0;
f64 motorKiv = 0.01;

bool printOutMotors = false;
bool printOutTracks = false;

void controlMotors(f64 dt)
{
  f64 u;
  f64 vel;

  f64 pos = encoder.read();
  f64 error, derror;

  if (motorControlMode == CM_POS) {
    error = desPosMotor - pos;
  }
  else {
    vel = 0.2 * (pos - lastPosMotor) / dt + 0.8 * lastVelMotor;
    error = desVelMotor - vel;
    lastVelMotor = vel;
  }
  derror = error - errorLastPosMotor;
  integralErrorMotor += error;

  errorLastPosMotor = error;
  derror /= dt;
  lastPosMotor = pos;

  s8 dir = 0;

  switch (motorControlMode) {
  case CM_POS:
  {
    u = motorKpp * error + motorKdp * derror + motorKip * integralErrorMotor;

    if (u < 0) {
      dir = 1;
      u = abs(u);
    }
    u = min(u, 255);
    break;
  }
  case CM_VEL:
  {
    u = motorKpv * error + motorKdv * derror + motorKiv * integralErrorMotor;

    if (u < 0) {
      dir = 1;
      u = abs(u);
    }
    u = min(u, 255);
    break;
  }
  case CM_THROTTLE:
  {
    u = motorKpv * error + motorKdv * derror + motorKiv * integralErrorMotor;

    if (u < 0) {
      dir = 1;
      u = abs(u);
    }
    if (dir == unwindDir) {
      u = 0;
      integralErrorMotor = 0;
      derror = 0;
      dir = 1 - unwindDir;
    }
    u = min(u, 255);
    break;
  }
  }

  if (dir != dirLastMotor) {
    digitalWrite(dirAPin, dir);
    dirLastMotor = dir;
  }

  if (printOutMotors)
  {
    if (count++ % TRACE_COUNT_MOTORS == 0 && motorControlMode == CM_POS)
      Serial.println("Des Pos " + String(desPosMotor) + " Pos " + String(pos) + " Error " + String(error) + " u " + String(u) + " dt " + String(dt * 1000.0) + " derror " + String(derror) + " integralErrorMotor " + String(integralErrorMotor));
    else if (count % TRACE_COUNT_MOTORS == 0 && motorControlMode == CM_VEL)
      Serial.println("DesVel " + String(desVelMotor) + " Vel " + String(vel) + " Dir " + String((s32)dir) + " Error " + String(error) + " u " + String(u) + " dt " + String(dt * 1000.0) + " derror " + String(derror) + " integralErrorMotor " + String(integralErrorMotor));
    else if (count % TRACE_COUNT_MOTORS == 0 && motorControlMode == CM_THROTTLE)
      Serial.println("DesVel " + String(desVelMotor) + " Vel " + String(vel) + " Dir " + String((s32)dir) + " Error " + String(error) + " u " + String(u) + " dt " + String(dt * 1000.0) + " derror " + String(derror) + " integralErrorMotor " + String(integralErrorMotor));
    analogWrite(pwmAPin, u);
  }
}

#if 0
const u8 crc8_Table[] =
{
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

void sendSlaTrackingPacket(u8 *buffer, s32 trackRow, s32 trackCol)
{
  buffer[0] = 0x51;
  buffer[1] = 0xAC;
  buffer[2] = 0x07;

  buffer[3] = 0x05;
  buffer[4] = (u8)(trackCol & 0xFF);
  buffer[5] = (u8)((trackCol & 0xFF00) >> 8);
  buffer[6] = (u8)(trackRow & 0xFF);
  buffer[7] = (u8)((trackRow & 0xFF00) >> 8);
  buffer[8] = (0x40) | (0x01);

  u8 crc = 1;
  for (s32 i = 3; i <= 8; i++)
    crc = crc8_Table[crc ^ (buffer[i])];

  buffer[9] = crc;

  Serial1.write(buffer, 10);
}
#endif

//#define DO_TEST

#ifdef DO_TEST
void runTest();
#endif

f64 g_totTime = 0;

void loop()
{
  TIME_LOOP(MainLoop, 5000000);
  u32 currTime = micros();
  f64 dt = ((f64)currTime - (f64)lastTime) / ((f64)1e6);
  lastTime = currTime;
  g_totTime += dt;

#ifdef DO_TEST
  delay(100);
  Serial.println("About to run test");
  runTest();
  delay(10000);
#endif

  ////MOTOR CONTROL
  //controlMotors(dt);


  //PRESSURE CONTROL
  controlPressures();

  if (!invalidPos) {
    //STEERING CONTROL
    controlSteering();
  }

  count++;
  if (g_totTime>100e-3) {
    for (s32 i = 0; i < N_TURN_ACT; i++)
      currPressures[i] = analogRead(pressureSensorPins[i]) / 1024.0;
    Serial.println("P " + String(pressureLowPassError[0], 5) + ", " + String(pressureLowPassError[1], 5) + ", " + String(pressureLowPassError[2], 5) + ", " + String(1 / (g_totTime / count), 6) + ", " + String(trackPos.x) + ", " + String(trackPos.y) + ", " + String(trackConf) + ";");
    g_totTime = 0;
    count = 0;
  }

  //SERIAL HANDLER
#if 1
  if (Serial.available()) {
    nBytes = Serial.readBytes(input, BUFFER_LEN);
    handleSerial(input, nBytes);
    memset(input, 0, nBytes);
  }
#endif

#ifdef SLA_SERIAL
  if (slaSerial.available()) {
    nBytes = slaSerial.readBytes(input, BUFFER_LEN);
    handleSlaSerial((u8 *)input, nBytes);
    memset(input, 0, nBytes);
  }
#else
  if (Serial1.available()) {
    nBytes = Serial1.readBytes(input, BUFFER_LEN);
    handleSlaSerial((u8 *)input, nBytes);
    memset(input, 0, nBytes);
  }
#endif
}

static s32 g_pressureCount = 0;

void controlPressure(s32 i, f64 dt)
{
  f64 kTerm, dTerm, iTerm;
  f64 error = 0;
  f64 currPressure = 0;
  f64 derror = 0;
  f64 u = 0;
  f64 actuatorAmount = 0;

  currPressure = analogRead(pressureSensorPins[i])/1024.0;
  error = pressureSetPoints[i] - currPressure;
  pressureLowPassError[i] = currPressure*1e-2 + pressureLowPassError[i] * (1 - 1e-2);
  derror = (pressureLowPassError[i] - pressureErrorLast[i]) / dt;

  kTerm = error*pressurePs[i];
  iTerm = pressureIntegralError[i] * pressureIs[i];
  dTerm = -derror*pressureDs[i];

  u = kTerm + iTerm + dTerm;
  if (0 < u+pressureActuatorLast[i] && u+pressureActuatorLast[i] < 1)
    pressureIntegralError[i] += error;
  actuatorAmount = CLAMP((u + pressureActuatorLast[i]), 0, 1);

#if 0
  if (i == 1)
    g_pressureCount++;
  if (g_pressureCount == 1 && i == 1) {
    Serial.println("u = " + String(u,6) + " error " + String(error,6) + " actuatorAmount " + String(actuatorAmount,6) + " currPressure " + String(currPressure,6));
    g_pressureCount = 0;
    //Serial.println("derror = " + String(derror) + " pressure = " + String(currPressure) + " regulatorAmount " + String(actuatorAmount, 6) + " error = " + String(error, 6) + " kTerm " + String(kTerm, 6) + " iTerm " + String(iTerm, 6) + " dTerm " + String(dTerm, 6));
  }
#endif

  pressureActuatorLast[i] = actuatorAmount;
  pressureErrorLast[i] = pressureLowPassError[i];

  ACTUATE_REGULATOR(i, CLAMP(actuatorAmount, 0, 1));
}

void controlPressures()
{
  u32 currTime = micros();
  f64 dt = (f64)(currTime - lastTimePressureVisited) / (f64)1e6;
  aveTimePerPressureLoop = dt*0.2 + 0.8*aveTimePerPressureLoop;

  if (lastTimePressureVisited == 0)
    dt = 0;
  lastTimePressureVisited = currTime;
  if (dt == 0)
    return;

  for (s32 i = 0; i < N_TURN_ACT; i++) {
    controlPressure(i, dt);
  }
}

void handleSerial(const s8 *input, s32 nBytes)
{
  switch (input[0]) {
  case 'm':   // switch mode
  {
    String mode;
    if (input[2] == 'v') {              //velocity
      motorControlMode = CM_VEL;
      mode = "velocity";
    }
    else if (input[2] == 'p') {       //position
      motorControlMode = CM_POS;
      mode = "position";
    }
    else if (input[2] == 't') {       //throttle
      motorControlMode = CM_THROTTLE;
      mode = "throttle";
    }
    desVelMotor = integralErrorMotor = 0;
    errorLastPosMotor = 0;
    desPosMotor = lastPosMotor;
    integralErrorMotor = 0;
    lastVelMotor = 0;

    Serial.println("Control mode switched to " + mode);
    break;
  }
  case 'c':
  {
    String mode;
    if (input[2] == 'c') {
      steeringControlMode = SCM_CALIBRATE;
      mode = "calibrate";
    }
    else if (input[2] == 'r') {
      steeringControlMode = SCM_CALIBRATE_PAUSE;
      mode = "calibration paused";
    }
    else if (input[2] == 'p') {
      steeringControlMode = SCM_POS;
      mode = "position";
    }
    else if (input[2] == 'g') {
      steeringControlMode = SCM_CALIBRATE_KIN;
      mode = "kinematic";
    }
    else if (input[2] == 'j') {
      setJacobian = !setJacobian;
      if (setJacobian)
        mode = "jacobianOn";
      else
        mode = "jacobianOff";
    }
    else if (input[2] == 'n') {
      steeringControlMode = SCM_NOTHING;
      mode = "none";
      
      resetTracking();
    }
    else if (input[2] == 'h') {
      steeringControlMode = SCM_NOTHING;
      mode = "none";
      for (s32 jj = 0; jj < N_TURN_ACT; jj++)
        pressureSetPoints[jj] = 0;

      resetTracking();
    }
    else if (input[2] == 'k') {
      f64 *Kp, *Kd, *Ki;
      Kp = &steeringKpp;
      Kd = &steeringKdp;
      Ki = &steeringKip;
      f64 val = atof(input + 5);
      if (input[3] == 'p') {
        *Kp = val;
        Serial.println("Set Kp to " + String(val, 10));
      }
      else if (input[3] == 'd') {
        *Kd = val;
        Serial.println("Set Kd to " + String(val, 10));
      }
      else if (input[3] == 'i') {
        *Ki = val;
        Serial.println("Set Ki to " + String(val));
      }
    }
    else if (input[2] == 'l') {
      if (input[3] == 'p') {
        steeringKpp = steeringKpp * 2.0;
        steeringKdp = steeringKdp * 2.0;
      }
      else if(input[3] == 'd') {
        steeringKpp = steeringKpp / 2.0;
        steeringKdp = steeringKdp / 2.0;
      }
      Serial.println("Set kp to " + String(steeringKpp, 10) + " kd to " + String(steeringKdp, 10));
    }
    if (input[2] == 'c' || input[2] == 'p' || input[2] == 'n' || input[2] == 'j' || input[2] == 'g' || input[2] == 'h')
      Serial.println("Setting steering control mode " + mode);
    break;
  }
  case 'q':  // decrement set point. interpretation depends on mode
  {
    f64 decrement = atof(input + 2);
    if (motorControlMode == CM_POS)
      desPosMotor -= decrement;
    else
      desVelMotor -= decrement;

    Serial.println("Decrementing " + String(decrement));
    break;
  }
  case 'w':   // increment set point. interpretation depends on mode
  {
    f64 increment = atof(input + 2);
    if (motorControlMode == CM_POS)
      desPosMotor += increment;
    else
      desVelMotor += increment;

    Serial.println("Incrementing " + String(increment));
    break;
  }
  case 'K':   //set Kp/Kd/Ki
  case 'k':
  {
    f64 *Kp, *Kd, *Ki;
    if (motorControlMode == CM_VEL) {
      Kp = &motorKpv;
      Kd = &motorKdv;
      Ki = &motorKiv;
    }
    else {
      Kp = &motorKpp;
      Kd = &motorKdp;
      Ki = &motorKip;
    }
    f64 val = atof(input + 3);
    if (input[1] == 'p') {
      *Kp = val;
      Serial.println("Set Kp to " + String(val));
    }
    else if (input[1] == 'd') {
      *Kd = val;
      Serial.println("Set Kd to " + String(val));
    }
    else if (input[1] == 'i') {
      *Ki = val;
      Serial.println("Set Ki to " + String(val));
    }
    break;
  }
  case 'p':   //Pause
  case 'P':

    desVelMotor = integralErrorMotor = 0;
    errorLastPosMotor = 0;
    desPosMotor = lastPosMotor;
    integralErrorMotor = 0;
    lastVelMotor = 0;

    Serial.println("Pause");
    break;
  case 'd':
  case 'D':
  {
    ACTUATE_REGULATOR(1, 1);
    ACTUATE_REGULATOR(2, 1);
    //Serial.println("pwm amount " + String(MAP_VAL(amount, actuatorCalibs[reg - 1].sig.a, actuatorCalibs[reg - 1].sig.b, actuatorCalibs[reg - 1].sig.c, actuatorCalibs[reg - 1].sig.d)));
    break;
  }
  case 'e':
  case 'E': //Pressure REgulator
  {
    f64 amnt = atof(input + 2);
    u8 amount = (s32)(amnt*255.0 + 0.5);
    Serial.println("Setting pressure regulator to " + String(amnt) + " " + String(amount));
    REGULATE_PRESSURE(amnt);
    break;
  }
  case 'l':
  case 'L': //Toggle Pin
  {
    s32 reg = atoi(input + 2);
    if (reg < 1 || reg > N_TURN_ACT)
      return;

    if (input[4] == 'k')
    {
      f64 val = atof(input + 7);
      if (input[5] == 'p') {
        pressurePs[reg - 1] = val;
        Serial.println("Set reg " + String(reg) + " Kp to " + String(val));
      }
      else if (input[5] == 'd') {
        pressureDs[reg - 1] = val;
        Serial.println("Set reg " + String(reg) + " Kd to " + String(val));
      }
      else if (input[5] == 'i') {
        pressureIs[reg - 1] = val;
        Serial.println("Set reg " + String(reg) + " Ki to " + String(val));
      }
    } 
    else if (input[4] == 'r')
    {
      Serial.println("Pressure in actuator " + String(reg) + " = " + String(analogRead(pressureSensorPins[reg - 1]) / 1024.0));
    }
    else 
    {
      f64 amount = atof(input + 4);
      Serial.println("Setting pressure " + String(reg) + " to " + String(amount) + " input string " + String(input));
      //ACTUATE_REGULATOR(reg - 1, amount);
      pressureSetPoints[reg - 1] = amount;
      //Serial.println("pwm amount " + String(MAP_VAL(amount, actuatorCalibs[reg - 1].sig.a, actuatorCalibs[reg - 1].sig.b, actuatorCalibs[reg - 1].sig.c, actuatorCalibs[reg - 1].sig.d)));
    }
    break;
  }
  case 'v':
  case 'V':
  {
    f64 pos = atof(input + 2);
    f64 angle = (closeAngle - openAngle)*pos + openAngle;
    Serial.println("Setting servo valve position " + String(angle));
    extensionServo.write(angle);
    break;
  }
  case 's':
  case 'S':
  {
    f64 step = 10;
    if (input[1] == 's')
      desTrackPos.y += step;
    else if (input[1] == 'a')
      desTrackPos.x += step;
    else if (input[1] == 'w')
      desTrackPos.y -= step;
    else if (input[1] == 'd')
      desTrackPos.x -= step;

    trackWarmupCount = 0;
    break;
  }
  default:
    break;
  }
}

String output;
u32 serialCount = 0;
u32 lastTimeTracks = 0;
void handleSlaSerial(const u8 *input, s32 nBytes)
{
#if 0
  char printStr[100] = { 0 };
  s8 *strPtr = printStr;
  for (s32 i = 0; i < nBytes; i++) {
    strPtr += sprintf(strPtr, "%02X ", input[i]);
  }
  Serial.println(String(printStr) + " Length " + String(nBytes));
#endif

  //not the correct header;
  if (input[0] != 0x51 || input[1] != 0xAC || input[3] != 0x43)
    return;

  s16 trackRow = input[6] + (input[7] << 8);
  s16 trackCol = input[4] + (input[5] << 8);

  s32 sceneRow8 = (input[10]<<8) + (input[11] << 16) + (input[23]);
  s32 sceneCol8 = (input[8]<<8) + (input[9] << 16) + (input[22]);

  trackPos = Vecf64<2>((f64)trackCol, (f64)trackRow);
  scenePos = Vecf64<2>((f64)sceneCol8/256.0, (f64)sceneRow8/256.0);
  trackConf = input[16];
  invalidPos = false;

  if ((serialCount++ % TRACE_COUNT_TRACKS) == 0 && printOutTracks) {
    u32 currTime = millis();
    Serial.println("scene row scene col " + String(scenePos.y) + " " + String(scenePos.x) + " time " + String((currTime - lastTimeTracks) / (f64)TRACE_COUNT_TRACKS));
    lastTimeTracks = currTime;
    serialCount = 1;
  }
}

// --------------------------
// Initialize
// --------------------------
void setup()
{
#ifdef SLA_SERIAL
  pinMode(slaSerialPinRx, INPUT);
#endif

  extensionServo.attach(servoValvePin);
  for (s32 i = 0; i < 5; i++) {
    extensionServo.write(openAngle);
    delay(200);
    extensionServo.write(closeAngle);
    delay(200);
  }

  Serial.setTimeout(5);

  // Set Up Serial
  Serial.begin(57600);
#ifdef SLA_SERIAL
  slaSerial.begin(57600);
  slaSerial.setTimeout(1);
#else
  Serial1.begin(57600);
  Serial1.setTimeout(5);
#endif

  // Output Pins
  pinMode(pwmAPin, OUTPUT);  // PWM for A
  pinMode(dirAPin, OUTPUT);  // dir for A

  resetTracking();
                             //regulator Pins
  for (s32 ii = 0; ii < N_TURN_ACT; ii++) {
    pinMode(regulatorPins[ii], OUTPUT);
    digitalWrite(regulatorPins[ii], LOW);
  }

  // Pressure Pin
  pinMode(pressurePin, OUTPUT);
  digitalWrite(pressurePin, LOW);

  for (s32 i = 0; i < N_TURN_ACT; i++)
    pinMode(pressureSensorPins[i], INPUT);

  // Encoder Pins
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);

  // Init Motor
  analogWrite(pwmAPin, 0);     // set to not be spinning (0/255)
  digitalWrite(dirAPin, LOW);  // set direction

  actuatorCalibs[0] = ActuatorCalibration(Vecf64<2>(0, 0));// , Sigmoid(0.003146, 0.984901, 127.109794, 0.033328));
  actuatorCalibs[1] = ActuatorCalibration(Vecf64<2>(0, 0));// , Sigmoid(0.049058, 0.976153, 182.573890, 0.028746));
  actuatorCalibs[2] = ActuatorCalibration(Vecf64<2>(0, 0));// , Sigmoid(0.011475, 0.971715, 144.373752, 0.042064));

  mapper[0] = (ActuatorMapper *)new DeadbandMapper(103.0 / 255.0, 255);
  mapper[1] = (ActuatorMapper *)new DeadbandMapper(103.0 / 255.0, 255);
  mapper[2] = (ActuatorMapper *)new DeadbandMapper(103.0 / 255.0, 255);

  Vecf64<3> qs; qs.m_data[0] = qs.m_data[1] = qs.m_data[2] = 0;

  trackPosLast = Vecf64<2>(-1, -1);

#if 0
  for (s32 ii = 0; ii < N_TURN_ACT; ii++) {
    String printStr = "Actuator ii " + String(ii) + ": ";
    for (s32 jj = 0; jj < 256; jj++) {
      printStr += String(MAP_VAL(jj / 256.0, actuatorCalibs[ii].sig.a, actuatorCalibs[ii].sig.b, actuatorCalibs[ii].sig.c, actuatorCalibs[ii].sig.d)) + ", ";
    }
    Serial.println(printStr);
    delay(1000);
  }
#endif


  // Init Timer and Set Haptic Loop
  //Timer1.initialize();
  //long period = 10000; // 10000 [us] -> 100 Hz sample time
  //Timer1.attachInterrupt(hapticLoop,period);
}