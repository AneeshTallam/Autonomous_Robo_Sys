#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

const int enablePin = 9; // Connect this to L293D Pin 1 (Enable 1)

void setup() {
  Serial.begin(9600);

  if (!accel.begin()) {
    Serial.println("No ADXL345 detected.");
    while (1);
  }

  accel.setRange(ADXL345_RANGE_2_G); // ±2g range
  pinMode(enablePin, OUTPUT);

  // Direction is fixed in hardware: IN1 = 5V, IN2 = GND
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  // Read X-axis acceleration in g
  float x = event.acceleration.x / 9.81;

  // Clamp to safe range
  x = constrain(x, -1.0, 1.0);

  // Map tilt to motor speed
  // -1g → 0 speed, 0g → 127, +1g → 255
  int speed = map(x * 100, -100, 100, 0, 255);
  speed = constrain(speed, 0, 255); // Just in case

  analogWrite(enablePin, speed); // PWM to control speed via H-bridge

  // Debug output
  Serial.print("Tilt X = ");
  Serial.print(x, 2);
  Serial.print(" g   -> Speed = ");
  Serial.println(speed);

  delay(100);
}
