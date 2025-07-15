#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";

Servo myServo;

struct ControlData {
  int angle;
  bool override;
};

void setup() {
  Serial.begin(9600);
  myServo.attach(3);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  static bool overrideMode = false;
  static int angle = 0;
  static int direction = 1;

  ControlData data;

  if (radio.available()) {
    radio.read(&data, sizeof(data));
    overrideMode = data.override;

    if (overrideMode) {
      myServo.write(data.angle);
    }
  }

  if (!overrideMode) {
    myServo.write(angle);
    angle += direction;
    if (angle >= 180 || angle <= 0) {
      direction = -direction;
    }
    delay(15);
  }

  delay(10);
}
