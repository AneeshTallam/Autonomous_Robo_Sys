#include <IRremote.hpp>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// === IR Remote Setup ===
#define IR_RECEIVE_PIN 3
#define KEY_SECURITY_ON  0x08  // Remote Key 4
#define KEY_SECURITY_OFF 0x1C  // Remote Key 5

// === Accelerometer Setup ===
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
bool securityMode = false;
bool alarmTriggered = false;

// === Dot Matrix pins ===
const int dataPin  = 8;  // DS
const int clockPin = 7;  // SH_CP
const int latchPin = 6;  // ST_CP

void setup() {
  Serial.begin(9600);

  // IR remote setup
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR ready. Press key 4 (ON) or key 5 (OFF)");

  // Accelerometer setup
  if (!accel.begin()) {
    Serial.println("❌ ADXL345 not detected. Check connections.");
    while (1);
  }
  accel.setRange(ADXL345_RANGE_2_G);
  Serial.println("ADXL345 initialized.\n");

  // Dot matrix pins
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  clearDisplay();
}

void loop() {
  // IR input
  if (IrReceiver.decode()) {
    uint8_t cmd = IrReceiver.decodedIRData.command;

    if (cmd == KEY_SECURITY_ON) {
      securityMode = true;
      alarmTriggered = false;
      Serial.println("🔐 Security Mode ENABLED");
    } else if (cmd == KEY_SECURITY_OFF) {
      securityMode = false;
      alarmTriggered = false;
      Serial.println("🔓 Security Mode DISABLED");
    }

    IrReceiver.resume();
  }

  // Read accelerometer
  sensors_event_t event;
  accel.getEvent(&event);
  int16_t x = (int16_t)(event.acceleration.x * 256);
  int16_t y = (int16_t)(event.acceleration.y * 256);
  int16_t z = (int16_t)(event.acceleration.z * 256);

  // Display values
  Serial.print("X: "); Serial.print(x);
  Serial.print("   Y: "); Serial.print(y);
  Serial.print("   Z: "); Serial.print(z);
  Serial.print("   Mode: ");
  Serial.println(securityMode ? "SECURED" : "UNSECURED");

  if (securityMode) {
    if (x > 1000 || x < -1000 || y > 1000 || y < -1000) {
      if (!alarmTriggered) {
        Serial.println("🚨 ALARM TRIGGERED: X or Y acceleration exceeded limit!\n");
        alarmTriggered = true;
      }
    } else {
      alarmTriggered = false;
    }
  } else {
    alarmTriggered = false;
  }

  if (alarmTriggered) {
    showXOnDisplay();
  } else {
    clearDisplay();
  }

  delay(50);
}

// Sends data to both shift registers
void shiftOutDual(byte rowData, byte colData) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, rowData);  // Rows
  shiftOut(dataPin, clockPin, MSBFIRST, colData);  // Columns
  digitalWrite(latchPin, HIGH);
}

// Improved animated X pattern
void showXOnDisplay() {
  for (int i = 0; i < 8; i++) {
    byte rowByte = 1 << i;
    byte colByte = ~((1 << i) | (1 << (7 - i)));  // Active LOW

    shiftOutDual(rowByte, colByte);
    delay(5);

    clearDisplay();
    delay(5);
  }
}

// Turn off all LEDs
void clearDisplay() {
  shiftOutDual(0x00, 0xFF);  // All rows and columns off
}
