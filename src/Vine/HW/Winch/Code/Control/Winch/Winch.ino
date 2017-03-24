// Includes 

typedef double f64;
typedef char s8;
typedef unsigned char u8;
typedef int s32;
typedef unsigned long u32;

#include <TimerOne.h>
#include <Encoder.h>
#include <math.h>

// Pin Declares
int pwmPins[2] = { 5,6 };
int dirPins[2] = { 8,7 };
int encoderPinAs[2] = { 3,2 };
int encoderPinBs[2] = { 11,10 };
int pressurePin = 9;

s32 ledPins[2] = { 13,12 };
bool ledPinsOn[2] = { false,false };

// Program Scope Variables
Encoder encoders[2] = { Encoder(encoderPinAs[0], encoderPinBs[0]), Encoder(encoderPinAs[1],encoderPinBs[1]) };
f64 countPerRev = 1024;
int counter = 0;

// --------------------------
// Initialize
// --------------------------
void setup()
{
  // Set Up Serial
  Serial.begin(57600);
  Serial.setTimeout(15);

  for (s32 i = 0; i < 2; i++) {
    // Set PWM frequency 
    setPwmFrequency(pwmPins[i], 1); //PWM on pin 3 is (32500/1) = 32500 [Hz] 

                                 // Output Pins
    pinMode(pwmPins[i], OUTPUT);  // PWM for A
    pinMode(dirPins[i], OUTPUT);  // dir for A

    pinMode(encoderPinAs[i], INPUT);
    pinMode(encoderPinBs[i], INPUT);
  }

  pinMode(pressurePin, OUTPUT);

  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);

  for (s32 i = 0; i < 2; i++) {
    // Init Motor 
    analogWrite(pwmPins[i], 0);     // set to not be spinning (0/255)
    digitalWrite(dirPins[i], LOW);  // set direction 

                                 // Init Timer and Set Haptic Loop 
                                 //Timer1.initialize(); 
                                 //long period = 10000; // 10000 [us] -> 100 Hz sample time 
                                 //Timer1.attachInterrupt(hapticLoop,period); 
  }
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

f64 errorLasts[2] = { 0,0 };
u8 dirLasts[2] = { 0,0 };
u32 lastTime = 0;
f64 desPoss[2] = { 0,0 };
f64 desVels[2] = { 0,0 };
f64 desPres[2] = { 0,0 };
u32 count = 0;
f64 integralErrors[2] = { 0,0 };
f64 lastPoss[2] = { 0,0 };
f64 lastVels[2] = { 0,0 };

// Position Constants
f64 Kpp = 0.08;
f64 Kdp = 0.008;
f64 Kip = 0.00005;

// Velocity Constants
f64 Kpv = 1;
f64 Kdv = 0.1;
f64 Kiv = 0.01;

f64 minTension = 0;

CONTROL_MODE controlModes[2] = { CM_POS,CM_POS };

u8 unwindDir = 1;

f64 lowPassDesVels[2] = { 0,0 };
f64 alpha = 0.995;
f64 alphaPos = 0.2;

void loop()
{
  u32 currTime = millis();
  f64 poss[2] = { 0 };
  f64 vels[2] = { 0 };
  f64 dt = (f64)(currTime - lastTime) / (1000.0);
  for (s32 i = 0; i < 2; i++) {
    poss[i] = encoders[i].read();
    f64 u;

    if (currTime == lastTime) {
      Serial.println("WTF");
      return;
    }

    f64 error, derror;

    lowPassDesVels[i] = lowPassDesVels[i]*alpha + (1 - alpha)*desVels[i];

    vels[i] = alphaPos*(poss[i] - lastPoss[i]) / dt + (1 - alphaPos)*lastVels[i];
    if (controlModes[i] == CM_POS) {
      error = desPoss[i] - poss[i];
    }
    else {
      error = lowPassDesVels[i] - vels[i];
      lastVels[i] = vels[i];
    }
    derror = error - errorLasts[i];
    integralErrors[i] += error;

    errorLasts[i] = error;
    derror /= dt;
    lastPoss[i] = poss[i];

    s8 dir = 0;

    switch (controlModes[i]) {
    case CM_POS:
    {
      u = Kpp*error + -Kdp*vels[i] + Kip*integralErrors[i];

      if (u < 0) {
        dir = 1;
        u = abs(u);
      }
      u = min(u, 255);
      break;
    }
    case CM_VEL:
    {
      u = Kpv*error - Kdv*vels[i] + Kiv*integralErrors[i];

      if (u < 0) {
        dir = 1;
        u = abs(u);
      }
      u = min(u, 255);
      break;
    }
    case CM_THROTTLE:
    {
      u = Kpv*error + Kdv*derror + Kiv*integralErrors[i];

      if (u < minTension) {
        u = minTension;
        integralErrors[i] = 0;
        derror = 0;
        dir = 1 - unwindDir;
      }
      u = min(u, 255);
      break;
    }
    }

    if (controlModes[i] != CM_NONE) {
      if (dir != dirLasts[i]) {
        digitalWrite(dirPins[i], dir);
        dirLasts[i] = dir;
      }
      //    if(count % 499 == 0)
      //      Serial.println("act " + String(currAct+1));
      //    if(count++ % 2000 == 0) 
      //      Serial.println("Des Pos " +  String(desPos) + " Pos " + String(pos) + " DesVel " + String(desVel) + " Vel " + String(vel) + " Pressure " + String(desPres) + " Error " + String(error) + " u " + String(u) + " dt " + String(dt*1000.0) + " derror " + String(derror) + " integralError " + String(integralError));

      analogWrite(pwmPins[i], u);
    }
  }
  if (count++ % 10 == 0)
    Serial.println("E " + String(poss[0], 5) + ", " + String(vels[0]) + ", " + String(poss[1],5) + ", " + String(vels[1],5) + ";");
  lastTime = currTime;

}

void SetVel(f64 vel,u8 motor)
{
  Serial.println("Set vel " + String(vel, 6));
  if (abs(vel) < abs(desVels[motor]))
    lowPassDesVels[motor] = vel;
  desVels[motor] = vel;
}

u32 zeroPoint = 0;

f64 unwindSign = -1;
f64 popVel = -500;
f64 nonPopVel = -500;
f64 popPressure = 1;
f64 nonPopPressure = 0.55;
f64 firstPressure = 0.55;

u8 defaultPop = true;

void SetMode(u8 mode, u8 motor)
{
  controlModes[motor] = (CONTROL_MODE)mode;
  lowPassDesVels[motor] = 0;
  desVels[motor] = integralErrors[motor] = 0;
  errorLasts[motor] = 0;
  desPoss[motor] = lastPoss[motor];
  integralErrors[motor] = 0;
  lastVels[motor] = 0;
}

#define BUFFER_LEN 20
s8 input[BUFFER_LEN] = { 0 };
s32 nBytes = 0;

void serialEvent()
{
  if (Serial.available()) {
    nBytes = Serial.readBytes(input, BUFFER_LEN);

    u8 motor = atoi(&input[2]);
    switch (input[0]) {
    case 'm':   // switch mode 
    {
      String mode;
      CONTROL_MODE mm;
      if (input[4] == 'v') {               //velocity
        mm = CM_VEL;
        mode = "velocity";
      }
      else if (input[4] == 'p') {        //position
        mm = CM_POS;
        mode = "position";
      }
      else if (input[4] == 't') {        //throttle
        mm = CM_THROTTLE;
        mode = "throttle";
      }

      SetMode(mm, motor);

      Serial.println("Control mode switched to " + mode);
      break;
    }
    case 'q':  // decrement set point. interpretation depends on mode
    {
      u8 motor = atoi(input + 2);
      f64 decrement = atof(input + 4);
      if (controlModes[motor] == CM_POS)
        desPoss[motor] -= decrement;
      else
        desVels[motor] -= decrement;

      Serial.println("Decrementing " + String(decrement));
      break;
    }
    case 'w':   // increment set point. interpretation depends on mode
    {
      u8 motor = atoi(input + 2);
      f64 increment = atof(input + 4);
      if (controlModes[motor] == CM_POS)
        desPoss[motor] += increment;
      else
        desVels[motor] += increment;

      Serial.println("Incrementing " + String(increment));
      break;
    }
    case 'b':
    case 'B':
    {
      s32 num = atoi(input + 2) - 1;
      digitalWrite(ledPins[num], ledPinsOn[num] ? LOW : HIGH);
      ledPinsOn[num] = !ledPinsOn[num];
      break;
    }
    case 'K':   //set Kp/Kd/Ki
    case 'k':
    {
      f64 *Kp, *Kd, *Ki;
      if (controlModes[motor] == CM_VEL) {
        Kp = &Kpv;
        Kd = &Kdv;
        Ki = &Kiv;
      }
      else {
        Kp = &Kpp;
        Kd = &Kdp;
        Ki = &Kip;
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
    case 'a':
    case 'A':
    {
      u8 motor = atoi(&input[2]);
      u8 dir = atoi(&input[4]);
      f64 amnt = atof(&input[6]);

      s32 amount = (s32)(amnt*255.0);

      Serial.println("Setting motor to " + String(amnt, 6) + " pwm " + String(amount) + " direction " + String(dir));
      digitalWrite(dirPins[motor], dir > 0 ? HIGH : LOW);
      analogWrite(pwmPins[motor], amount);
      controlModes[motor] = CM_NONE;
      break;
    }
    case 'p':   //Pause
    case 'P':
    {
      u8 motor = atoi(&input[2]);
      lowPassDesVels[motor] = desVels[motor] = integralErrors[motor] = 0;
      errorLasts[motor] = 0;
      desPoss[motor] = lastPoss[motor];
      integralErrors[motor] = 0;
      lastVels[motor] = 0;

      controlModes[motor] = CM_NONE;

      analogWrite(pwmPins[motor], 0);

      Serial.println("Pause");
      break;
    }
    case 's':   //Pause
    case 'S':
    {
      u8 motor = atoi(&input[2]);
      lowPassDesVels[motor] = desVels[motor] = integralErrors[motor] = 0;
      errorLasts[motor] = 0;
      desPoss[motor] = lastPoss[motor];
      integralErrors[motor] = 0;
      lastVels[motor] = 0;

      if(controlModes[motor] == CM_NONE)
        controlModes[motor] = CM_POS;

      Serial.println("Stop");
      break;
    }
    case 'y':
    case 'Y':
    {
      minTension = atof(&input[2]);


      Serial.println("Setting min tension to " + String(minTension));
      break;
    }
    default:
      break;
    }
    memset(input, 0, BUFFER_LEN);
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
    }
    else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  }
  else if (pin == 3 || pin == 11) {
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
