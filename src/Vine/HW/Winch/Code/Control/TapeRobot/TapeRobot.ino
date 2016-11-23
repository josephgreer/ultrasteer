// Includes 
#include <TimerOne.h>
#include <Encoder.h>
#include <math.h>

typedef double f64;
typedef char s8;
typedef unsigned char u8;
typedef unsigned long u32;
typedef int s32;

// Pin Declares
int pwmAPin = 5;  
int dirAPin = 8;  
int encoderPinA = 2;
int encoderPinB = 3; 
int pressurePin = 9;

// Program Scope Variables
Encoder encoder(encoderPinA, encoderPinB);
f64 countPerRev = 1024;
int counter = 0; 

// --------------------------
// Initialize
// --------------------------
void setup() 
{
  // Set Up Serial
  Serial.begin(115200);
  Serial.setTimeout(15);
  
  // Set PWM frequency 
  setPwmFrequency(pwmAPin,1); //PWM on pin 3 is (32500/1) = 32500 [Hz] 
  
  // Output Pins
  pinMode(pwmAPin, OUTPUT);  // PWM for A
  pinMode(dirAPin, OUTPUT);  // dir for A

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

f64 minTension = 5.0;

CONTROL_MODE controlMode = CM_POS;

u8 unwindDir = 1;

//encoder tick of first actuator
f64 firstAct = 9000;
f64 actSpacing = 11250;
u8 currAct = 0;

f64 lowPassDesVel = 0;

void loop() 
{
  f64 pos = encoder.read();
  u32 currTime = millis();
  f64 dt = (f64)(currTime-lastTime)/(1000.0);
  f64 u;
  f64 vel;
  
  f64 error, derror;

  handlePopState(pos);

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
  lastTime = currTime;
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
  
      if(u < minTension) {
        u = minTension;
        integralError = 0;
        derror = 0;
        dir = 1-unwindDir;
      }
      u = min(u, 255);
      break;
    }
  }

  if(controlMode != CM_NONE) {
    if(dir != dirLast) {
      digitalWrite(dirAPin, dir);
      dirLast = dir;
    }
    if(count % 499 == 0)
      Serial.println("act " + String(currAct+1));
    if(count++ % 2000 == 0) 
      Serial.println("Des Pos " +  String(desPos) + " Pos " + String(pos) + " DesVel " + String(desVel) + " Vel " + String(vel) + " Pressure " + String(desPres) + " Error " + String(error) + " u " + String(u) + " dt " + String(dt*1000.0) + " derror " + String(derror) + " integralError " + String(integralError));
    analogWrite(pwmAPin, u);
  }

}

void SetVel(f64 vel)
{
  if(abs(vel) < abs(desVel))
    lowPassDesVel = vel;
  desVel = vel; 
}

u32 zeroPoint = 0;

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

void SetPop(bool pop)
{
  if(controlMode == CM_THROTTLE) {
    desPres = pop ? popPressure : nonPopPressure;
    s32 amount = (s32)(255.0*(desPres));
    analogWrite(pressurePin, amount);
    SetVel(pop ? popVel : nonPopVel);
  }
}

void handlePopState(f64 encoderTick)
{
  if(controlMode == CM_THROTTLE && unwindSign*(encoderTick-zeroPoint) > (firstAct+currAct*actSpacing)) {
    currAct = currAct+1;

    Serial.println("act " + String(currAct+1));
  }
}

#define BUFFER_LEN 20
s8 input[BUFFER_LEN] = {0};
s32 nBytes = 0;

void serialEvent() 
{
  if(Serial.available()) {
    nBytes = Serial.readBytes(input, BUFFER_LEN);

    switch(input[0]) {
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
        f64 decrement = atof(input+2);
        if(controlMode == CM_POS)
          desPos -= decrement;
        else
          desVel -= decrement;

        Serial.println("Decrementing " + String(decrement));
        break;
      }
      case 'w':   // increment set point. interpretation depends on mode
      {
        f64 increment = atof(input+2);
        if(controlMode == CM_POS)
          desPos += increment ;
        else
          desVel += increment;
          
        Serial.println("Incrementing " + String(increment));
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
        f64 val = atof(input+3);
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
          desPres = min(max(atof(&input[4])+desPres,0),1);
        } else if(input[2] == 'd') {
          desPres = min(max(desPres-atof(&input[4]),0),1);
        } else {
          desPres = min(max(atof(&input[2]),0),1);
        }
        s32 amount = (s32)(desPres*255.0);

        Serial.println("Setting pressure to " + String(desPres,6) + " pwm " + String(amount));
        analogWrite(pressurePin, amount);
        break;
      }
      case 'a':
      case 'A':
      {
        u8 dir = atoi(&input[2]);
        f64 amnt = atof(&input[4]);

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
      case 't':
      case 'T':
      {
        Serial.println("Setting mode to throttle");
        SetMode(CM_THROTTLE);
        
        bool turn = input[2] == 'y';
        String mm = turn ? "on" : "off";
        SetPop(!turn);
        Serial.println("Turning " + mm);
        break;
      }
      case 'y':
      case 'Y':
      {
        minTension = atof(&input[2]);

        
        Serial.println("Setting min tension to " + String(minTension));
        break;
      }
      case 'z':
      case 'Z':
      {
        zeroPoint = encoder.read();
        Serial.println("Zero point = " + String(zeroPoint));
        currAct = 0;
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
