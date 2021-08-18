/*!
 * @file SimpleCalibratedSensor.h
 * @author Sebastian S.
 * @brief Declaration for SimpleCalibratedSensor and adjacent types.
 */

#ifndef SIMPLE_CALIBRATED_SENSOR_H_
#define SIMPLE_CALIBRATED_SENSOR_H_

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "CalibrationStorage.h"

/*!
 * @struct AxisLimits
 * A representation of minimum and maximum values read by a three-axis sensor
 */
struct AxisLimits {
  float xMin, xMax;
  float yMin, yMax;
  float zMin, zMax;
};

/*!
 * @class SimpleCalibratedSensor
 * @brief A simple sensor that can be calibrated, store that calibration, and apply it to readings.
 */
class SimpleCalibratedSensor {
 public:
  /*!
   * @brief Default constructor
   */
  SimpleCalibratedSensor() = default;
  
  /*!
   * @brief Initializes the sensor
   * @return True if the sensor was successfully initialized
   *
   * Must be implemented by subclasses
   */
  virtual bool Initialize() = 0;
  
  /*!
   * @brief Takes a reading from the sensor compensated by stored calibration
   * @param event Pointer to a sensors_event_t, to be filled in by compensated data
   * @param type The type of data to read from this sensor, can be ignored by sensors with one type of reading
   * @return True if the sensor successfully returned a reading
   */
  bool GetEvent(sensors_event_t *event, sensors_type_t type = 0);
  
  /*!
   * @brief Takes a raw reading from the sensor
   * @param event Pointer to a sensors_event_t, to be filled in by raw data
   * @param type The type of data to read from this sensor, can be ignored by sensors with one type of reading
   * @return True if the sensor successfully returned a reading
   *
   * Must be implemented by subclasses
   */
  virtual bool GetEventRaw(sensors_event_t *event, sensors_type_t type = 0) = 0;
  
  /*!
   * @brief Begins calibrating the sensor for the given type of reading
   * @param type The type of reading to calibrate
   */
  virtual void BeginCalibration(sensors_type_t type) { }
  
  /*!
   * @brief Stops calibrating the sensor
   *
   * Stops whatever calibration was currently being performed.
   */
  virtual void EndCalibration() { }
  
  /*!
   * @return The type of reading currently being calibrated
   */
  sensors_type_t Calibrating() { return _currCalibration; }
  
  /*!
   * @brief Takes a calibration sample for the reading currently being calibrated.
   *
   * Must be called between BeginCalibration and EndCalibration.
   */
  virtual void AddCalibrationSample() { }
  
  /*!
   * @brief Returns the currently stored offsets of the given reading type.
   * @param offsets Pointer to an AxisOffsets to be filled in
   * @param type The type of reading to receive offsets from
   */
  virtual void GetCalibration(struct AxisOffsets *offsets, sensors_type_t type = 0) { }
  
  /*!
   * @brief Clears the currently stored offsets of the given reading type.
   * @param type The type of reading to receive offsets from
   */
  virtual void ClearCalibration(sensors_type_t type = 0) { }
 protected:
  sensors_type_t _currCalibration;
  
  /*!
   * @brief Compensates the sensor value of the given type pointed to by reading
   * @param reading The sensor reading to compensate with calibration
   * @param type The type of the reading
   */
  virtual void Compensate(sensors_event_t *reading, sensors_type_t type) { }
  
  /*!
   * @brief Fetches the calibration offset data stored in the EEPROM
   */
  virtual void FetchCalibration() { }
  
  /*!
   * @brief Updates the calibration offset data stored in the EEPROM with the current offsets
   */
  virtual void UpdateCalibration() { }
};

#endif
