#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Servo.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

const int enablePin = 9;

// 7-segment shift register pins
const int dataPin  = 8;
const int latchPin = 11;
const int clockPin = 12;

// Servo pin
const int servoPin = 6;
Servo speedServo;

// Joystick pins
const int joyXPin = A0;
const int joyYPin = A1;
const int joyButtonPin = 7;

// Ultrasonic sensor pins
const int trigPin = 5;
const int echoPin = 4;

// 7-segment digit encodings
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

// Fall detection thresholds
const float lowerThreshold = 0.4 * 9.81;
const float upperThreshold = 2.0 * 9.81;

bool droneMode = false;
int lastButtonState = HIGH;

int currentSpeed = 0;                        // ← persistent speed value
const float tiltThreshold = 0.1;            // ← deadzone threshold in g
const int speedStep = 5;                    // ← speed change per tilt

void setup() {
  Serial.begin(9600);

  if (!accel.begin()) {
    Serial.println("No ADXL345 detected.");
    while (1);
  }

  accel.setRange(ADXL345_RANGE_2_G);

  pinMode(enablePin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  pinMode(joyButtonPin, INPUT_PULLUP);

  speedServo.attach(servoPin);

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
  return duration * 0.034 / 2;
}

void loop() {
  int buttonState = digitalRead(joyButtonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {
    droneMode = !droneMode;
    Serial.print("Drone mode: ");
    Serial.println(droneMode ? "ON" : "OFF");
    delay(200);
  }
  lastButtonState = buttonState;

  sensors_event_t event;
  accel.getEvent(&event);

  float ax = event.acceleration.x;
  float ay = event.acceleration.y;
  float az = event.acceleration.z;
  float totalAccel = sqrt(ax*ax + ay*ay + az*az);

  if (totalAccel < lowerThreshold || totalAccel > upperThreshold) {
    analogWrite(enablePin, 0);
    speedServo.write(0);
    displayDigitOn7Segment(0);
    Serial.print("Fall or impact detected!");
    while (true);
  }

  // --------------------- SPEED CONTROL ---------------------
  int speed;
  if (droneMode) {
  int joyVal = analogRead(joyYPin);  // Range: ~0 to 1023
  const int center = 512;
  const int deadZone = 50;

  if (joyVal > center + deadZone) {
    currentSpeed += speedStep;
  } else if (joyVal < center - deadZone) {
    currentSpeed -= speedStep;
  }
  // else → joystick is near center → maintain speed

  currentSpeed = constrain(currentSpeed, 0, 255);
  speed = currentSpeed;
}
  else {
    float x = event.acceleration.x / 9.81;

    if (x > tiltThreshold) {
      currentSpeed += speedStep;
    } else if (x < -tiltThreshold) {
      currentSpeed -= speedStep;
    }

    currentSpeed = constrain(currentSpeed, 0, 255);
    speed = currentSpeed;
  }

  // --------------------- OBSTACLE BRAKING ---------------------
  long distance = getDistanceCM();
  if (distance < 20) {
    int brakeFactor = map(distance, 0, 20, speed, 0);
    speed = constrain(brakeFactor, 0, 255);
    currentSpeed = speed; // update currentSpeed so we resume braking state correctly
  }

  analogWrite(enablePin, speed);

  int displayDigit = map(speed, 0, 255, 0, 9);
  displayDigitOn7Segment(displayDigit);

  int angle = map(speed, 0, 255, 0, 180);
  speedServo.write(angle);

  // --------------------- DEBUG OUTPUT ---------------------
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
