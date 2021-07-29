// Testing calibration code
#include <Wire.h>
#include <Math.h>
#include <Adafruit_Sensor.h>
#include "CalibratedLIS2MDL.h"

// calibration time in milliseconds
constexpr unsigned long CALIBRATION_TIME = 15000;

CalibratedLIS2MDL mag;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  while (!Serial) { }

  if (!mag.Initialize()) {
    Serial.println("Failed to find IMU");
  }

  unsigned long start = millis();

  Serial.println("Beginning calibration...");
  mag.BeginCalibration(SENSOR_TYPE_MAGNETIC_FIELD);
  delay(10);
  while (millis() - start < CALIBRATION_TIME) {
    mag.AddCalibrationSample();
    delay(10);
  }
  mag.EndCalibration();
  Serial.println("Calibration complete!");
  
  struct AxisOffsets off;
  mag.GetCalibration(&off);
  Serial.print("x: ");
  Serial.println(off.xOff, 5);
  Serial.print("y: ");
  Serial.println(off.yOff, 5);
  Serial.print("z: ");
  Serial.println(off.zOff, 5);
}

void loop() {
  sensors_event_t event;
  mag.GetEvent(&event, SENSOR_TYPE_MAGNETIC_FIELD);
  /*
  Serial.print(event.magnetic.x, 5);
  Serial.print("\t");
  Serial.print(event.magnetic.y, 5);
  Serial.print("\t");
  Serial.print(event.magnetic.z, 5);
  Serial.print("\t");
  Serial.println();
  //*/
  float heading = atan2(event.magnetic.y, event.magnetic.x) * 180 / PI;
  Serial.println(heading);

  delay(100);
}
