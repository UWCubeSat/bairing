/*!
 * @file CalibratedBNO080.h
 * @author Sebastian S.
 * @brief Implementation of CalibratedBNO080.h
 */

#include "CalibratedBNO080.h"

/*!
 * @var uint8_t BNO080_ADDR
 * Default I2C address of our BNO080.
 */
constexpr uint8_t BNO080_ADDR = 0x4A;

/*!
 * @var uint16_t BNO080_PERIOD
 * Default sample period between packets.
 */
constexpr uint16_t BNO080_PERIOD = 100;

CalibratedBNO080::CalibratedBNO080() : _bno080(BNO080()) { }

bool CalibratedBNO080::Initialize() {
  bool status = _bno080.begin(BNO080_ADDR);
  if (status) {
    _bno080.enableRotationVector(BNO080_PERIOD / 4);  // quaternions; also enables calculation of eulers
    _bno080.enableMagnetometer(BNO080_PERIOD / 4);    // magnetic field
    _bno080.enableAccelerometer(BNO080_PERIOD / 4);   // acceleration including gravity
    _bno080.enableGyro(BNO080_PERIOD / 4);            // angular rotation
  }
  return status;
}

bool CalibratedBNO080::UpdateReadings() {
  return _bno080.dataAvailable();
}

bool CalibratedBNO080::GetEventRaw(sensors_event_t *event, sensors_type_t type) {
  // grab all data that may be pending
  while (_bno080.dataAvailable()) { }
  
  uint8_t accuracy;
  
  switch (type) {
    case SENSOR_TYPE_MAGNETIC_FIELD:
      _bno080.getMag(event->magnetic.x, event->magnetic.y, event->magnetic.z, accuracy);
    case SENSOR_TYPE_ACCELEROMETER:
      _bno080.getAccel(event->acceleration.x, event->acceleration.y, event->acceleration.z, accuracy);
    case SENSOR_TYPE_GYROSCOPE:
      _bno080.getGyro(event->gyro.x, event->gyro.y, event->gyro.z, accuracy);
    default:
      return false;
  }
  return true;
}

bool CalibratedBNO080::GetOrientationQuaternion(struct Quaternion *quaternion) {
  // grab all data that may be pending
  while (_bno080.dataAvailable()) { }
  
  quaternion->x = _bno080.getQuatI();
  quaternion->y = _bno080.getQuatJ();
  quaternion->z = _bno080.getQuatK();
  quaternion->w = _bno080.getQuatReal();
  return true;
}

bool CalibratedBNO080::GetOrientationEulers(struct Vector *eulers) {
  // grab all data that may be pending
  while (_bno080.dataAvailable()) { }
  
  eulers->roll = _bno080.getRoll();
  eulers->pitch = _bno080.getPitch();
  eulers->heading = _bno080.getYaw();
  return true;
}

void CalibratedBNO080::BeginCalibration(sensors_type_t type) {
  switch (type) {
    case SENSOR_TYPE_MAGNETIC_FIELD:
      _bno080.calibrateMagnetometer();
    case SENSOR_TYPE_ACCELEROMETER:
      _bno080.calibrateAccelerometer();
    case SENSOR_TYPE_GYROSCOPE:
      _bno080.calibrateGyro();
  }
}

void CalibratedBNO080::GetCalibration(struct AxisOffsets *offsets, sensors_type_t type) {
  offsets->xOff = 0.0;
  offsets->yOff = 0.0;
  offsets->zOff = 0.0;
}

void CalibratedBNO080::EndCalibration() {
  _bno080.endCalibration();
  _bno080.saveCalibration();
}

void CalibratedBNO080::ClearCalibration(sensors_type_t type) { }