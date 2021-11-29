/*!
 * @file BlueboyPeripherals.h
 * @author Sebastian S.
 * @brief Definition for BlueboyPeripherals.
 */

#ifndef BLUEBOY_PERIPHERALS_H_
#define BLUEBOY_PERIPHERALS_H_
#include <Wire.h>

#include <Adafruit_Sensor.h>
#include "Blueboy.h"
#include "sensor/OneUDriver.h"
#include "sensor/CalibratedBNO080.h"
// #include "sensor/CalibratedLSM6DS33.h"
// #include "sensor/CalibratedLIS2MDL.h"

/*!
 * @class BlueboyPeripherals
 * @brief Communicates with peripherals (sensors and test system), configures and receives data from them.
 */
class BlueboyPeripherals {
 public:
  /*!
   * @brief BlueboyPeripherals constructor
   */
  BlueboyPeripherals() : bno080(CalibratedBNO080()),
                                          oneU() , _initialized(false) { }
  
  /*!
   * @brief Initializes sensors and the mounted test system.
   */
  bool Initialize();

  /*!
   * @brief Reads raw data from the given device.
   * @param dev Device to read raw data from
   * @param data Pointer to an AttitudeData struct to be filled with data
   */
  bool ReadRaw(Device dev, struct AttitudeData *data);
  
  /*!
   * @brief Reads raw data from Blueboy sensors.
   * @param data Pointer to an AttitudeData struct to be filled with data
   */
  bool ReadOwnRaw(struct AttitudeData *data);
  
  /*!
   * @brief Reads raw data from the mounted test system.
   * @param data Pointer to an AttitudeData struct to be filled with data
   */
  bool ReadTestRaw(struct AttitudeData *data);

  /*!
   * @brief Reads orientation data from the given device.
   * @param dev Device to read orientation data from
   * @param data Pointer to an AttitudeData struct to be filled with data
   */
  bool ReadOrientation(Device dev, struct AttitudeData *data);
  
  /*!
   * @brief Reads orientation data from Blueboy.
   * @param data Pointer to an AttitudeData struct to be filled with data
   */
  bool ReadOwnOrientation(struct AttitudeData *data);
  
  /*!
   * @brief Reads orientation data from the mounted test system.
   * @param data Pointer to an AttitudeData struct to be filled with data
   */
  bool ReadTestOrientation(struct AttitudeData *data);
  
  /*!
   * @return True if any onboard sensors are currently calibrating.
   */
  bool Calibrating() { return bno080.Calibrating(); }
  
  /*!
   * @var CalibratedLSM6DS33 Internal calibrated LSM6DS33 driver
   */
  // CalibratedLSM6DS33 lsm6ds33;
  
  /*!
   * @var CalibratedLIS2MDL Internal calibrated LIS2MDL driver
   */
  // CalibratedLIS2MDL lis2mdl;
  
  /*!
   * @var CalibratedBNO080 Internal calibrated BNO080 driver
   */
  CalibratedBNO080 bno080;

  /*!
   * @var OneUDriver Internal driver for the mounted 1U test system
   */
  OneUDriver oneU;
 private:
  bool _initialized;
};

#endif
