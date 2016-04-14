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

// Program Scope Variables
Encoder encoder(encoderPinA, encoderPinB);
f64 countPerRev = 1024;
f64 lastPos = 0; 
f64 lastVel = 0; 
f64 objectNaturalPos = 0; 
f64 objectPos = 0; 
f64 objectLastPos = 0; 
f64 objectVel = 0; 
f64 objectLastVel = 0; 
char virtualEnvCase; 
int counter = 0; 

// --------------------------
// Initialize
// --------------------------
void setup() 
{
  // Set Up Serial
  Serial.begin(9600);
  
  // Set PWM frequency 
  setPwmFrequency(pwmAPin,1); //PWM on pin 3 is (32500/1) = 32500 [Hz] 
  
  // Output Pins
  pinMode(pwmAPin, OUTPUT);  // PWM for A
  pinMode(dirAPin, OUTPUT);  // dir for A

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
f64 errorLast = 0;
u8 dirLast = 0;
u32 lastTime = 0;
f64 des = 0;
u32 count = 0;
f64 integralError = 0;

f64 Kp = 0.08;
f64 Kd = 0.5;
f64 Ki = 0.00005;
void loop() 
{
  f64 pos = encoder.read();

  f64 error = des-pos;
  f64 derror = error-errorLast;
  integralError += error;
    
  errorLast = error;

  u32 currTime = millis();
  f64 dt = (f64)(currTime-lastTime);
  derror /= dt;
  lastTime = currTime;
    
  f64 u = Kp*error + Kd*derror + Ki*integralError;

  char dir = 1;
  if(u < 0) {
    dir = 0;
    u = abs(u);
  }
  u = min(u, 255);
  if(dir != dirLast) {
    digitalWrite(dirAPin, dir);
    dirLast = dir;
  }
  analogWrite(pwmAPin, u);
  if(count++ % 2000 == 0)
    Serial.println("Pos " + String(pos) + " Error " + String(error) + " u " + String(u) + " dt " + String(dt) + " derror " + String(derror) + " integralError " + String(integralError));

  s32 input = 0;
  while(Serial.available()) {
    input = Serial.read();
    if(input == 'p') {
      des += 2048;
      Serial.println("Incrementing desired value to " + String(des));
    } else if(input == 'n') {
      des -= 2048;
      Serial.println("Decrementing desired value to " + String(des));
    }
  }
}

// --------------------------
// Haptic Loop 
// --------------------------
void hapticLoop()
{
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
