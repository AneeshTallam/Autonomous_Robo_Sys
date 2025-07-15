#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// RF24 Setup
#define CE_PIN 9
#define CSN_PIN 10
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "LDR01";

// Shift register pins
const int dataPin  = 8;
const int clockPin = 7;
const int latchPin = 6;

const int ldrPin = A0;  // LDR connected to analog pin

// Sun rising animation frames (row-wise: top to bottom, columns active LOW)
const byte sunFrames[5][8] = {
  {
    0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00011000, 0b00111100, 0b00011000, 0b00000000
  },
  {
    0b00000000, 0b00000000, 0b00000000, 0b00011000,
    0b00111100, 0b00011000, 0b00000000, 0b00000000
  },
  {
    0b00000000, 0b00000000, 0b00011000, 0b00111100,
    0b00011000, 0b00000000, 0b00000000, 0b00000000
  },
  {
    0b00000000, 0b00011000, 0b00111100, 0b00011000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000
  },
  {
    0b00111100, 0b01111110, 0b00111100, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000
  }
};

void setup() {
  Serial.begin(9600);

  // Shift register pins
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  // RF24 setup
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108);
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  int ldrValue = analogRead(ldrPin);
  bool lightDetected = (ldrValue > 500);  // Threshold

  // Send LDR status via RF
  radio.write(&lightDetected, sizeof(lightDetected));

  Serial.print("LDR Value: ");
  Serial.print(ldrValue);
  Serial.print(" | Light: ");
  Serial.println(lightDetected ? "YES" : "NO");

  if (lightDetected) {
    // Sun rising animation
    for (int i = 0; i < 5; i++) {
      displayFrame(sunFrames[i], 300);  // 300ms per frame
    }
  } else {
    shiftOutDual(0x00, 0xFF); // Clear display
  }

  delay(200);  // Delay to reduce transmission rate
}

// Display 1 animation frame
void displayFrame(const byte frame[8], int durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    for (int row = 0; row < 8; row++) {
      byte rowData = 1 << row;             // Active HIGH
      byte colData = ~frame[row];          // Active LOW
      shiftOutDual(rowData, colData);
      delay(1);
    }
  }
}

// Shift out to dual 74HC595s
void shiftOutDual(byte rowData, byte colData) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, rowData);  // Rows
  shiftOut(dataPin, clockPin, MSBFIRST, colData);  // Columns
  digitalWrite(latchPin, HIGH);
}
