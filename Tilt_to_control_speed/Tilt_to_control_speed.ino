#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

const int motorPin = 9; // PWM pin connected to motor

void setup() {
  Serial.begin(9600);

  if (!accel.begin()) {
    Serial.println("No ADXL345 detected.");
    while (1);
  }

  accel.setRange(ADXL345_RANGE_2_G); // ±2g range
  pinMode(motorPin, OUTPUT);
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  // Read X-axis in g
  float x = event.acceleration.x / 9.81;

  // Clamp to safe range
  x = constrain(x, -1.0, 1.0);

  // Map tilt to motor speed
  // -1g → 0 speed, 0g → 127, +1g → 255
  int speed = map(x * 100, -100, 100, 0, 255);

  analogWrite(motorPin, speed);

  // Debug output
  Serial.print("Tilt X = ");
  Serial.print(x, 2);
  Serial.print(" g   -> Speed = ");
  Serial.println(speed);

  delay(100);
}
