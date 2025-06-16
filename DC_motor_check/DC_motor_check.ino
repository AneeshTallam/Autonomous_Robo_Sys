const int motorPin = 9; // Connect this to motor + terminal

void setup() {
  pinMode(motorPin, OUTPUT);
}

void loop() {
  // Ramp motor speed from 0 to 255
  for (int speed = 0; speed <= 255; speed++) {
    analogWrite(motorPin, speed);
    delay(10);
  }

  delay(1000); // Full speed hold

  // Ramp motor speed back down
  for (int speed = 255; speed >= 0; speed--) {
    analogWrite(motorPin, speed);
    delay(10);
  }

  delay(1000); // Motor stopped
}
