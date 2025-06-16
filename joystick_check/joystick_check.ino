const int joyXPin = A0;      // Joystick X-axis connected to A0
const int joyYPin = A1;      // Joystick Y-axis connected to A1
const int joyButtonPin = 7;  // Joystick button connected to digital pin 7

void setup() {
  Serial.begin(9600);
  pinMode(joyButtonPin, INPUT_PULLUP); // Button active LOW
}

void loop() {
  int joyX = analogRead(joyXPin);
  int joyY = analogRead(joyYPin);
  int buttonState = digitalRead(joyButtonPin);

  Serial.print("X: ");
  Serial.print(joyX);
  Serial.print("  Y: ");
  Serial.print(joyY);
  Serial.print("  Button: ");
  Serial.println(buttonState == LOW ? "Pressed" : "Released");

  delay(200);
}

