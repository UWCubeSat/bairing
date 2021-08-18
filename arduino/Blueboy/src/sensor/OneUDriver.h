/*!
 * @file OneUDriver.h
 * @author Sebastian S.
 * @brief Declaration for OneUDriver
 */

#ifndef ONEU_DRIVER_H_
#define ONEU_DRIVER_H_

#include <Arduino.h>
#include <Adafruit_Sensor.h>

#include "SimpleCalibratedSensor.h"
#include "../Blueboy.h"

/*!
 * @class OneUDriver
 * @brief Driver for the mounted 1U test system
 */
class OneUDriver : public SimpleCalibratedSensor {
 public:
  OneUDriver();
  
  bool Initialize() override;
  
  // Outputs a sensor event of the given event (ignored if this sensor only outputs one type)
  // Returns true iff the sensor was successfully read
  bool GetEventRaw(sensors_event_t *event, sensors_type_t type = 0) override;
  
  /*!
   * @brief Requests a reset of the 1U test system
   */
  void Reset();
  
  /*!
   * @brief Checks the health of a sensor aboard the test system
   * @param type The type of sensor to check the health of
   * @return True if the given sensor is healthy
   */
  bool OneUDriver::SensorHealthy(sensors_type_t type);
  
  /*!
   * @brief Takes a reading of the 1U test system's orientation in euler angles
   * @param eulers Pointer to a Vector to fill with angles
   * @return True if the system successfully returned a reading
   */
  bool GetOrientationEulers(struct Vector *eulers);
  
  /*!
   * @brief Takes a reading of the 1U test system's orientation as a quaternion
   * @param eulers Pointer to a Quaternion to fill
   * @return True if the system successfully returned a reading
   */
  bool GetOrientationQuaternion(struct Quaternion *quaternion);
  
  // Begins calibrating the sensor of the given type
  void BeginCalibration(sensors_type_t type) override;
  
  // Ends the current calibration
  void EndCalibration() override;
};

#endif
