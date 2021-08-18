/*!
 * @file CalibratedLSM6DS33.h
 * @author Sebastian S.
 * @brief Declaration for CalibratedLSM6DS33
 */

#ifndef CALIBRATED_LSM6DS33_H_
#define CALIBRATED_LSM6DS33_H_

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM6DS33.h>

#include "SimpleCalibratedSensor.h"

/*!
 * @class CalibratedLSM6DS33
 * @brief Calibrated sensor driver for the LSM6DS33 6-dof IMU
 * 
 * The LSM6DS33 takes accelerometer and gyroscope readings.
 */
class CalibratedLSM6DS33 : public SimpleCalibratedSensor {
 public:  
  CalibratedLSM6DS33();
  
  bool Initialize() override;
  
  // Outputs a sensor event of the given event (ignored if this sensor only outputs one type)
  // Returns true iff the sensor was successfully read
  /*! Allows type to be SENSOR_TYPE_ACCELEROMETER or SENSOR_TYPE_GYROSCOPE */
  bool GetEventRaw(sensors_event_t *event, sensors_type_t type = 0) override;
  
  // Begins calibrating the sensor of the given type
  /*! Allows type to be SENSOR_TYPE_ACCELEROMETER or SENSOR_TYPE_GYROSCOPE */
  void BeginCalibration(sensors_type_t type) override;
  
  // Ends the current calibration
  void EndCalibration() override;
  
  // Adds a calibration sample for the given sensor type (ignored if this sensor only outputs one type)
  void AddCalibrationSample() override;
  
  /*! Allows type to be SENSOR_TYPE_ACCELEROMETER or SENSOR_TYPE_GYROSCOPE */
  void GetCalibration(struct AxisOffsets *offsets, sensors_type_t type = 0) override { *offsets = _gyroOffsets; }
  
  // Clears the currently stored calibration offsets
  /*! Allows type to be SENSOR_TYPE_ACCELEROMETER or SENSOR_TYPE_GYROSCOPE */
  virtual void ClearCalibration(sensors_type_t type = 0) override {
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