int pressurePin = 3;

void setup() {
  Serial.begin(57600);
  pinMode(pressurePin, OUTPUT);

}

double freqMin = 0.05;
double freqMax = 2;
double freqStep = 0.03;
double currFreq = 0;
double t0 = 0;
double delayTime = 3;
double activeTime = 20;
double amplitude = 0.85;
int firstCount = 0;

bool begin = false;

void loop() {
  if(Serial.available()) {
    Serial.println("Begin");
    Serial.read();
    begin = true;
  }
  if(!begin)
    return;
  if(t0 == 0)
    t0 = micros()*1e-6;

  if(currFreq > freqMax)
    return;

  double t = micros()*1e-6;
  if(t-t0 > activeTime && currFreq == 0 && firstCount < 2) {
    t0 = 0;
    analogWrite(pressurePin, 0);
    delay((int)(delayTime*1e3));
    Serial.println("Staying on " + String(currFreq,6) + "Hz");
    firstCount++;
    if(firstCount == 2)
      currFreq = freqMin;
  } else if(t-t0 > activeTime) {
    t0 = 0;
    analogWrite(pressurePin, 0);
    delay((int)(delayTime*1e3));
    currFreq = currFreq+freqStep;
    Serial.println("Moving onto " + String(currFreq,6) + "Hz");
    return;
  }

  double amnt = 0.5*amplitude*(cos(2*PI*currFreq*(t-t0))+1);
  analogWrite(pressurePin, (int)(amnt*255.0));
}
