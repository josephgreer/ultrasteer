#include "/Users/Joey/ultrasteer/src/Vine/HW/Winch/Code/Control/Common.h"
#include "/Users/Joey/ultrasteer/src/Vine/HW/Winch/Code/Control/JacobianControl.h"

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
f64 currPressures[N_TURN_ACT] = {0,0,0};


s32 mainChamberPressurePin = A8;

int ledPin = 2;

ActuatorMapper *mapper[N_TURN_ACT];

s32 regulatorPins[N_TURN_ACT] = { 9, 10, 11 };

#define ACTUATE_REGULATOR(r,amnt) (analogWrite(regulatorPins[r], mapper[r]->MapVal(amnt)))

void controlPressure(s32 i, f64 dt)
{
  f64 kTerm, dTerm, iTerm;
  f64 error = 0;
  f64 currPressure = 0;
  f64 derror = 0;
  f64 u = 0;
  f64 actuatorAmount = 0;

  currPressure = analogRead(pressureSensorPins[i]) / 1024.0;
  currPressures[i] = currPressure;
  error = pressureSetPoints[i] - currPressure;
  pressureLowPassError[i] = error*0.1 + pressureLowPassError[i] * (1 - 0.1);
  derror = (pressureLowPassError[i] - pressureErrorLast[i]) / dt;

  kTerm = error*pressurePs[i];
  iTerm = pressureIntegralError[i] * pressureIs[i];
  dTerm = derror*pressureDs[i];

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

  for (s32 i = 0; i < N_TURN_ACT-2; i++) {
    controlPressure(i, dt);
  }

  f64 mainChamberPress = analogRead(mainChamberPressurePin) / 1024.0;

  Serial.println(String(millis()) + " " + String(currPressures[0], 6) + " " + String(mainChamberPress,6));
}


f64 actuatorPressure = 0;
f64 mainChamberPressure = 0.4;
f64 actuatorStepPressure = 0.025;
f64 maxActuatorPressure = 0.15;
u32 activeTime = 7500;
u32 delayTime = 3000;

bool begin = false;

void setup() {
  Serial.begin(57600);
  
  mapper[0] = (ActuatorMapper *)new DeadbandMapper(103.0 / 255.0, 200);
  mapper[1] = (ActuatorMapper *)new DeadbandMapper(103.0 / 255.0, 255);
  mapper[2] = (ActuatorMapper *)new DeadbandMapper(103.0 / 255.0, 255);

  for(s32 i=0; i<N_TURN_ACT; i++) {
    pinMode(pressureSensorPins[i], INPUT);
    pinMode(regulatorPins[i], OUTPUT);
  }

  pinMode(mainChamberPressurePin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

}

bool ledOn = false;

u32 t0 = 0;
u32 t;

void loop() {
  if(Serial.available()) {
    Serial.println("Begin");
    Serial.read();
    begin = true;
  }
  if(!begin)
    return;

  if(actuatorPressure > maxActuatorPressure) {
    digitalWrite(ledPin, LOW);
    return;
  }

  pressureSetPoints[0] = actuatorPressure;

  if(t0 == 0)
    t0 = millis();

  t = millis();

  digitalWrite(ledPin, HIGH);

  analogWrite(3, (s32)(mainChamberPressure*255.0+0.5));

  if(t-t0 > activeTime) {
    t0 = 0;
    digitalWrite(ledPin, LOW);
    delay(delayTime);
    actuatorPressure = actuatorPressure+actuatorStepPressure;
    Serial.println("Moving onto prssure " + String(actuatorPressure,6));
    return;
  }
    
  controlPressures();
}