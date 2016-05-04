#include <Servo.h>

Servo myservo;
void setup() {
  Serial.begin(57600);
  myservo.attach(9);
  myservo.write(45);
}

float angle = 0;
#define BUFFER_LEN 50
char input[BUFFER_LEN];
void loop() {
  int nBytes;
  if (Serial.available()) {
    nBytes = Serial.readBytes(input, BUFFER_LEN);
    angle = atof(input);
    Serial.println("Setting angle " + String(angle));
    myservo.write(angle);
    memset(input, 0, nBytes);
  }
}
