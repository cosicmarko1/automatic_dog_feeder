const int dirPin = 18;
const int stepPin = 19;

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void loop() {
  digitalWrite(dirPin, HIGH);

  // moving stepper motor in four steps
  // each step makes 90 degrees rotation
  for(int x = 0; x < 50; x++){
    digitalWrite(stepPin, HIGH);                                      
    delayMicroseconds(2000); 
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000); 
  }
  // delay 2 seconds before making next step
  delay(2000);

  digitalWrite(dirPin, LOW);
}
