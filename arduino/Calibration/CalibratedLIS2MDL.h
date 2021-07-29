#ifndef CALIBRATED_LIS2MDL_H_
#define CALIBRATED_LIS2MDL_H_

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LIS2MDL.h>

#include "SimpleCalibratedSensor.h"

class CalibratedLIS2MDL : public SimpleCalibratedSensor {
 public:  
  CalibratedLIS2MDL();
  
  bool Initialize() override;
  
  // Outputs a sensor event of the given event (ignored if this sensor only outputs one type), with values compensated for by previous calibration
  // Returns true iff the sensor was successfully read
  bool GetEvent(sensors_event_t *event, sensors_type_t type = 0) override;
  
  // Outputs a sensor event of the given event (ignored if this sensor only outputs one type)
  // Returns true iff the sensor was successfully read
  bool GetEventRaw(sensors_event_t *event, sensors_type_t type = 0) override;
  
  // Begins calibrating the sensor of the given type
  void BeginCalibration(sensors_type_t type) override;
  
  // Ends the current calibration
  void EndCalibration() override;
  
  // Adds a calibration sample for the given sensor type (ignored if this sensor only outputs one type)
  void AddCalibrationSample() override;

  void GetCalibration(struct AxisOffsets *offsets) override { *offsets = _magOffsets; }
 private:
  Adafruit_LIS2MDL   _lis2mdl;        // internal LIS2MDL driver
  struct AxisLimits   _magLimits;     // magnetometer limits
  struct AxisOffsets  _magOffsets;    // magnetometer offsets
  int _magToDiscard;                  // number of samples to discard
  
  // Compensates the sensor value of the given type pointed to by reading
  void Compensate(sensors_event_t *reading, sensors_type_t) override;
};

#endif
