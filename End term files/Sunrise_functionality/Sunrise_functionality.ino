// Shift register pins
const int dataPin  = 8;
const int clockPin = 7;
const int latchPin = 6;

const int ldrPin = A0;  // LDR connected to analog pin

// Sun rising animation frames (row-wise: top to bottom, columns active LOW)
const byte sunFrames[5][8] = {
  { // Frame 0 - Sun just rising
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00111100,
    0b00011000,
    0b00000000
  },
  { // Frame 1 - Sun higher
    0b00000000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00111100,
    0b00011000,
    0b00000000,
    0b00000000
  },
  { // Frame 2 - Sun mid
    0b00000000,
    0b00000000,
    0b00011000,
    0b00111100,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00000000
  },
  { // Frame 3 - Sun near top
    0b00000000,
    0b00011000,
    0b00111100,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  },
  { // Frame 4 - Sun at top
    0b00111100,
    0b01111110,
    0b00111100,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  }
};

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  Serial.begin(9600);
}

// Function to display one 8x8 frame
void displayFrame(const byte frame[8], int durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    for (int row = 0; row < 8; row++) {
      byte rowData = 1 << row;             // active HIGH for row
      byte colData = ~frame[row];          // invert for active LOW columns
      shiftOutDual(rowData, colData);
      delay(1);  // small delay for persistence
    }
  }
}

void shiftOutDual(byte rowData, byte colData) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, rowData);  // SR2: rows
  shiftOut(dataPin, clockPin, MSBFIRST, colData);  // SR1: columns
  digitalWrite(latchPin, HIGH);
}

void loop() {
  int lightLevel = analogRead(ldrPin);
  Serial.print("LDR Value: ");
  Serial.println(lightLevel);

  if (lightLevel > 500) {
    for (int i = 0; i < 5; i++) {
      displayFrame(sunFrames[i], 300);  // show each frame for 300ms
    }
  } else {
    shiftOutDual(0x00, 0xFF); // Clear display
  }

  delay(100);
}
