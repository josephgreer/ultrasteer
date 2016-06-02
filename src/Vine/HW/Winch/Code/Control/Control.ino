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


s32 regulatorPins[N_TURN_ACT] = { 11, 9, 10 };
JacobianControl *jc = (JacobianControl *)new JacobianHeuristicControl();

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
  SCM_CALIBRATE_INCREMENTAL,
  SCM_CALIBRATE_PAUSE,
  SCM_POS
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

#define DO_TEST

#ifdef DO_TEST
void runTest();
#endif

void loop()
{
  TIME_LOOP(MainLoop, 5000000);
  u32 currTime = micros();
  f64 dt = ((f64)currTime - (f64)lastTime) / ((f64)1e6);
  lastTime = currTime;

#ifdef DO_TEST
  runTest();
  delay(10000);
#endif

  //MOTOR CONTROL
  controlMotors(dt);

  if (!invalidPos) {
    //STEERING CONTROL
    controlSteering();
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
  Sigmoid sig;
  ActuatorCalibration()
    : offset(0,0)
    , sig()
  {
  }
  ActuatorCalibration(Vecf64<2> o, Sigmoid s)
    : offset(o)
    , sig(s)
  {
  }
};

ActuatorCalibration actuatorCalibs[N_TURN_ACT];
Matrixf64<2,N_TURN_ACT> JJ;

// Position Constants
f64 steeringKpp = 0.08;
f64 steeringKdp = 0.02;
f64 steeringKip = 0;

Vecf64<2> errorLastTrack(0, 0);
Vecf64<2> integralErrorTrack(0, 0);

f64 totalCalibTime = 3;
f64 totalRestTime = 2;
f64 incrementalCalibTime = 0.3;
f64 incrementalCalibRestTime = 15;
f64 calibTime = 0;
s8 currCalibAct = -1;
Vecf64<2> calibStartTrackPos(0, 0);

s32 outputCount = 0;

#define MAX_REGULATOR_PWM 255
#define CALIB_AMNT 0.2

Vecf64<2> trackPos;
Vecf64<2> scenePos;
Vecf64<2> desTrackPos(320, 240);

u32 lastTimeSteeringVisited = 0;

u8 calibPWM = 0;
#define CALIB_BIN_DS 1
#define CALIB_BIN_DOWNSAMPLE 2.0

#define MAP_VAL(amnt,a,b,c,d) (CLAMP((s32) ( (d*c + log10(MAX(amnt-a,EPS)) - log10(MAX(b-amnt,EPS)))/d ), 0, 255))
#define ACTUATE_REGULATOR(r,amnt) (analogWrite(regulatorPins[r], MAP_VAL(amnt,actuatorCalibs[r].sig.a,actuatorCalibs[r].sig.b,actuatorCalibs[r].sig.c,actuatorCalibs[r].sig.d)))
bool hasOutput = false;
void controlSteering()
{
  f64 u = 0;
  f64 vel;
  invalidPos = true;

  TIME_LOOP(controlMotors, 100);

  u32 currTime = micros();
  f64 dt = (f64)(currTime - lastTimeSteeringVisited) / (f64)1e6;
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

      if (currCalibAct >= 0) {
        actuatorCalibs[currCalibAct].offset = delta*(1.0/CALIB_AMNT);
        analogWrite(regulatorPins[currCalibAct], 0);
      }
    }
    if (doneResting) {
      calibTime = 0;
      currCalibAct++;
      if (currCalibAct >= N_TURN_ACT) {
        steeringControlMode = SCM_NOTHING;
        currCalibAct = -1;

        Serial.println("Done calibrating");
        for (s32 ii = 0; ii < N_TURN_ACT; ii++)
          Serial.println("Actuator " + String(ii) + " offset " + String(actuatorCalibs[ii].offset.x) + ", " + String(actuatorCalibs[ii].offset.y));
      }

      Vecf64<2> offsets[N_TURN_ACT] = { 0 };
      for (s32 ii = 0; ii < N_TURN_ACT; ii++)
        offsets[ii] = actuatorCalibs[ii].offset;
      JJ = Matrixf64<2, N_TURN_ACT>::FromCols(offsets);

      calibStartTrackPos = trackPos;
      ACTUATE_REGULATOR(currCalibAct, CALIB_AMNT);
    }

    break;
  }
  case SCM_CALIBRATE_INCREMENTAL:
  {
    bool incrementBin = false;
    bool doneResting = false;

    calibTime += dt;
    if (calibTime >= incrementalCalibTime*(calibPWM >> CALIB_BIN_DS))
      incrementBin = true;
    else
      calibStartTrackPos += scenePos;

    if ((calibTime >= incrementalCalibTime*(MAX_REGULATOR_PWM / CALIB_BIN_DOWNSAMPLE) + totalRestTime) || currCalibAct == -1) {
      Serial.println("Done resting actuator " + String((s32)currCalibAct));
      doneResting = true;
      incrementBin = false;
    } else if (calibTime >= incrementalCalibTime*(MAX_REGULATOR_PWM / CALIB_BIN_DOWNSAMPLE)) {
      return;
    }

    if (incrementBin) {
      Vecf64<2> delta = scenePos + calibStartTrackPos;
      Serial.println(String((s32)currCalibAct) + ", " + String(calibPWM) + ", " + String(delta.x) + ", " + String(delta.y));
#if 0 
      calibTrackDisplacements[currCalibAct][(calibPWM >> CALIB_BIN_DS)] = delta;
#endif
      calibPWM += (1 << CALIB_BIN_DS);
      if (calibPWM >= MAX_REGULATOR_PWM)
        calibPWM = 0;

      //ACTUATE_REGULATOR(currCalibAct, calibPWM/256.0);
      analogWrite(regulatorPins[currCalibAct], calibPWM);

      calibStartTrackPos = Vecf64<2>(0,0);
    }
    if (doneResting) {
      if (currCalibAct >= 0)
        analogWrite(regulatorPins[currCalibAct], 0);
      currCalibAct++;
      Serial.println("Incrementing Calibrator to " + String((s32)currCalibAct));
      if (currCalibAct >= N_TURN_ACT) {

        for (s32 ii = 0; ii < N_TURN_ACT; ii++)
          analogWrite(regulatorPins[ii], 0);

        steeringControlMode = SCM_NOTHING;

#if 0
        for (s32 jj = 0; jj < N_TURN_ACT; jj++) {
          String calibString = "Actuator " + String(jj) + " Calibration\n";
          for (s32 ii = 0; ii < (s32)(MAX_REGULATOR_PWM/CALIB_BIN_DOWNSAMPLE); ii++) {
            calibString += String(ii << CALIB_BIN_DS) + ", " + String(calibTrackDisplacements[jj][ii].magnitude()) + ", ";
          }
          Serial.println(calibString);
        }
#endif
      }
      calibStartTrackPos = Vecf64<2>(0,0);
      analogWrite(regulatorPins[currCalibAct], 0);
      calibTime = 0;
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
    Vecf64<2> error = desTrackPos - trackPos;
    Vecf64<2> derror = error - errorLastTrack;

    errorLastTrack = error;
    derror = derror*(1.0 / dt);

    Vecf64<2> u = error*steeringKpp + derror*steeringKdp + integralErrorTrack*steeringKip;
    if (!hasOutput)
      Serial.println("u " + String(u.x) + " " + String(u.y));

    Vecf64<N_TURN_ACT> qs = jc->Update(u, JJ);
    ACTUATE_REGULATOR(0, qs[0]);
    ACTUATE_REGULATOR(1, qs[1]);
    ACTUATE_REGULATOR(2, qs[2]);

    if ((outputCount++ % 60) == 0)
      Serial.println("q[0] q[1] q[2] " + String(qs[0]) + ", " + String(qs[1]) + ", " + String(qs[2]));

    hasOutput = true;

    //anti windup
    if (0 < qs[0] && qs[0] < 1 && 0 < qs[1] && qs[1] < 1 && 0 < qs[2] && qs[2] < 1) {
      integralErrorTrack.x += error.x;
      integralErrorTrack.y += error.y;
    }
  }
  }
}

f64 errorLastPosMotor = 0;
u8 dirLastMotor = 0;
f64 desPosMotor = 0;
f64 desVelMotor = 0;
u32 count = 0;
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
    else if(input[2] == 'i') {
      steeringControlMode = SCM_CALIBRATE_INCREMENTAL;
      currCalibAct = (s32)(atoi(&input[4]));
      mode = "calibration incremental";
    }
    else if (input[2] == 'r') {
      steeringControlMode = SCM_CALIBRATE_PAUSE;
      mode = "calibration paused";
    }
    else if (input[2] == 'p') {
      steeringControlMode = SCM_POS;
      mode = "position";
    }
    else if (input[2] == 'n') {
      steeringControlMode = SCM_NOTHING;
      mode = 'none';
      calibPWM = 0;
      calibStartTrackPos = Vecf64<2>(0, 0);
      calibTime = 0;
      currCalibAct = 0;
      for (s32 jj = 0; jj < N_TURN_ACT; jj++)
        analogWrite(regulatorPins[jj], 0);
    }
    else if (input[2] == 'k') {
      f64 *Kp, *Kd, *Ki;
      Kp = &steeringKpp;
      Kd = &steeringKdp;
      Ki = &steeringKip;
      f64 val = atof(input + 5);
      if (input[3] == 'p') {
        *Kp = val;
        Serial.println("Set Kp to " + String(val));
      }
      else if (input[3] == 'd') {
        *Kd = val;
        Serial.println("Set Kd to " + String(val));
      }
      else if (input[3] == 'i') {
        *Ki = val;
        Serial.println("Set Ki to " + String(val));
      }
    }
    if (input[2] == 'c' || input[2] == 'p' || input[2] == 'n')
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
  case 'e':
  case 'E': //Servo Valve
  {
    f64 pos = atof(input + 2);
    f64 angle = (closeAngle - openAngle)*pos + openAngle;
    Serial.println("Setting servo valve position " + String(angle));
    extensionServo.write(angle);
    break;
  }
  case 'l':
  case 'L': //Toggle Pin
  {
    s32 reg = atoi(input + 2);
    if (reg < 1 || reg > N_TURN_ACT)
      return;
    f64 amount = atof(input + 4);
    Serial.println("Setting valve " + String(reg) + " to " + String(amount) + " input string " + String(input));
    ACTUATE_REGULATOR(reg - 1, amount);
    Serial.println("pwm amount " + String(MAP_VAL(amount, actuatorCalibs[reg - 1].sig.a, actuatorCalibs[reg - 1].sig.b, actuatorCalibs[reg - 1].sig.c, actuatorCalibs[reg - 1].sig.d)));
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
  extensionServo.write(closeAngle);

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

                             //regulator Pins
  for (s32 ii = 0; ii < N_TURN_ACT; ii++) {
    pinMode(regulatorPins[ii], OUTPUT);
    digitalWrite(regulatorPins[ii], LOW);
  }

  // Encoder Pins
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);

  // Init Motor
  analogWrite(pwmAPin, 0);     // set to not be spinning (0/255)
  digitalWrite(dirAPin, LOW);  // set direction

#if 1
                               // Set up actuator calibrationsA
  actuatorCalibs[0] = ActuatorCalibration(Vecf64<2>(0, 0), Sigmoid(0.003146, 0.984901, 127.109794, 0.033328));
  actuatorCalibs[1] = ActuatorCalibration(Vecf64<2>(0, 0), Sigmoid(0.049058, 0.976153, 182.573890, 0.028746));
  actuatorCalibs[2] = ActuatorCalibration(Vecf64<2>(0, 0), Sigmoid(0.011475, 0.971715, 144.373752, 0.042064));
#else
  actuatorCalibs[0] = ActuatorCalibration(Vec2f64(0,0), Sigmoid(-0.000100, 0.982211, 116.866446, 0.041995));
  actuatorCalibs[1] = ActuatorCalibration(Vec2f64(0,0), Sigmoid(0.047229, 0.946368, 176.593023, 0.036252));
  actuatorCalibs[2] = ActuatorCalibration(Vec2f64(0,0), Sigmoid(0.015325, 0.974421, 134.341202, 0.060189));

  f64 qs[N_TURN_ACT]; memset(&qs[0], 0, sizeof(f64) * 3);
  jc->SetQs(qs);
#endif

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