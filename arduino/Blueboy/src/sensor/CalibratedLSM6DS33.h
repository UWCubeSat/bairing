#ifndef CALIBRATED_LSM6DS33_H_
#define CALIBRATED_LSM6DS33_H_

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM6DS33.h>

#include "SimpleCalibratedSensor.h"

class CalibratedLSM6DS33 : public SimpleCalibratedSensor {
 public:  
  CalibratedLSM6DS33();
  
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
  
  void GetCalibration(struct AxisOffsets *offsets) override { *offsets = _gyroOffsets; }
 private:
  Adafruit_LSM6DS33   _lsm6ds33;      // internal lsm6ds33 driver
  struct AxisLimits   _gyroLimits;    // gyroscope limits
  struct AxisOffsets  _gyroOffsets;   // gyroscope offsets
  int _gyroToDiscard;                 // number of samples to discard
  
  // Compensates the sensor value of the given type pointed to by reading
  void Compensate(sensors_event_t *reading, sensors_type_t) override;
};

#endif