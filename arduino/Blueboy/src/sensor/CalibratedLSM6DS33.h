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
  
  // Clears the currently stored calibration offsets
  virtual void ClearCalibration() override {    
    CalibrationStorage::Clear(_handle);
    _gyroOffsets.xOff = _gyroOffsets.yOff = _gyroOffsets.zOff = 0.0;
  }
 private:
  Adafruit_LSM6DS33   _lsm6ds33;      // internal lsm6ds33 driver
  struct AxisLimits   _gyroLimits;    // gyroscope limits
  struct AxisOffsets  _gyroOffsets;   // gyroscope offsets
  int _gyroToDiscard;                 // number of samples to discard
  StorageHandle _handle;              // EEPROM handle
  
  // Compensates the sensor value of the given type pointed to by reading
  void Compensate(sensors_event_t *reading, sensors_type_t) override;
  
  // Fetches the calibration offset data stored in the EEPROM
  void FetchCalibration() override { CalibrationStorage::Fetch(_handle, &_gyroOffsets); }
  
  // Updates the calibration offset data stored in the EEPROM with the current offsets
  void UpdateCalibration() override { CalibrationStorage::Update(_handle, &_gyroOffsets); }
};

#endif