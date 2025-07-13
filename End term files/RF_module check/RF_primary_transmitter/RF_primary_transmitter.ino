#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define JOY_X A1
#define JOY_BTN A2

RF24 radio(9, 10);  // CE, CSN

const byte address[6] = "00001";

struct ControlData {
  int angle;
  bool override;
};

void setup() {
  Serial.begin(9600);
  pinMode(JOY_BTN, INPUT_PULLUP);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  static bool lastBtnState = HIGH;
  static bool overrideMode = false;
  static unsigned long lastDebounce = 0;

  bool btnState = digitalRead(JOY_BTN);

  if (btnState != lastBtnState && millis() - lastDebounce > 200) {
    lastDebounce = millis();
    if (btnState == LOW) {
      overrideMode = !overrideMode;  // Toggle mode on button press
    }
  }
  lastBtnState = btnState;

  int joyVal = analogRead(JOY_X);
  int angle = map(joyVal, 0, 1023, 0, 180);

  ControlData data = { angle, overrideMode };
  radio.write(&data, sizeof(data));

  delay(100);
}
