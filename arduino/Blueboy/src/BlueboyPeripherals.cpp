#include "BlueboyPeripherals.h"

constexpr uint8_t ONEU_ADDRESS = 0x3A;

BlueboyPeripherals::BlueboyPeripherals(): lsm6ds33(CalibratedLSM6DS33()),
                                          lis2mdl(CalibratedLIS2MDL()),
                                          _oneU(ONEU_ADDRESS) , _initialized(false) { }

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

  if (!_oneU.Initialize()) {
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
  
  lis2mdl.GetEvent(&event);
  data->raw.magnetic = *(struct Vector *)&event.magnetic;
  
  lsm6ds33.GetEventRaw(&event, SENSOR_TYPE_ACCELEROMETER);
  data->raw.acceleration = *(struct Vector *)&event.acceleration;
  
  lsm6ds33.GetEvent(&event, SENSOR_TYPE_GYROSCOPE);
  data->raw.gyro = *(struct Vector *)&event.gyro;
  
  return true;
}

bool BlueboyPeripherals::ReadTestRaw(struct AttitudeData *data) {
  struct OneUData oud;
  if (!_oneU.ReadData(OneUDataType::Mag, &oud)) {
    return false;
  }
  data->raw.magnetic = oud.raw.magnetic;
  
  if (!_oneU.ReadData(OneUDataType::Acc, &oud)) {
    return false;
  }
  data->raw.acceleration = oud.raw.acceleration;
  
  if (!_oneU.ReadData(OneUDataType::Gyro, &oud)) {
    return false;
  }
  data->raw.gyro = oud.raw.gyro;
  
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

bool BlueboyPeripherals::ReadOwnOrientation(struct AttitudeData *data) {
  return false;
}

bool BlueboyPeripherals::ReadTestOrientation(struct AttitudeData *data) {
  return false;
}
