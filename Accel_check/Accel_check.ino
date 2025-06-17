#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// Create an ADXL345 object
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void setup() {
  Serial.begin(9600);
  if (!accel.begin()) {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1);
  }

  // Set range (optional): ADXL345_RANGE_2_G, _4_G, _8_G, _16_G
  accel.setRange(ADXL345_RANGE_2_G);

  // Calibration (offsets) — optional
 // Z-axis offset

  Serial.println("ADXL345 Initialized");
  delay(100);
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  // Convert m/s² to g (1g ≈ 9.81 m/s²)
  float x = event.acceleration.x / 9.81;
  float y = event.acceleration.y / 9.81;
  float z = event.acceleration.z / 9.81;

  Serial.print("Xa = ");
  Serial.print(x, 3); // 3 decimal places
  Serial.print(" g   Ya = ");
  Serial.print(y, 3);
  Serial.print(" g   Za = ");
  Serial.println(z, 3);
  Serial.print(" g");

  delay(250);
}
