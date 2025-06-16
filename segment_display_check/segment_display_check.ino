// Shift register pins
const int dataPin  = 8;   // DS
const int latchPin = 11;  // STCP
const int clockPin = 12;  // SHCP

// Segment encodings for digits 0-9 (common cathode)
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

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

void loop() {
  for (int digit = 0; digit <= 9; digit++) {
    displayDigit(digit);
    delay(1000); // Wait 1 second before showing the next number
  }
}

// Sends a digit to the 7-segment display
void displayDigit(int digit) {
  if (digit < 0 || digit > 9) return;

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digitSegments[digit]);
  digitalWrite(latchPin, HIGH);
}
