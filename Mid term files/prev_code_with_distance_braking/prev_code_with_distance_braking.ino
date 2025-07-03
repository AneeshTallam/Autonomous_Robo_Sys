#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Servo.h>
#include <Stepper.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

const int enablePin = 3;

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

// Stepper motor (via H-bridge)
const int stepsPerRevolution = 2048;
Stepper myStepper(stepsPerRevolution, 13, 9, 10, 2);
const int joyCenter = 512;
const int joyDeadzone = 50;

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
const float lowerThreshold = 0.6 * 9.81;
const float upperThreshold = 1.5 * 9.81;

bool droneMode = false;
int lastButtonState = HIGH;

int currentSpeed = 0;
const float tiltThreshold = 0.1;
const int speedStep = 5;

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

  myStepper.setSpeed(60);  // RPM
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
    speedServo.write(180);
    displayDigitOn7Segment(0);
    Serial.print("Fall or impact detected!");
    while (true);
  }

  // --------------------- SPEED CONTROL ---------------------
  int speed;
  if (droneMode) {
    int joyVal = analogRead(joyYPin);
    const int center = 512;
    const int deadZone = 50;

    if (joyVal > center + deadZone) {
      currentSpeed += speedStep;
    } else if (joyVal < center - deadZone) {
      currentSpeed -= speedStep;
    }
    currentSpeed = constrain(currentSpeed, 0, 255);
    speed = currentSpeed;
  } else {
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
    // Target speed linearly scales with distance
    int targetSpeed = map(distance, 0, 20, 0, currentSpeed);

    // Smooth braking: step-wise approach toward target
    if (currentSpeed > targetSpeed) {
      int brakingRate = map(distance, 0, 20, 20, 2);  // stronger braking when closer
      currentSpeed -= min(brakingRate, currentSpeed - targetSpeed);
      currentSpeed = constrain(currentSpeed, 0, 255);
    }
    speed = currentSpeed;
  }



  analogWrite(enablePin, speed);

  int displayDigit = map(speed, 0, 255, 0, 9);
  displayDigitOn7Segment(displayDigit);

  int angle = map(speed, 0, 255, 180, 0);
  speedServo.write(angle);

  // --------------------- STEPPER STEERING ---------------------

  int joyX = analogRead(joyXPin);  // Read horizontal joystick axis
  if(droneMode){
   if (joyX > joyCenter + joyDeadzone) {
    // Joystick pushed to the right → turn right
    myStepper.setSpeed(10);
    myStepper.step(10);  // Positive steps = clockwise
  }
  else if (joyX < joyCenter - joyDeadzone) {
    // Joystick pushed to the left → turn left
    myStepper.setSpeed(10);
    myStepper.step(-10); // Negative steps = counter-clockwise
  }
  }

  // --------------------- DEBUG OUTPUT ---------------------
Serial.println(
  String("Drone: ") + (droneMode ? "ON" : "OFF") +
  " Speed: " + speed +
  " Digit: " + displayDigit +
  " Servo: " + angle +
  " Accel: " + totalAccel +
  " Dist: " + distance
  
);


  delay(100);
}

void displayDigitOn7Segment(int digit) {
  digit = constrain(digit, 0, 9);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digitSegments[digit]);
  digitalWrite(latchPin, HIGH);
}
