#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <RTClib.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Set time only if RTC lost power or time is invalid (optional)
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Initialize Accelerometer
  if (!accel.begin()) {
    Serial.println("No ADXL345 detected");
    while (1);
  }

  Serial.println("Setup complete!");
}

void loop() {
  // Get time
  DateTime now = rtc.now();
  Serial.print("Time: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());

  // Get accelerometer data
  sensors_event_t event;
  accel.getEvent(&event);
  Serial.print("X: "); Serial.print(event.acceleration.x);
  Serial.print(" m/s^2, Y: "); Serial.print(event.acceleration.y);
  Serial.print(" m/s^2, Z: "); Serial.println(event.acceleration.z);

  Serial.println("------------------------");
  delay(1000);
}
