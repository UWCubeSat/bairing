#include "src/sensors/CalibratedBNO080.h"
#include "src/Blueboy.h"

CalibratedBNO080 bno;

unsigned long last;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  while (!Serial) { }

  if (!bno.Initialize()) {
    Serial.println("Failed to find IMU");
  }
  Wire.setClock(400000); //Increase I2C data rate to 400kHz

  last = millis();
}

struct Vector q;

void loop() {
  unsigned long now = millis();
  if (now - last >= 200) {
    bno.GetOrientationEulers(&q);
    Serial.print(F("Eulers: "));
    Serial.print(q.x, 2);
    Serial.print(F(", "));
    Serial.print(q.y, 2);
    Serial.print(F(", "));
    Serial.print(q.z, 2);
    Serial.println();

    last = now;
  }
}
