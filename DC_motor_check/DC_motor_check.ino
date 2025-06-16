const int enablePin = 9; // PWM to L293D Enable pin 1

void setup() {
  pinMode(enablePin, OUTPUT);
}

void loop() {
  // Speed up
  for (int speed = 0; speed <= 255; speed += 5) {
    analogWrite(enablePin, speed);
    delay(30); // Adjust delay for ramping speed
  }

  delay(500); // Hold at full speed

  // Slow down
  for (int speed = 255; speed >= 0; speed -= 5) {
    analogWrite(enablePin, speed);
    delay(30);
  }

  delay(500); // Pause before repeating
}