#include "src/sensors/CalibratedBNO080.h"

CalibratedBNO080 bno;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  while (!Serial) { }

  if (!bno.Initialize()) {
    Serial.println("Failed to find IMU");
  }
}

void loop() {
  sensors_event_t event;
  /*
  mag.GetEvent(&event, SENSOR_TYPE_MAGNETIC_FIELD);
  
  float heading = atan2(event.magnetic.y, event.magnetic.x) * 180 / PI;
  Serial.println(heading);

  delay(100);
  */
}
