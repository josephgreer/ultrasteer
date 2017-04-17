// Includes 
#include <TimerOne.h>
#include <Encoder.h>
#include <math.h>
#include <SoftwareSerial.h>
#include "C:/Joey/ultrasteer/src/Vine/HW/Winch/Code/Control/Common.h"

// Pin Declares
int pwmAPin = 5;  
int dirAPin = 8;  
int encoderPinA = 2;
int encoderPinB = 3; 
int pressurePin = 9;
int solenoidPinLeft = 12;
int solenoidPinRight = 13;
int slaSerialRx = 11;
int slaSerialTx = 10;
double globalPressureSetPoint = 0;
double currPressureSetPoint = 0;
double pressureDecayRate = 0.01;
double motorToPressureConstant = 1;

// Program Scope Variables
Encoder encoder(encoderPinA, encoderPinB);
f64 countPerRev = 1024;
int counter = 0; 

SoftwareSerial slaSerial(slaSerialRx, slaSerialTx);

// --------------------------
// Initialize
// --------------------------
void setup() 
{
  // Set Up Serial
  Serial.begin(57600);
  Serial.setTimeout(15);

  slaSerial.begin(57600);
//  slaSerial.setTimeout(15);
  
  // Set PWM frequency 
  setPwmFrequency(pwmAPin,1); //PWM on pin 3 is (32500/1) = 32500 [Hz] 
  
  // Output Pins
  pinMode(pwmAPin, OUTPUT);  // PWM for A
  pinMode(dirAPin, OUTPUT);  // dir for A

  pinMode(solenoidPinLeft, OUTPUT);
  pinMode(solenoidPinRight, OUTPUT);

  digitalWrite(solenoidPinLeft, LOW);
  digitalWrite(solenoidPinRight, LOW);

  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);

  pinMode(pressurePin, OUTPUT);
  
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
  CM_THROTTLE = 2,
  CM_NONE = 3,
};

f64 errorLast = 0;
u8 dirLast = 0;
u32 lastTime = 0;
f64 desPos = 0;
f64 desVel = 0;
f64 desPres = 0;
u32 count = 0;
f64 integralError = 0;
f64 lastPos = 0;
f64 lastVel = 0;

// Position Constants
f64 Kpp = 0.08;
f64 Kdp = 0;
f64 Kip = 0.00005;

// Velocity Constants
f64 Kpv = 1;
f64 Kdv = 0;
f64 Kiv = 0.01;

CONTROL_MODE controlMode = CM_POS;

u8 unwindDir = 1;

f64 lowPassDesVel = 0;

void loop() 
{
  u32 currTime = millis();
  f64 dt = (f64)(currTime-lastTime)/(1000.0);
  lastTime = currTime;

  if(slaSerial.available() > 0) {
    handleSlaSerial();
  }

  controlMotors(dt);
  controlSteering(dt);

}

Vecf64<2> trackPos(-1,-1);
Vecf64<2> lastTrackPos(-1,-1);

f64 rotation = 0;
#define DEADBAND 50
bool steeringEnabled = false;
unsigned int g_count = 0;
f64 totalTime = 0;
void controlSteering(f64 dt)
{
  u8 us[2] = {HIGH,HIGH}; 
  Vecf64<2> currTrackPos(-1,-1);
  if(steeringEnabled) {
    if(trackPos.x > 0 && trackPos.y > 0) {
      f64 theta = rotation*PI/180.0;
      Matrixf64<2, 2> rot(cos(theta), -sin(theta), sin(theta), cos(theta));
      Vecf64<2> center(320,240);
      currTrackPos = rot*(trackPos-center)+center;
    } else if(lastTrackPos.x > 0 && lastTrackPos.y > 0) {
      currTrackPos = lastTrackPos;
    }

    if(currTrackPos.x > 0 && currTrackPos.y > 0) {
      if(currTrackPos.x > 320+DEADBAND)
        us[0] = LOW;
      if(currTrackPos.x < 320-DEADBAND)
        us[1] = LOW;
    }
    digitalWrite(solenoidPinLeft, us[0]);
    digitalWrite(solenoidPinRight, us[1]);
    lastTrackPos = currTrackPos;
  }
  if(g_count++ % 1000)
    Serial.println(String(currPressureSetPoint, 5) + " " + String(desVel, 6) + " " + String(lastVel,6)); 
//  if(totalTime > 0.1) {
//    Serial.println("P " + String(millis()) + ", " + String(us[0]) + ", " + String(us[1]) + ", " + String(currTrackPos.x) + ", " + String(currTrackPos.y) + ", " + String(rotation,6) + ", " + 
//    String(lastPos,6) + ", " + String(lastVel,6) + ", " + String(currPressureSetPoint,6) + ", " + String(globalPressureSetPoint,6) + ";");
//    totalTime = 0;
//  }
}

void controlMotors(f64 dt)
{
  f64 u;
  f64 vel;
  f64 pos = encoder.read();
  f64 error, derror;

  lowPassDesVel = lowPassDesVel*0.9999+(1-0.9999)*desVel;

  vel = 0.2*(pos-lastPos)/dt + 0.8*lastVel;
  if(controlMode == CM_POS) {
    error = desPos-pos;
  } else {
    error = lowPassDesVel-vel;
    lastVel = vel;
  }
  derror = error-errorLast;
  integralError += error;
    
  errorLast = error;
  derror /= dt;
  lastPos = pos;

  s8 dir = 0;
  
  switch(controlMode) {
    case CM_POS:
    {
      u = Kpp*error + Kdp*derror + Kip*integralError;
  
      if(u < 0) {
        dir = 1;
        u = abs(u);
      }
      u = min(u, 255);
      break;
    }
    case CM_VEL:
    {
      u = Kpv*error + Kdv*derror + Kiv*integralError;
  
      if(u < 0) {
        dir = 1;
        u = abs(u);
      }
      u = min(u, 255);
      break;
    }
    case CM_THROTTLE:
    {
      u = Kpv*error + Kdv*derror + Kiv*integralError;
  
      if(u < 0) {
        currPressureSetPoint = globalPressureSetPoint + abs(u)*motorToPressureConstant;
        currPressureSetPoint = min(currPressureSetPoint,255);
        if(currPressureSetPoint >= 255) {
          integralError -= error;
        }
        analogWrite(pressurePin, currPressureSetPoint);
        u = 0;
        dir = 1-unwindDir;
        derror = 0;
      } else {
        currPressureSetPoint = currPressureSetPoint*(1-pressureDecayRate)+(globalPressureSetPoint-currPressureSetPoint)*pressureDecayRate;
        u = min(u, 255);
      }
      break;
    }
  }

  if(controlMode != CM_NONE) {
    if(dir != dirLast) {
      digitalWrite(dirAPin, dir);
      dirLast = dir;
    }
//    if(count++ % 2000 == 0) 
//      Serial.println("Des Pos " +  String(desPos) + " Pos " + String(pos) + " DesVel " + String(desVel) + " Vel " + String(vel) + " Pressure " + String(desPres) + " Error " + String(error) + " u " + String(u) + " dt " + String(dt*1000.0) + " derror " + String(derror) + " integralError " + String(integralError));
    analogWrite(pwmAPin, u);
  }
}

void SetVel(f64 vel)
{
  if(abs(vel) < abs(desVel))
    lowPassDesVel = vel;
  desVel = vel; 
}

f64 unwindSign = -1;
f64 popVel = -30;
f64 nonPopVel = -150;
f64 popPressure = 1;
f64 nonPopPressure = 0.4;

u8 defaultPop = true;

void SetMode(u8 mode)
{
  controlMode = (CONTROL_MODE)mode;
  lowPassDesVel = 0;
  desVel = integralError = 0;
  errorLast = 0;
  desPos = lastPos;
  integralError = 0;
  lastVel = 0;
}

#define BUFFER_LEN 100
u8 input[BUFFER_LEN] = {0};
s32 nBytes = 0;
u32 trackCount = 0;

void handleSlaSerial()
{
  nBytes = slaSerial.readBytes(input, BUFFER_LEN);
#if 0
  char printStr[100] = { 0 };
  s8 *strPtr = printStr;
  for (s32 i = 0; i < nBytes; i++) {
    strPtr += sprintf(strPtr, "%02X ", input[i]);
  }
  if(trackCount++ % 30 == 0)
    Serial.println(String(printStr) + " Length " + String(nBytes));
#endif

  if (input[0] != 0x51 || input[1] != 0xAC || input[3] != 0x43)
    return;
  int trackRow = (s16)input[6] + ((s16)input[7] << 8);
  int trackCol = (s16)input[4] + ((s16)input[5] << 8);

  trackPos = Vecf64<2>(trackCol, trackRow);

  s32 sceneRow8 = (input[10]<<8) + (input[11] << 16) + (input[23]);
  s32 sceneCol8 = (input[8]<<8) + (input[9] << 16) + (input[22]);

  s16 sceneRot7 = (s16)input[24] | ((s16)(input[25] << 8));
  rotation += sceneRot7/128.0;

//  if(trackCount++ % 30 == 0)
//    Serial.println("track Row " + String(trackRow) + " track Col " + String(trackCol));
}

void serialEvent() 
{
  if(Serial.available()) {
    nBytes = Serial.readBytes(input, BUFFER_LEN);

    switch(input[0]) {
      case 'e':
      case 'E':
      {
        steeringEnabled = !steeringEnabled;
        if(!steeringEnabled) {
          digitalWrite(solenoidPinLeft, LOW);
          digitalWrite(solenoidPinRight, LOW);
        }
        String("Steering enabled " + String(steeringEnabled));
        rotation = 0;
        break;
      }
      case 'v':
      case 'V':
      {
        f64 vel = atof((char *)&input[2]);
        SetVel(vel);
        break;
      }
      case 'm':   // switch mode 
      {
        String mode;
        CONTROL_MODE mm;
        if(input[2] == 'v') {               //velocity
          mm = CM_VEL;
          mode = "velocity";
        } else if(input[2] == 'p') {        //position
          mm = CM_POS;
          mode = "position";
        } else if(input[2] == 't') {        //throttle
          mm = CM_THROTTLE;
          mode = "throttle";
        }

        SetMode(mm);
        
        Serial.println("Control mode switched to " + mode);   
        break;
      }
      case 'q':  // decrement set point. interpretation depends on mode
      {
        f64 decrement = atof((char *)input+2);
        if(controlMode == CM_POS)
          desPos -= decrement;
        else
          desVel -= decrement;

        Serial.println("Decrementing " + String(decrement));
        break;
      }
      case 'w':   // increment set point. interpretation depends on mode
      {
        f64 increment = atof((char *)input+2);
        if(controlMode == CM_POS)
          desPos += increment ;
        else
          desVel += increment;
          
        Serial.println("Incrementing " + String(increment));
        break;
      }
      case 's':
      case 'S':
      {
        s32 solenoid = atoi((char *)&input[2]);
        s32 amount = atoi((char *)&input[4]);
        s32 pinn = solenoid == 1 ? solenoidPinLeft : solenoidPinRight;

        Serial.println("Setting solenoid " + String(solenoid) + " amount " + String(amount));
        if(amount > 0)
          digitalWrite(pinn, HIGH);
        else {
          digitalWrite(pinn, LOW);
          Serial.println("Set low");
        }
          
        break;
      }
      case 'K':   //set Kp/Kd/Ki
      case 'k':
      {
        f64 *Kp, *Kd, *Ki;
        if(controlMode == CM_VEL) {
          Kp = &Kpv;
          Kd = &Kdv;
          Ki = &Kiv;
        } else {
          Kp = &Kpp;
          Kd = &Kdp;
          Ki = &Kip;
        }
        f64 val = atof((char *)input+3);
        if(input[1] == 'p') {
          *Kp = val;
          Serial.println("Set Kp to " + String(val));
        } else if(input[1] == 'd') {
          *Kd = val;
          Serial.println("Set Kd to " + String(val));
        } else if(input[1] == 'i') {
          *Ki = val;
          Serial.println("Set Ki to " + String(val));
        }
        break;
      }
      case 'l':
      case 'L':
      {
        if(input[2] == 'i') {
          desPres = min(max(atof((char *)&input[4])+desPres,0),1);
        } else if(input[2] == 'd') {
          desPres = min(max(desPres-atof((char *)&input[4]),0),1);
        } else {
          desPres = min(max(atof((char *)&input[2]),0),1);
        }
        s32 amount = (s32)(desPres*255.0);
        globalPressureSetPoint = amount;

        Serial.println("Setting pressure to " + String(desPres,6) + " pwm " + String(amount));
        analogWrite(pressurePin, amount);
        break;
      }
      case 'a':
      case 'A':
      {
        u8 dir = atoi((char *)&input[2]);
        f64 amnt = atof((char *)&input[4]);

        s32 amount = (s32)(amnt*255.0);

        Serial.println("Setting motor to " + String(amnt,6) + " pwm " + String(amount) + " direction " + String(dir));
        digitalWrite(dirAPin, dir > 0 ? HIGH : LOW);
        analogWrite(pwmAPin, amount);
        controlMode = CM_NONE;
        break;
      }
      case 'p':   //Pause
      case 'P':
      {
        lowPassDesVel = desVel = integralError = 0;
        errorLast = 0;
        desPos = lastPos;
        integralError = 0;
        lastVel = 0;

        Serial.println("Pause");
        break;
      }
      default:
        break;
    }
    memset(input,0,BUFFER_LEN);
  }
}

// --------------------------
// Set PWM Freq 
// --------------------------
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
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
