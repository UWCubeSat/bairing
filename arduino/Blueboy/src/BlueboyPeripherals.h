#ifndef BLUEBOY_PERIPHERALS_H_
#define BLUEBOY_PERIPHERALS_H_
#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_LIS2MDL.h>
#include <Adafruit_LSM6DS33.h>
#include "Blueboy.h"
#include "sensor/OneUDriver.h"

class BlueboyPeripherals {
 public:
  BlueboyPeripherals();
  
  bool Initialize();

  bool ReadRaw(Device dev, struct AttitudeData *data);
  bool ReadOwnRaw(struct AttitudeData *data);
  bool ReadTestRaw(struct AttitudeData *data);

  bool ReadOrientation(Device dev, struct AttitudeData *data);
  bool ReadOwnOrientation(struct AttitudeData *data);
  bool ReadTestOrientation(struct AttitudeData *data);
  
 private:
  Adafruit_LSM6DS33 _lsm6ds33;
  Adafruit_LIS2MDL _lis2mdl;

  OneUDriver _oneU;             // driver for attached 1U system
  
  bool _initialized;
};

#endif
