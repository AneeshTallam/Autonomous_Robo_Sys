// Pin definitions
const int dataPin  = 8;  // DS
const int clockPin = 7;  // SH_CP
const int latchPin = 6;  // ST_CP

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
}

// Sends data to both shift registers
void shiftOutDual(byte rowData, byte colData) {
  digitalWrite(latchPin, LOW);
  
  // Send row first (2nd SR), then column (1st SR)
  shiftOut(dataPin, clockPin, MSBFIRST, rowData);  // SR2
  shiftOut(dataPin, clockPin, MSBFIRST, colData);  // SR1

  digitalWrite(latchPin, HIGH);
}

void loop() {
  // Sweep one dot left to right, top to bottom
  for (int row = 0; row < 8; row++) {
    byte rowByte = 1 << row;         // turn on one row
    for (int col = 0; col < 8; col++) {
      byte colByte = ~(1 << col);    // turn on one column (active LOW)
      shiftOutDual(rowByte, colByte);
      delay(100);
    }
  }
}
