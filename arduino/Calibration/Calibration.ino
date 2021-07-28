// Testing calibration code
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "CalibratedLSM6DS33.h"

// calibration time in milliseconds
constexpr unsigned long CALIBRATION_TIME = 5000;

CalibratedLSM6DS33 imu;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  while (!Serial) { }

  if (!imu.Initialize()) {
    Serial.println("Failed to find IMU");
  }

  unsigned long start = millis();

  Serial.println("Beginning calibration...");
  imu.BeginCalibration(SENSOR_TYPE_GYROSCOPE);
  delay(10);
  while (millis() - start < CALIBRATION_TIME) {
    imu.AddCalibrationSample();
  }
  imu.EndCalibration();
  Serial.println("Calibration complete!");
  
  struct AxisOffsets off;
  imu.GetCalibration(&off);
  Serial.print("x: ");
  Serial.println(off.xOff, 5);
  Serial.print("y: ");
  Serial.println(off.yOff, 5);
  Serial.print("z: ");
  Serial.println(off.zOff, 5);
}

void loop() {
  ///*
  sensors_event_t event;
  imu.GetEvent(&event, SENSOR_TYPE_GYROSCOPE);
  Serial.print(event.gyro.x, 5);
  Serial.print("\t");
  Serial.print(event.gyro.y, 5);
  Serial.print("\t");
  Serial.print(event.gyro.z, 5);
  Serial.print("\t");
  Serial.println();
  //*/

  delay(100);
}
