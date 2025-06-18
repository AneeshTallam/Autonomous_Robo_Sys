#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Servo.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

const int enablePin = 9; // PWM to L293D Enable pin

// 7-segment shift register pins
const int dataPin  = 8;
const int latchPin = 11;
const int clockPin = 12;

// Servo pin
const int servoPin = 6;
Servo speedServo; // Servo object

// Joystick pins
const int joyXPin = A0;       // Not used here but wired for completeness
const int joyYPin = A1;       // Y-axis for speed control in drone mode
const int joyButtonPin = 7;   // Joystick button pin

// Ultrasonic sensor pins
const int trigPin = 5;
const int echoPin = 4;

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

// Fall detection thresholds (in m/s^2)
const float freeFallThreshold = 0.2 * 9.81; // ~0.2g
const float impactThreshold = 3.0 * 9.81;   // ~3g

// Drone mode flag and button state tracking
bool droneMode = false;
int lastButtonState = HIGH;

void setup() {
  Serial.begin(9600);

  if (!accel.begin()) {
    Serial.println("No ADXL345 detected.");
    while (1);
  }

  accel.setRange(ADXL345_RANGE_2_G); // ±2g

  pinMode(enablePin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  pinMode(joyButtonPin, INPUT_PULLUP);

  speedServo.attach(servoPin); // Attach servo to pin 6

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

long getDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;  // in cm
  return distance;
}

void loop() {
  // Handle joystick button toggle for drone mode
  int buttonState = digitalRead(joyButtonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {
    droneMode = !droneMode;
    Serial.print("Drone mode: ");
    Serial.println(droneMode ? "ON" : "OFF");
    delay(200); // simple debounce delay
  }
  lastButtonState = buttonState;

  // Read accelerometer
  sensors_event_t event;
  accel.getEvent(&event);

  // Calculate total acceleration magnitude for fall detection
  float ax = event.acceleration.x;
  float ay = event.acceleration.y;
  float az = event.acceleration.z;
  float totalAccel = sqrt(ax*ax + ay*ay + az*az);

  if (totalAccel < freeFallThreshold || totalAccel > impactThreshold) {
    // Emergency stop on fall or impact
    analogWrite(enablePin, 0);
    speedServo.write(0);
    displayDigitOn7Segment(0);

    Serial.print("Fall or impact detected! Total Accel: ");
    Serial.println(totalAccel);

    delay(100);
    return;     // Skip rest of loop
  }

  int speed;

  if (droneMode) {
    int joyVal = analogRead(joyYPin);
    speed = map(joyVal, 0, 1023, 0, 255);
  } else {
    float x = event.acceleration.x / 9.81;
    x = constrain(x, -1.0, 1.0);
    speed = map(x * 100, -100, 100, 0, 255);
  }
  speed = constrain(speed, 0, 255);

  // Obstacle detection and braking
  long distance = getDistanceCM();
  if (distance < 20) {
    int brakeFactor = map(distance, 0, 20, speed, 0);
    speed = constrain(brakeFactor, 0, 255);
  }

  analogWrite(enablePin, speed); // Control motor speed

  int displayDigit = map(speed, 0, 255, 0, 9);
  displayDigitOn7Segment(displayDigit);

  int angle = map(speed, 0, 255, 0, 180);
  speedServo.write(angle);

  // Debug output
  Serial.print("Drone mode: ");
  Serial.print(droneMode ? "ON" : "OFF");
  Serial.print("  Speed = ");
  Serial.print(speed);
  Serial.print("  Digit = ");
  Serial.print(displayDigit);
  Serial.print("  Servo Angle = ");
  Serial.print(angle);
  Serial.print("  Total Accel = ");
  Serial.print(totalAccel);
  Serial.print("  Distance = ");
  Serial.println(distance);

  delay(100);
}

void displayDigitOn7Segment(int digit) {
  digit = constrain(digit, 0, 9);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digitSegments[digit]);
  digitalWrite(latchPin, HIGH);
}
