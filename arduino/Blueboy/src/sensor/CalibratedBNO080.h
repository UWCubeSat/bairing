/*!
 * @file CalibratedBNO080.h
 * @author Sebastian S.
 * @brief Declaration for the BNO080 driver
 */

#ifndef CALIBRATED_BNO080_H_
#define CALIBRATED_BNO080_H_

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <SparkFun_BNO080_Arduino_Library.h>

#include "SimpleCalibratedSensor.h"
#include "../Blueboy.h"

/*!
 * @class CalibratedBNO080
 * @brief Calibrated sensor driver for the BNO080 9-dof IMU
 * 
 * The BNO080 takes magnetometer, accelerometer, and gyroscope readings,
 * and automatically fuses them into an orientation.
 */
class CalibratedBNO080 : public SimpleCalibratedSensor {
 public:
  CalibratedBNO080();
  
  bool Initialize() override;
  
  /*!
   * @brief Updates readings from the BNO080, blocking until receiving the latest data.
   * @return True if a reading was successfully taken.
   */   
  bool CalibratedBNO080::UpdateReadings();
  
  // Outputs a sensor event of the given event (ignored if this sensor only outputs one type)
  // pre: UpdateReadings() was previously called
  // Returns true iff the sensor was successfully read
  /*! Allows type to be:
   *  SENSOR_TYPE_MAGNETOMETER
   *  SENSOR_TYPE_ACCELEROMETER
   *  SENSOR_TYPE_GYROSCOPE
   */
  bool GetEventRaw(sensors_event_t *event, sensors_type_t type = 0) override;
  
  /*!
   * @brief Takes a reading of the 1U test system's orientation as a quaternion
   * @param eulers Pointer to a Quaternion to fill
   * @return True if the system successfully returned a reading
   */
  bool GetOrientationQuaternion(struct Quaternion *quaternion);
  
    /*!
   * @brief Takes a reading of the BNO080's orientation in euler angles
   * @param eulers Pointer to a Vector to fill with angles
   * @return True if a reading was successfuly returned
   */
  bool GetOrientationEulers(struct Vector *eulers);
  
  // Begins calibrating the sensor of the given type
  /*! Allows type to be:
   *  SENSOR_TYPE_MAGNETOMETER
   *  SENSOR_TYPE_ACCELEROMETER
   *  SENSOR_TYPE_GYROSCOPE
   */
  void BeginCalibration(sensors_type_t type) override;
  
  // Ends the current calibration
  void EndCalibration() override;
  
  /*! Allows type to be:
   *  SENSOR_TYPE_MAGNETOMETER
   *  SENSOR_TYPE_ACCELEROMETER
   *  SENSOR_TYPE_GYROSCOPE
   */
  void GetCalibration(struct AxisOffsets *offsets, sensors_type_t type = 0) override;
  
  // Clears the currently stored calibration offsets
  /*! Allows type to be:
   *  SENSOR_TYPE_MAGNETOMETER
   *  SENSOR_TYPE_ACCELEROMETER
   *  SENSOR_TYPE_GYROSCOPE
   */
  virtual void ClearCalibration(sensors_type_t type = 0) override;
  
  // Requests a software reset of the sensor
  void Reset();
 private:
  BNO080    _bno080;      // internal BNO080 driver
};

#endif