#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// Accelerometer setup
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// H-bridge Enable pin for motor (L293D Pin 1)
const int enablePin = 9;

// Shift register pins for 7-segment display
const int dataPin  = 8;
const int latchPin = 11;
const int clockPin = 12;

// 7-segment digit encodings (common cathode)
const byte digitSegments[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

void setup() {
  Serial.begin(9600);

  if (!accel.begin()) {
    Serial.println("No ADXL345 detected.");
    while (1);
  }

  accel.setRange(ADXL345_RANGE_2_G); // ±2g
  pinMode(enablePin, OUTPUT);        // L293D Enable pin (PWM)

  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  float x = event.acceleration.x / 9.81;
  x = constrain(x, -1.0, 1.0);

  // Convert tilt to motor speed (0–255)
  int speed = map(x * 100, -100, 100, 0, 255);
  speed = constrain(speed, 0, 255);

  analogWrite(enablePin, speed); // PWM to control motor speed via H-bridge

  // Map speed to a single digit (0–9)
  int displayDigit = map(speed, 0, 255, 0, 9);

  // Debug info
  Serial.print("Tilt X = ");
  Serial.print(x, 2);
  Serial.print(" g   -> Speed = ");
  Serial.print(speed);
  Serial.print("  -> Display Digit = ");
  Serial.println(displayDigit);

  // Show digit on 7-segment display
  displayDigitOn7Segment(displayDigit);

  delay(100);
}

void displayDigitOn7Segment(int digit) {
  digit = constrain(digit, 0, 9);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digitSegments[digit]);
  digitalWrite(latchPin, HIGH);
}
