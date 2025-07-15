#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

// For sending the email
int startMinute = -1;
bool emailSent = false;

// Accelerometer Setup
int ADXL345 = 0x53; // I2C address
float X_out, Y_out, Z_out;  // Outputs
float baseX = 0, baseY = 0, baseZ = 0;
bool baselineSet = false;

void setup() {
  Serial.begin(9600);
  delay(3000);  // Let serial monitor connect

  Wire.begin();

  // --- RTC Initialization ---
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, setting time now...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Sync RTC with compile time
  }

  // --- ADXL345 Initialization ---
  Wire.beginTransmission(ADXL345);
  Wire.write(0x2D); // Power control register
  Wire.write(8);    // Measurement mode
  Wire.endTransmission();
  delay(10);

  // Optional: Offset Calibration
  Wire.beginTransmission(ADXL345); Wire.write(0x1E); Wire.write(1); Wire.endTransmission(); delay(10); // X offset
  Wire.beginTransmission(ADXL345); Wire.write(0x1F); Wire.write(-2); Wire.endTransmission(); delay(10); // Y offset
  Wire.beginTransmission(ADXL345); Wire.write(0x20); Wire.write(-7); Wire.endTransmission(); delay(10); // Z offset
}

void loop() {
  // --- RTC Reading ---
  DateTime now = rtc.now();
  int currentMinute = now.minute();

  if (startMinute == -1) {
    startMinute = currentMinute;
  }

  if (!emailSent && currentMinute == (startMinute + 2) % 60) {
    Serial.println("SEND_EMAIL");
    emailSent = true;
  }

  Serial.print("Time: ");
  Serial.print(now.year()); Serial.print('/');
  Serial.print(now.month()); Serial.print('/');
  Serial.print(now.day()); Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]); Serial.print(") ");
  Serial.print(now.hour()); Serial.print(':');
  Serial.print(now.minute()); Serial.print(':');
  Serial.print(now.second()); Serial.println();

  // --- ADXL345 Reading ---
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start at X-axis data register
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);

  if (Wire.available() == 6) {
    X_out = (int16_t)(Wire.read() | Wire.read() << 8) / 256.0;
    Y_out = (int16_t)(Wire.read() | Wire.read() << 8) / 256.0;
    Z_out = (int16_t)(Wire.read() | Wire.read() << 8) / 256.0;

    Serial.print("Accel -> X: "); Serial.print(X_out);
    Serial.print("  Y: "); Serial.print(Y_out);
    Serial.print("  Z: "); Serial.println(Z_out);

    if (!baselineSet) {
      baseX = X_out;
      baseY = Y_out;
      baseZ = Z_out;
      baselineSet = true;
      Serial.println("Baseline set.");
    }

    float deltaX = abs(X_out - baseX);
    float deltaY = abs(Y_out - baseY);
    float deltaZ = abs(Z_out - baseZ);

    if (deltaX > 0.5 || deltaY > 0.5 || deltaZ > 0.5) {
      Serial.println("🚨 Door opened!");
    }
  } else {
    Serial.println("ADXL345 read error!");
  }

  Serial.println("--------------------------------------------------");
  delay(1000);
}
