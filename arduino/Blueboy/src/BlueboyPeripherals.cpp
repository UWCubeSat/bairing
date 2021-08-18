/*!
 * @file BlueboyPeripherals.cpp
 * @author Sebastian S.
 * @brief Implementation of BlueboyPeripherals.h
 */

#include "BlueboyPeripherals.h"

/*!
 * @var uint8_t ONEU_ADDRESS
 * I2C address of the 1U test system.
 */
constexpr uint8_t ONEU_ADDRESS = 0x3A;

bool BlueboyPeripherals::Initialize() {
  if (_initialized) {
    return true;
  }

  CalibrationStorage::Initialize();
  
  if (!lis2mdl.Initialize()) {
    Serial.println(F("Failed to find LIS2MDL"));
    return false;
  }
  
  if (!lsm6ds33.Initialize()) {
    Serial.println(F("Failed to find LSM6DS33"));
    return false;
  }

  if (!oneU.Initialize()) {
    Serial.println(F("Failed to find test system"));
    return false;
  }

  _initialized = true;
  return true;
}

bool BlueboyPeripherals::ReadRaw(Device dev, struct AttitudeData *data) {
  switch (dev) {
    case Device::Own:
      return ReadOwnRaw(data);
    case Device::Test:
      return ReadTestRaw(data);
    default:
      return false;
  }
}

bool BlueboyPeripherals::ReadOwnRaw(struct AttitudeData *data) {
  sensors_event_t event;
  
  // Adafruit's unified sensor vector has to be converted to our vectors
  if (!lis2mdl.GetEvent(&event)) {
    return false;
  }
  data->raw.magnetic = *(struct Vector *)&event.magnetic;
  
  if (!lsm6ds33.GetEventRaw(&event, SENSOR_TYPE_ACCELEROMETER)) {
    return false;
  }
  data->raw.acceleration = *(struct Vector *)&event.acceleration;
  
  if (!lsm6ds33.GetEvent(&event, SENSOR_TYPE_GYROSCOPE)) {
    return false;
  }
  data->raw.gyro = *(struct Vector *)&event.gyro;
  
  return true;
}

bool BlueboyPeripherals::ReadTestRaw(struct AttitudeData *data) {  
  sensors_event_t event;
  
  // Adafruit's unified sensor vector has to be converted to our vectors
  if (!oneU.GetEventRaw(&event, SENSOR_TYPE_MAGNETIC_FIELD)) {
    return false;
  }
  data->raw.magnetic = *(struct Vector *)&event.magnetic;
  
  if (!oneU.GetEventRaw(&event, SENSOR_TYPE_ACCELEROMETER)) {
    return false;
  }
  data->raw.acceleration = *(struct Vector *)&event.acceleration;
  
  if (!oneU.GetEventRaw(&event, SENSOR_TYPE_GYROSCOPE)) {
    return false;
  }
  data->raw.gyro = *(struct Vector *)&event.gyro;
  
  return true;
}

bool BlueboyPeripherals::ReadOrientation(Device dev, struct AttitudeData *data) {
  switch (dev) {
    case Device::Own:
      return ReadOwnOrientation(data);
    case Device::Test:
      return ReadTestOrientation(data);
  }
  return false;
}

//! @todo finish this
bool BlueboyPeripherals::ReadOwnOrientation(struct AttitudeData *data) {
  return false;
}

//! @todo finish this
bool BlueboyPeripherals::ReadTestOrientation(struct AttitudeData *data) {
  return false;
}
