#include <Wire.h>
#include <Adafruit_ADXL345.h>

// Create accelerometer object
Adafruit_ADXL345 accel = Adafruit_ADXL345();

void setup() {
  Serial.begin(9600);

  if (!accel.begin()) {
    Serial.println("No ADXL345 detected. Check wiring!");
    while (1);
  }

  // Set range: ±2g, ±4g, ±8g, ±16g
  accel.setRange(ADXL345_RANGE_2_G);

  // Write calibration offsets (values from -127 to +127)
  accel.writeRegister(ADXL345_OFSX, 1);   // Adjust X-axis
  accel.writeRegister(ADXL345_OFSY, -2);  // Adjust Y-axis
  accel.writeRegister(ADXL345_OFSZ, -7);  // Adjust Z-axis

  Serial.println("ADXL345 Initialized with Internal Offset Calibration");
}

void loop() {
  int16_t x, y, z;
  accel.getAcceleration(&x, &y, &z); // Raw accelerations in mG (milli-g = 0.001g)

  // Convert to g
  float fx = x / 1000.0;
  float fy = y / 1000.0;
  float fz = z / 1000.0;

  Serial.print("X = ");
  Serial.print(fx, 3);
  Serial.print(" g   Y = ");
  Serial.print(fy, 3);
  Serial.print(" g   Z = ");
  Serial.print(fz, 3);
  Serial.println(" g");

  delay(250);
}
