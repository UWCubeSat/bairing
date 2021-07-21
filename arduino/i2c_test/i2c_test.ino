#include <Adafruit_ISM330DHCX.h>
#include <Adafruit_LSM6DS33.h>
#include <Adafruit_LIS2MDL.h>


#include<Wire.h>

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  Wire.begin();
}

void loop() {
}
