#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10
#define JOYSTICK_X A1
#define BUTTON_PIN A3

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "1Node";

bool lastButtonState = HIGH;
bool sendControl = false;

struct Payload {
  int direction;       // -1 = left, 0 = stop, +1 = right
  bool controlMode;    // true = manual, false = auto
};

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108);
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  int joystickValue = analogRead(JOYSTICK_X);
  int center = 512;
  int threshold = 50;
  int direction = 0;

  if (joystickValue > center + threshold) direction = 1;
  else if (joystickValue < center - threshold) direction = -1;

  // Button press to toggle mode
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    sendControl = !sendControl;
    Serial.println("Button Pressed: Toggling Mode");
    delay(300);  // Debounce
  }
  lastButtonState = currentButtonState;

  // Send payload
  Payload payload;
  payload.direction = direction;
  payload.controlMode = sendControl;
  radio.write(&payload, sizeof(payload));

  Serial.print("Sent direction: ");
  Serial.print(direction);
  Serial.print(" | Mode: ");
  Serial.println(sendControl ? "Manual" : "Auto");

  delay(100);
}
