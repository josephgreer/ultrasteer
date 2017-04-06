int pressurePin = 3;

void setup() {
  // Pressure Pin
  pinMode(pressurePin, OUTPUT);
  digitalWrite(pressurePin, LOW);

  Serial.begin(57600);
}


double t0 = 0;
double freq = 0.1;
double amplitude = 0.5;
#define PI 3.14159265359
void loop() {
  if(t0 == 0) {
    t0 = ((double)micros())*1e-6;
  }

  double t = ((double)micros())*1e-6;

  double pressure = amplitude*((sin(2*PI*freq*(t-t0))+1)*0.5);
  Serial.println(pressure);

  analogWrite(pressurePin, (int)(pressure*255.0+0.5));
}
