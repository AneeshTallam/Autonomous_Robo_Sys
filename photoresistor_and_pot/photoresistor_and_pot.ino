const int ldrPin = A2;          // Photoresistor
const int potPin = A3;          // Potentiometer
const int ledPin = 3;           // LED on PWM pin

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int potValue = analogRead(potPin);
  int brightness;

  if (potValue < 50) {
    // Auto mode: LED brightness controlled by LDR
    int ldrValue = analogRead(ldrPin);
    // Invert brightness: darker = brighter
    brightness = map(ldrValue, 0, 1023, 255, 0);
    brightness = constrain(brightness, 0, 255);

    Serial.print("Photoresistor Mode | Light = ");
    Serial.print(ldrValue);
    Serial.print(" | Brightness = ");
    Serial.println(brightness);
  } else {
    // Manual mode: LED brightness controlled by potentiometer
    brightness = map(potValue, 0, 1023, 0, 255);

    Serial.print("Potentiometer Mode | Pot = ");
    Serial.print(potValue);
    Serial.print(" | Brightness = ");
    Serial.println(brightness);
  }

  analogWrite(ledPin, brightness);
  delay(100);
}
