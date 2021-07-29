#ifndef SIMPLE_CALIBRATED_SENSOR_H_
#define SIMPLE_CALIBRATED_SENSOR_H_

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "CalibrationStorage.h"

struct AxisLimits {
  float xMin, xMax;
  float yMin, yMax;
  float zMin, zMax;
};

class SimpleCalibratedSensor {
 public:
  SimpleCalibratedSensor() = default;
  
  virtual bool Initialize();
  
  // Outputs a sensor event of the given event (ignored if this sensor only outputs one type), with values compensated for by previous calibration
  // Returns true iff the sensor was successfully read
  bool GetEvent(sensors_event_t *event, sensors_type_t type = 0);
  
  // Outputs a sensor event of the given event (ignored if this sensor only outputs one type)
  // Returns true iff the sensor was successfully read
  virtual bool GetEventRaw(sensors_event_t *event, sensors_type_t type = 0) = 0;
  
  // Begins calibrating the sensor of the given type
  virtual void BeginCalibration(sensors_type_t type) = 0;
  
  // Ends the current calibration
  virtual void EndCalibration() = 0;
  
  // Returns the sensor type currently being calibrated
  sensors_type_t Calibrating() { return _currCalibration; }
  
  // Reads a calibration sample and adds it to the stored data for the sensor type currently being calibrated.
  // Must be called between a call to BeginCalibration and EndCalibration
  virtual void AddCalibrationSample() = 0;
  
  virtual void GetCalibration(struct AxisOffsets *offsets) = 0;
  
  // Clears the currently stored calibration offsets
  virtual void ClearCalibration() = 0;
 protected:
  sensors_type_t _currCalibration;
  
  // Compensates the sensor value of the given type pointed to by reading
  virtual void Compensate(sensors_event_t *reading, sensors_type_t type) = 0;
  
  // Fetches the calibration offset data stored in the EEPROM
  virtual void FetchCalibration() = 0;
  
  // Updates the calibration offset data stored in the EEPROM with the current offsets
  virtual void UpdateCalibration() = 0;
};

#endif
