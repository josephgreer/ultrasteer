/*
 Name:		PressureRegulator.ino
 Created:	6/30/16 4:42:40 PM
 Author:	Joey
*/

#include "Common.h"
#include <Servo.h>

s32 pressureInPin = A0;
s32 pressureOutPin = 3;
s32 servoPin = 45;

Servo linearServo;
#define AMOUNT_TO_SECONDS(amnt)((s32)(amnt * 1000.0 + 1000.0))

// the setup function runs once when you press reset or power the board
void setup() {
  linearServo.attach(servoPin);

  pinMode(pressureInPin, INPUT);
  pinMode(pressureOutPin, OUTPUT);

  analogWrite(pressureOutPin, 0);

  Serial.begin(57600);
  Serial.setTimeout(5);
  linearServo.writeMicroseconds(AMOUNT_TO_SECONDS(1.0));
}

// the loop function runs over and over again until power down or reset
s32 count = 0;
void loop() {
  if (++count % 100000 == 0) {
    s32 pressure = analogRead(pressureInPin);
    Serial.println("Pressure in " + String((f64)pressure / 1023.0));
    count = 0;
  }
}

u8 buffer[100] = { 0 };
f64 g_amnt = 1.0f;
f64 g_step = 0.05;
void serialEvent() {
  if (Serial.available()) {
    Serial.readBytes(buffer, 100);
    switch (buffer[0]) {
    case 'b':
    case 'B':
    {
      g_amnt = g_amnt + g_step;
      g_amnt = CLAMP(g_amnt, 0, 1);
      Serial.println("Amount = " + String(g_amnt));
      linearServo.writeMicroseconds(AMOUNT_TO_SECONDS(g_amnt));
      break;
    }
    case 'n':
    case 'N':
    {
      g_amnt = g_amnt - g_step;
      g_amnt = CLAMP(g_amnt, 0, 1);
      Serial.println("Amount = " + String(g_amnt));
      linearServo.writeMicroseconds(AMOUNT_TO_SECONDS(g_amnt));
      break;
    }
    case 'P':
    case 'p':
    {
      f64 amnt = atof((s8 *)&buffer[2]);
      Serial.println("Setting pressure to " + String(amnt));
      analogWrite(pressureOutPin, (s32)(255.0*amnt));
      break;
    }
    case 'S':
    case 's':
    {
      f64 amnt = atof((s8 *)&buffer[2]);
      amnt = CLAMP(amnt, 0, 1);
      g_amnt = amnt;
      Serial.println("Setting servo pos to " + String(amnt));
      linearServo.writeMicroseconds(AMOUNT_TO_SECONDS(amnt));
      break;
    }
    }
  }
  memset(buffer, 0, sizeof(buffer[0]) * 100);
}
