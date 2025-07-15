#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <IRremote.hpp>

#define BUTTON_PIN A3
#define JOYSTICK_X A1
#define LDR_PIN A0
#define DATA_PIN 8
#define CLOCK_PIN 7
#define LATCH_PIN 6
#define SERVO_VALVE_PIN 5
#define IR_RECEIVE_PIN 3
#define LED1_PIN 2
#define LED2_PIN 4

RF24 radio(9, 10); // CE, CSN
const byte addressA[6] = "1Node";
const byte addressB[6] = "2Node";

Servo valveServo;

const byte sunFrames[5][8] = {
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00011000,0b00111100,0b00011000,0b00000000},
  {0b00000000,0b00000000,0b00000000,0b00011000,0b00111100,0b00011000,0b00000000,0b00000000},
  {0b00000000,0b00000000,0b00011000,0b00111100,0b00011000,0b00000000,0b00000000,0b00000000},
  {0b00000000,0b00011000,0b00111100,0b00011000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00111100,0b01111110,0b00111100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000}
};

struct PayloadToB {
  int direction;
  bool controlMode;
  bool lightDetected;
  int motorSpeedLevel;
  bool stopAlarm;
};

struct PayloadFromB {
  bool objectNear;
  bool lowWater;
};

bool lastButtonState = HIGH;
bool sendControl = false;
int motorSpeedLevel = 0;
int lastSentSpeed = -1;
bool stopAlarm = false;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  valveServo.attach(SERVO_VALVE_PIN);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108);
  radio.openWritingPipe(addressA);
  radio.openReadingPipe(1, addressB);
  radio.startListening();

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  int joystickValue = analogRead(JOYSTICK_X);
  int direction = 0;
  if (joystickValue > 562) direction = 1;
  else if (joystickValue < 462) direction = -1;

  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    sendControl = !sendControl;
    Serial.println("Button Toggled Mode");
    delay(300);
  }
  lastButtonState = currentButtonState;

  if (IrReceiver.decode()) {
    uint32_t command = IrReceiver.decodedIRData.command;
    Serial.print("IR command: 0x");
    Serial.println(command, HEX);
    if (command == 0x47) {
      stopAlarm = true;
      Serial.println("Stop alarm command received");
    }
    switch (command) {
      case 0x16: motorSpeedLevel = 0; break;
      case 0xC:  motorSpeedLevel = 1; break;
      case 0x18: motorSpeedLevel = 2; break;
      case 0x5E: motorSpeedLevel = 3; break;
      default: break;
    }
    IrReceiver.resume();
  }

  int ldrValue = analogRead(LDR_PIN);
  bool lightDetected = (ldrValue > 500);

  radio.stopListening();
  PayloadToB payload = {direction, sendControl, lightDetected, motorSpeedLevel, stopAlarm};
  radio.write(&payload, sizeof(payload));
  radio.startListening();

  if (motorSpeedLevel != lastSentSpeed) {
    Serial.print("Sending speed level: ");
    Serial.println(motorSpeedLevel);
    lastSentSpeed = motorSpeedLevel;
  }

  if (radio.available()) {
    PayloadFromB received;
    radio.read(&received, sizeof(received));

    digitalWrite(LED1_PIN, received.objectNear);
    digitalWrite(LED2_PIN, received.objectNear);
    valveServo.write(received.lowWater ? 90 : 0);
  }

  Serial.print("LDR: "); Serial.print(ldrValue);
  Serial.print(" | Light: "); Serial.print(lightDetected ? "YES" : "NO");
  Serial.print(" | StopAlarm: "); Serial.println(stopAlarm ? "YES" : "NO");

  if (lightDetected) {
    for (int i = 0; i < 5; i++) displayFrame(sunFrames[i], 200);
  } else {
    shiftOutDual(0x00, 0x00);
  }

  delay(100);
}

void displayFrame(const byte frame[8], int durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    for (int row = 0; row < 8; row++) {
      byte rowData = 1 << row;
      byte colData = ~frame[row];
      shiftOutDual(rowData, colData);
      delay(1);
    }
  }
}

void shiftOutDual(byte rowData, byte colData) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, rowData);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, colData);
  digitalWrite(LATCH_PIN, HIGH);
}
