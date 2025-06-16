#include <Servo.h>

Servo myServo;        // Create a servo object
const int servoPin = 6; // Connect servo signal wire to pin 6

void setup() {
  myServo.attach(servoPin); // Attach the servo to the pin
}

void loop() {
  // Sweep from 0 to 180 degrees
  for (int angle = 0; angle <= 180; angle++) {
    myServo.write(angle);
    delay(10); // Wait for servo to reach position
  }

  delay(500); // Hold at 180 degrees

  // Sweep back from 180 to 0 degrees
  for (int angle = 180; angle >= 0; angle--) {
    myServo.write(angle);
    delay(10);
  }

  delay(500); // Hold at 0 degrees
}
