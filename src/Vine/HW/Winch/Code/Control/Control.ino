// Includes
#include <TimerOne.h>
#include <Encoder.h>
#include <SoftwareSerial.h>
#include "Common.h"
//#include "ServoValve.h"
#include <math.h>
#include <Servo.h>
//#define SLA_SERIAL

#define N_TURN_ACT 3

// Pin Declares
int pwmAPin = 5;
int dirAPin = 8;
int encoderPinA = 13;
int encoderPinB = 3;

int servoValvePin = 45;
f64 openAngle = 70;
f64 closeAngle = 90;


s32 regulatorPins[N_TURN_ACT]  = {11, 9, 10};

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
	for(s32 ii=0; ii<N_TURN_ACT; ii++) {
		pinMode(regulatorPins[ii], OUTPUT);
		digitalWrite(regulatorPins[ii], LOW);
	}

  // Encoder Pins
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);

  // Init Motor
  analogWrite(pwmAPin, 0);     // set to not be spinning (0/255)
  digitalWrite(dirAPin, LOW);  // set direction
  
  // Init Timer and Set Haptic Loop
  //Timer1.initialize();
  //long period = 10000; // 10000 [us] -> 100 Hz sample time
  //Timer1.attachInterrupt(hapticLoop,period);
}

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
s8 input[BUFFER_LEN] = {0};
s32 nBytes = 0;

void loop()
{
	TIME_LOOP(MainLoop, 50000);
  u32 currTime = micros();
  f64 dt = ((f64)currTime - (f64)lastTime) / ((f64)1e6);
  lastTime = currTime;

  //MOTOR CONTROL
  controlMotors(dt);

	if(!invalidPos) {
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

struct ActuatorCalibration 
{
	f64 angle;
	f64 strength;
};

ActuatorCalibration actuatorCalibs[N_TURN_ACT] = {0};
u8 sortedAngleIdxs[N_TURN_ACT] = {0};

// Position Constants
f64 steeringKpp = 0.01;
f64 steeringKdp = 0;
f64 steeringKip = 0;

Vec2f64 errorLastTrack(0,0);
Vec2f64 integralErrorTrack(0,0);

f64 totalCalibTime = 0.800;
f64 totalRestTime = 2;
f64 calibTime = 0;
s8 currCalibAct = -1;
Vec2f64 calibStartTrackPos(0,0);

Vec2f64 trackPos;
Vec2f64 scenePos;
Vec2f64 desTrackPos(320, 240);

u32 lastTimeSteeringVisited = 0;

bool hasOutput = false;
void controlSteering()
{
  f64 u = 0;
  f64 vel;
	invalidPos = true;

	u32 currTime = micros();
	f64 dt = (f64)(currTime-lastTimeSteeringVisited)/(f64)1e6;
	if(lastTimeSteeringVisited == 0)
		dt = 0;
	lastTimeSteeringVisited = currTime;

  switch(steeringControlMode) 
  {
    case SCM_NOTHING: 
    {
      break;
    }
    case SCM_CALIBRATE:
    {
			bool transition = false;
			bool doneResting = false;

			if(currCalibAct == -1) {
				transition = true;
			}

			calibTime += dt;
			if(calibTime >= totalCalibTime)
				transition = true;

			if(calibTime >= totalCalibTime+totalRestTime) {
				Serial.println("Done resting actuator " + String((s32)currCalibAct));
				doneResting = true;
			}

			if(transition) {
				Vec2f64 delta = trackPos-calibStartTrackPos;

				if(currCalibAct >= 0) {
					actuatorCalibs[currCalibAct].angle = delta.angle();
					actuatorCalibs[currCalibAct].strength = delta.magnitude();
					analogWrite(regulatorPins[currCalibAct], 0);
				}
			}
			if(doneResting) {
				calibTime = 0;
				currCalibAct++;
				if(currCalibAct >= N_TURN_ACT) {
					steeringControlMode = SCM_NOTHING;
					currCalibAct = -1;

					memset(sortedAngleIdxs, 0, sizeof(sortedAngleIdxs));
					for(s32 ii=0; ii<N_TURN_ACT; ii++) {
						sortedAngleIdxs[ii] = ii;
						s32 idx = ii-1;
						while(idx >= 0 && actuatorCalibs[ii].angle < actuatorCalibs[sortedAngleIdxs[idx]].angle) {
							u8 tempIdx = sortedAngleIdxs[idx];
							sortedAngleIdxs[idx] = ii;
							sortedAngleIdxs[idx+1] = tempIdx;
							idx--;
						}
					}

					Serial.println("Calibration: ");
					for(s32 ii=0; ii<N_TURN_ACT; ii++) {
						Serial.println("ii = " + String(ii+1) + " angle = " + String(180*actuatorCalibs[ii].angle/PI) + " strength = " + String(actuatorCalibs[ii].strength));
					}
					Serial.println("Sorted Angle Idxs:  ");
					for(s32 ii=0; ii<N_TURN_ACT; ii++)
						Serial.println(String(sortedAngleIdxs[ii]));
					return;
				}
				calibStartTrackPos = trackPos;
				analogWrite(regulatorPins[currCalibAct], 255);
			}

      break;
    }
    case SCM_POS:
    {
			Vec2f64 error = desTrackPos-trackPos;
			Vec2f64 derror = error - errorLastTrack;

			errorLastTrack = error;
			derror = derror*(1.0/dt);

			Vec2f64 u = error*steeringKpp + derror*steeringKdp + integralErrorTrack*steeringKip;
			if(!hasOutput)
				Serial.println("u " + String(u.x) + " " + String(u.y));

			//anti windup
			if(u.x < 1)
				integralErrorTrack.x += error.x;
			if(u.y < 1)
				integralErrorTrack.y += error.y;

			f64 utheta = fmodf(u.angle()+PI,PI);
			f64 umag = u.magnitude();

			s8 firstLessThanAngleIdx = N_TURN_ACT-1;
			while(actuatorCalibs[sortedAngleIdxs[firstLessThanAngleIdx]].angle > utheta && firstLessThanAngleIdx >= 0)
				firstLessThanAngleIdx--;

			u8 angle1Idx, angle2Idx;
			if(firstLessThanAngleIdx < 0)
				angle1Idx = N_TURN_ACT-1;
			else
				angle1Idx = firstLessThanAngleIdx;

			if(firstLessThanAngleIdx == N_TURN_ACT-1)
				angle2Idx = 0;
			else
				angle2Idx = firstLessThanAngleIdx+1;

			angle1Idx = sortedAngleIdxs[angle1Idx];
			angle2Idx = sortedAngleIdxs[angle2Idx];

			f64 dist1, dist2;
			dist1 = ABS(utheta-actuatorCalibs[angle1Idx].angle);
			dist1 = MIN(dist1, 2*PI-dist1);
			dist2 = ABS(utheta-actuatorCalibs[angle2Idx].angle);
			dist2 = MIN(dist2, 2*PI-dist2);

			if(!hasOutput)
				Serial.println("utheta " + String(180.0/PI*utheta) + " actuator1 " + angle1Idx + " actuator2 " + angle2Idx + " angle1 " + String(180.0/PI*actuatorCalibs[angle1Idx].angle) + " angle2 " + String(180.0/PI*actuatorCalibs[angle2Idx].angle) + " distance1 " + String(180.0/PI*dist1) + " distance2 " + String(180.0/PI*dist2));

			f64 u1 = (dist2/(dist1+dist2));
			f64 u2 = 1-u1;
			if(!hasOutput)
				Serial.println("u1 u2 " + String(u1) + " " + String(u2));
			u1 = u1*umag;
			u2 = u2*umag;
			if(!hasOutput)
				Serial.println("u1 u2 after mag " + String(u1) + " " + String(u2));
			s32 u1amnt = (s32)(255.0*MIN(u1,1)+0.5);
			s32 u2amnt = (s32)(255.0*MIN(u2,1)+0.5);
			if(!hasOutput)
				Serial.println("u1amnt u2amnt " + String(u1amnt) + " " + String(u2amnt));

			hasOutput = true;

			for(s32 ii=0; ii<N_TURN_ACT; ii++) {
				if(ii != angle1Idx || ii != angle2Idx)
					analogWrite(regulatorPins[ii], 0);
			}

			analogWrite(regulatorPins[angle1Idx], u1amnt);
			analogWrite(regulatorPins[angle2Idx], u2amnt);

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
  } else {
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
      Serial.println("Des Pos " +  String(desPosMotor) + " Pos " + String(pos) + " Error " + String(error) + " u " + String(u) + " dt " + String(dt * 1000.0) + " derror " + String(derror) + " integralErrorMotor " + String(integralErrorMotor));
    else if (count % TRACE_COUNT_MOTORS == 0 && motorControlMode == CM_VEL)
      Serial.println("DesVel " + String(desVelMotor) + " Vel " + String(vel) + " Dir " + String((s32)dir) + " Error " + String(error) + " u " + String(u) + " dt " + String(dt * 1000.0) + " derror " + String(derror) + " integralErrorMotor " + String(integralErrorMotor));
    else if (count % TRACE_COUNT_MOTORS == 0 && motorControlMode == CM_THROTTLE)
      Serial.println("DesVel " + String(desVelMotor) + " Vel " + String(vel) + " Dir " + String((s32)dir) + " Error " + String(error) + " u " + String(u) + " dt " + String(dt * 1000.0) + " derror " + String(derror) + " integralErrorMotor " + String(integralErrorMotor));
    analogWrite(pwmAPin, u);
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
        } else if (input[2] == 'p') {       //position
          motorControlMode = CM_POS;
          mode = "position";
        } else if (input[2] == 't') {       //throttle
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
				if(input[2] == 'c') {
					steeringControlMode = SCM_CALIBRATE;
					mode = "calibrate";
				} else if(input[2] == 'p') {
					steeringControlMode = SCM_POS;
					mode = "position";
				} else if(input[2] == 'n') {
					steeringControlMode = SCM_NOTHING;
					mode = 'none';
				} else if(input[2] == 'k') {
					f64 *Kp, *Kd, *Ki;
					Kp = &steeringKpp;
					Kd = &steeringKdp;
					Ki = &steeringKip;
					f64 val = atof(input + 5);
					if (input[3] == 'p') {
						*Kp = val;
						Serial.println("Set Kp to " + String(val));
					} else if (input[1] == 'd') {
						*Kd = val;
						Serial.println("Set Kd to " + String(val));
					} else if (input[1] == 'i') {
						*Ki = val;
						Serial.println("Set Ki to " + String(val));
					} 
				}
				if(input[2] == 'c' || input[2] == 'p' || input[2] == 'n')
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
          desPosMotor += increment ;
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
        } else {
          Kp = &motorKpp;
          Kd = &motorKdp;
          Ki = &motorKip;
        }
        f64 val = atof(input + 3);
        if (input[1] == 'p') {
          *Kp = val;
          Serial.println("Set Kp to " + String(val));
        } else if (input[1] == 'd') {
          *Kd = val;
          Serial.println("Set Kd to " + String(val));
        } else if (input[1] == 'i') {
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
        f64 angle = (closeAngle-openAngle)*pos+openAngle;
        Serial.println("Setting servo valve position " + String(angle));
        extensionServo.write(angle);
        break;   
      }
    case 'l':
    case 'L': //Toggle Pin
      {
        s32 reg = atoi(input + 2);
				if(reg < 1 || reg > N_TURN_ACT)
					return;
        f64 amount = atof(input + 4);
        u8 amnt = (u8)(amount * 255.0);
        Serial.println("Setting valve " + String(reg) + " to " + String(amount) + " input string " + String(input) + " amount " + String(amnt));
				analogWrite(regulatorPins[reg-1], amnt);
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
  char printStr[100] = {0};
  s8 *strPtr = printStr;
  for (s32 i = 0; i < nBytes; i++) {
    strPtr += sprintf(strPtr, "%02X ", input[i]);
  }
  Serial.println(String(printStr) + " Length " + String(nBytes));
  #endif

  //not the correct header;
  if (input[0] != 0x51 || input[1] != 0xAC || input[3] != 0x43)
    return;

  u16 trackRow = input[6] + (input[7] << 8);
  u16 trackCol = input[4] + (input[5] << 8);

	u16 sceneRow = input[10] + (input[11] << 8);
	u16 sceneCol = input[8] + (input[9] << 8);

  trackPos = Vec2f64((f64)trackCol,(f64)trackRow);
	scenePos = Vec2f64((f64)sceneCol, (f64)sceneRow);
	invalidPos = false;

  if((serialCount++ % TRACE_COUNT_TRACKS) == 0 && printOutTracks) {
    u32 currTime = millis();
    Serial.println("track row track col " + String(trackRow) + " " + String(trackCol) + " time " + String((currTime-lastTimeTracks)/(f64)TRACE_COUNT_TRACKS));
    lastTimeTracks = currTime;
    serialCount = 1;
  }
}
