// Includes
#include <TimerOne.h>
#include <Encoder.h>
#include <SoftwareSerial.h>
#include <math.h>

typedef double f64;
typedef char s8;
typedef unsigned char u8;
typedef unsigned long u32;
typedef int s32;

// Pin Declares
int pwmAPin = 5;
int dirAPin = 8;
int encoderPinA = 11;
int encoderPinB = 3;

int regulator1Pin = 9;
int regulator2Pin = 10;

int slaSerialPinRx = 2;
int slaSerialPinTx = 12;

// Program Scope Variables
Encoder encoder(encoderPinA, encoderPinB);
f64 countPerRev = 1024;
int counter = 0;

SoftwareSerial slaSerial(slaSerialPinRx, slaSerialPinTx);

// --------------------------
// Initialize
// --------------------------
void setup()
{
  pinMode(slaSerialPinRx, INPUT);

  // Set Up Serial
  Serial.begin(57600);
  slaSerial.begin(57600);
  slaSerial.setTimeout(1);

  // Set PWM frequency
  setPwmFrequency(pwmAPin, 1); //PWM on pin 3 is (32500/1) = 32500 [Hz]

  // Output Pins
  pinMode(pwmAPin, OUTPUT);  // PWM for A
  pinMode(dirAPin, OUTPUT);  // dir for A

  //regulator Pins
  pinMode(regulator1Pin, OUTPUT);  // PWM for A
  pinMode(regulator2Pin, OUTPUT);  // dir for A

  // Encoder Pins
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);

  // Init Motor
  analogWrite(pwmAPin, 0);     // set to not be spinning (0/255)
  digitalWrite(dirAPin, LOW);  // set direction

  digitalWrite(regulator1Pin, LOW);
  digitalWrite(regulator2Pin, HIGH);

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
  SCM_CALIBRATE = 1,
  SCM_POS = 2
};

u32 lastTime = 0;

CONTROL_MODE motorControlMode = CM_POS;
STEERING_CONTROL_MODE steeringControlMode = SCM_NOTHING;

u8 unwindDir = 1;


#define TRACE_COUNT_TRACKS 50
#define TRACE_COUNT_MOTORS 50
#define BUFFER_LEN 50
s8 input[BUFFER_LEN] = {0};
s32 nBytes = 0;

void loop()
{
  u32 currTime = millis();
  f64 dt = (f64)(currTime - lastTime) / (1000.0);
  lastTime = currTime;

  //MOTOR CONTROL
  controlMotors(dt);

  //STEERING CONTROL
  controlSteering(dt);

  //SERIAL HANDLER
#if 1
  if (Serial.available()) {
    nBytes = Serial.readBytes(input, BUFFER_LEN);
    handleSerial(input, nBytes);
    memset(input, 0, nBytes);
  }
#endif

  if (slaSerial.available()) {
    nBytes = slaSerial.readBytes(input, BUFFER_LEN);
    handleSlaSerial((u8 *)input, nBytes);
    //memset(input, 0, nBytes);
  }
}

f64 vTrackRow = 0;
f64 vTrackCol = 0;

f64 desPosTrackRow = 0;
f64 desPosTrackCol = 0;
void controlSteering(f64 dt)
{
  f64 u = 0;
  f64 vel;

  switch(steeringControlMode) 
  {
    case SCM_NOTHING: 
    {
      break;
    }
    case SCM_CALIBRATE:
    {
      break;
    }
    case SCM_POS:
    {
      break;
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
bool printOutTracks = true;

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
    case 'l':
    case 'L': //Toggle Pin
      {
        s32 reg = atoi(input + 2);
        f64 amount = atof(input + 4);
        u8 amnt = (u8)(amount * 255.0);
        Serial.println("Setting valve " + String(reg) + " to " + String(amount) + " input string " + String(input) + " amount " + String(amnt));
        if(reg == 1)
          analogWrite(regulator1Pin, amnt);
        else if(reg == 2)
          analogWrite(regulator2Pin, amnt);
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
  vTrackRow = (f64)trackRow;
  vTrackCol = (f64)trackCol;

  if((serialCount++ % TRACE_COUNT_TRACKS) == 0 && printOutTracks) {
    u32 currTime = millis();
    Serial.println("track row track col " + String(trackRow) + " " + String(trackCol) + " time " + String((currTime-lastTimeTracks)/(f64)TRACE_COUNT_TRACKS));
    lastTimeTracks = currTime;
    serialCount = 1;
  }
}

// --------------------------
// Set PWM Freq
// --------------------------
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if (pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if (pin == 3 || pin == 11) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
