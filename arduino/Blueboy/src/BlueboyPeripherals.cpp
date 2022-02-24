/*!
 * @file BlueboyPeripherals.cpp
 * @author Sebastian S.
 * @brief Implementation of BlueboyPeripherals.h
 */

#include "BlueboyPeripherals.h"

bool BlueboyPeripherals::Initialize() {
  if (_initialized) {
    return true;
  }

  CalibrationStorage::Initialize();
  
  /*
  if (!lis2mdl.Initialize()) {
    Serial.println(F("Failed to find LIS2MDL"));
    return false;
  }
  
  if (!lsm6ds33.Initialize()) {
    Serial.println(F("Failed to find LSM6DS33"));
    return false;
  }
  */
  
  if (!bno080.Initialize()) {
    Serial.println(F("Failed to find BNO080"));
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
  
  if (bno080.UpdateReadings()) {
    if (!bno080.GetEvent(&event, SENSOR_TYPE_MAGNETIC_FIELD)) {
      return false;
    }
    data->raw.magnetic = *(struct Vector *)&event.magnetic;
    
    if (!bno080.GetEvent(&event, SENSOR_TYPE_ACCELEROMETER)) {
      return false;
    }
    data->raw.acceleration = *(struct Vector *)&event.acceleration;
    
    if (!bno080.GetEvent(&event, SENSOR_TYPE_GYROSCOPE)) {
      return false;
    }
    data->raw.gyro = *(struct Vector *)&event.gyro;
    
    return true;
  }  
  
  return false;
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

bool BlueboyPeripherals::ReadQuaternion(Device dev, struct AttitudeData *data) {
  switch (dev) {
    case Device::Own:
      return ReadOwnQuaternion(data);
    case Device::Test:
      return ReadTestQuaternion(data);
  }
  return false;
}

bool BlueboyPeripherals::ReadOwnQuaternion(struct AttitudeData *data) {
  struct Quaternion quaternion;
  if (bno080.GetOrientationQuaternion(&quaternion)) {
    data->orientation.quaternion = quaternion;
    return true;
  }
  return false;
}

//! @todo finish this
bool BlueboyPeripherals::ReadTestQuaternion(struct AttitudeData *data) {
  return false;
}

bool BlueboyPeripherals::ReadEulers(Device dev, struct AttitudeData *data) {
  switch (dev) {
    case Device::Own:
      return ReadOwnEulers(data);
    case Device::Test:
      return ReadTestEulers(data);
  }
  return false;
}

bool BlueboyPeripherals::ReadOwnEulers(struct AttitudeData *data) {
  struct Vector eulers;
  if (bno080.GetOrientationEulers(&eulers)) {
    data->orientation.eulers = eulers;
    return true;
  }
  return false;
}

//! @todo finish this
bool BlueboyPeripherals::ReadTestEulers(struct AttitudeData *data) {
  return false;
}
