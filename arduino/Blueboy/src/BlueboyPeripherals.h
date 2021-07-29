#ifndef BLUEBOY_PERIPHERALS_H_
#define BLUEBOY_PERIPHERALS_H_
#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_LIS2MDL.h>
#include "Blueboy.h"
#include "sensor/OneUDriver.h"
#include "sensor/CalibratedLSM6DS33.h"
#include "sensor/CalibratedLIS2MDL.h"

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
  
  CalibratedLSM6DS33 lsm6ds33;
  CalibratedLIS2MDL lis2mdl;
  
 private:

  OneUDriver _oneU;             // driver for attached 1U system
  
  bool _initialized;
};

#endif
