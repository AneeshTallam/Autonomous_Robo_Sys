#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

const int enablePin = 9; // PWM to L293D Enable pin

int currentSpeed = 0;  // Tracks speed across loops
const float tiltThreshold = 0.1; // g-units, to ignore small noise
const int speedStep = 5; // How much to increment/decrement per loop

void setup() {
  Serial.begin(9600);

  if (!accel.begin()) {
    Serial.println("No ADXL345 detected.");
    while (1);
  }

  accel.setRange(ADXL345_RANGE_2_G); // ±2g
  pinMode(enablePin, OUTPUT);
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  float x = event.acceleration.x / 9.81; // Convert to g-units

  // Forward tilt = increase speed
  if (x > tiltThreshold) {
    currentSpeed += speedStep;
  }
  // Backward tilt = decrease speed
  else if (x < -tiltThreshold) {
    currentSpeed -= speedStep;
  }
  // Otherwise (horizontal), do nothing (hold speed)

  currentSpeed = constrain(currentSpeed, 0, 255); // Clamp to valid PWM range

  analogWrite(enablePin, currentSpeed);

  // Debug info
  Serial.print("X Tilt (g): ");
  Serial.print(x, 2);
  Serial.print(" | Speed: ");
  Serial.println(currentSpeed);

  delay(100); // Adjust for smoothness
}
