// Driver for the 1U test system over I2C

#ifndef ONEU_DRIVER_H_
#define ONEU_DRIVER_H_

#include <Arduino.h>
#include <Adafruit_Sensor.h>

#include "SimpleCalibratedSensor.h"
#include "../Blueboy.h"

class OneUDriver : public SimpleCalibratedSensor {
 public:  
  OneUDriver();
  
  bool Initialize() override;
  
  // Outputs a sensor event of the given event (ignored if this sensor only outputs one type)
  // Returns true iff the sensor was successfully read
  bool GetEventRaw(sensors_event_t *event, sensors_type_t type = 0) override;
  
  // Begins calibrating the sensor of the given type
  void BeginCalibration(sensors_type_t type) override;
  
  // Ends the current calibration
  void EndCalibration() override;
};

#endif
