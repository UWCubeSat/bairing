/*
 * BlueboyInterface.cpp
 *
 *  Created on: Aug 6, 2021
 *      Author: Sebastian S.
 */

#include "BlueboyInterface.h"
#include <cstring>

namespace blueboy {

BlueboyInterface::BlueboyInterface(I2CBus::Handle handle, uint8_t whoami) : _handle(handle), _receivingAddr(true) {
  memset(_addressSpace, '\0', 256);
  _set<uint8_t>(WhoAmI, whoami);
}

void BlueboyInterface::OnReceive(int bufsize) {
  // if receiving address, take first byte to be the address
  // otherwise, write values in buffer to the address
  //   if incremental addressing enabled, increment address

  I2CBus *bus = eusci::GetI2C(_handle);

  if (_receivingAddr) {
    _reqAddr = bus->Read();
    _receivingAddr = false;
  } else {
    while (bus->Available()) {
      uint8_t read = bus->Read();
      _set<uint8_t>(_reqAddr, read);
      _reqAddr++;
    }
    _receivingAddr = true;
  }
}

void BlueboyInterface::OnRequest() {
  // write the value at the stored address to i2c
  //   if incremental addressing enabled, increment address

  I2CBus *bus = eusci::GetI2C(_handle);

  bus->Write(_get<uint8_t>(_reqAddr));
  _reqAddr++;
  _receivingAddr = true;
}

bool BlueboyInterface::ShouldReset() {
  // return bit 0 of Operation byte
  return (_get<uint8_t>((uint8_t) Operation) & (1 << 0)) != 0;
}

bool BlueboyInterface::ShouldOperate() {
  // return bit 1 of Operation byte
  return (_get<uint8_t>(Operation) & (1 << 1)) != 0;
}

bool BlueboyInterface::ShouldCalibrate(Sensor sensor) {
  // return sensor'th bit of Calibration byte
  return (_get<uint8_t>(Calibration) & (1 << sensor)) != 0;
}

void BlueboyInterface::UpdateSensorHealth(Sensor sensor, bool healthy) {
  // set the sensor'th bit to healthy
  uint8_t byte = _get<uint8_t>(SensorHealth);
  uint8_t mask = ~(1 << sensor);
  byte = (byte & mask) | (byte & (healthy << sensor));
}

void BlueboyInterface::UpdateSensorData(Sensor sensor, const struct Vector3& data) {
  // set the float values of the given sensor's data to the vector passed in
  uint8_t addr;
  switch(sensor) {
    case Magnetometer:
      addr = RawMag;
      break;
    case Accelerometer:
      addr = RawAcc;
      break;
    case Gyroscope:
      addr = RawGyro;
      break;
    default:
      return;
  }

  // set three floats following addr
  _set<float>(addr, data.x);
  _set<float>(addr + 4, data.y);
  _set<float>(addr + 8, data.z);
}

void BlueboyInterface::UpdateEulerData(const struct Vector3& eulers) {
  // set the float values of the orientation data in eulers to the vector passed in
  _set<float>(Euler, eulers.x);
  _set<float>(Euler + 4, eulers.y);
  _set<float>(Euler + 8, eulers.z);
}

void BlueboyInterface::UpdateQuaternionData(const struct Quaternion& quaternion) {
  // set the float values of the orientation data in a quaternion to the vector passed in
  _set<float>(Quaternion, quaternion.x);
  _set<float>(Quaternion + 4, quaternion.y);
  _set<float>(Quaternion + 8, quaternion.z);
  _set<float>(Quaternion + 12, quaternion.w);
}

void BlueboyInterface::UpdateGeneralFloatData(int index, float val) {
  // set the float value of the index'th general float to value
  _set<float>(GenFloats + index * 4, val);
}

void BlueboyInterface::UpdateGeneralBufferData(int index, const char *buffer) {
  // copies the bytes from buffer to the index'th general buffer
  for (int i = 0; i < 16; i++) {
    _set<uint8_t>(GenBufs + index * 16 + i, buffer[i]);
  }
}

}  // namespace blueboy
