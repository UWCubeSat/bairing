/*
 * BlueboyInterface.h
 *
 *  Created on: Aug 4, 2021
 *      Author: Sebastian S.
 *
 *  Interface class and other definitions to allow for interfacing with Blueboy
 */

#ifndef SRC_BLUEBOY_INTERFACE_H_
#define SRC_BLUEBOY_INTERFACE_H_

#include <cinttypes>
#include "eUSCI/i2c.h"

using eusci::I2CBus;

namespace blueboy {

struct Vector3 {
  union {
    struct {
      float x;
      float y;
      float z;
    };
    struct {
      float azimuth;
      float pitch;
      float roll;
    };
  };
  char __padding[4];  // get to a total size of 16 bytes
};

struct Quaternion {
  float x;
  float y;
  float z;
  float w;
};

// Addresses
// Status
constexpr uint8_t WhoAmI =        0x00;   // 1
constexpr uint8_t SensorHealth =  0x01;   // 1
constexpr uint8_t Battery =       0x02;   // 1

// Control
constexpr uint8_t Operation =     0x10;   // 1
constexpr uint8_t Calibration =   0x11;   // 1

// Data
constexpr uint8_t RawMag =        0x80;   // 12
constexpr uint8_t RawAcc =        0x8C;   // 12
constexpr uint8_t RawGyro =       0x98;   // 12
constexpr uint8_t Euler =         0xA4;   // 12
constexpr uint8_t Quaternion =    0xB0;   // 16

constexpr uint8_t GenFloats =     0xC0;   // 16
constexpr uint8_t GenBufs =       0xD0;   // 48

class BlueboyInterface {
public:
  enum Sensor : uint8_t {
    Magnetometer = 0,
    Accelerometer = 1,
    Gyroscope = 2
  };

  BlueboyInterface(I2CBus::Handle handle);

  void OnReceive(int bufsize);
  void OnRequest();

  bool ShouldReset();     // true if a reset has been requested
  bool ShouldOperate();   // true if the system should be in operation

  bool ShouldCalibrate(Sensor sensor);  // true if the given sensor should be calibrating

  void UpdateSensorHealth(Sensor sensor, bool healthy);  // Sets the health of the given sensor

  void UpdateSensorData(Sensor sensor, const struct Vector3& data);   // Updates the stored data for the given sensor

  void UpdateEulerData(const struct Vector3& eulers);                 // Updates the stored orientation in Euler angles
  void UpdateQuaternionData(const struct Quaternion& quaternion);     // Updates the stored orientation as a Quaternion

  void UpdateGeneralFloatData(int index, float val);            // Updates the index'th general float

  void UpdateGeneralBufferData(int index, const char *buffer);  // Updates the index'th general buffer
protected:
  uint8_t _addressSpace[256];     // 256 byte address space
  I2CBus::Handle _handle;         // i2c bus handle to use
  uint8_t _reqAddr;               // address requested (first byte written by master)
  bool _receivingAddr;            // whether the interface is currently expecting an address

  // Fetches a value of type T in the address space at the given address
  template <class T>
  T _get(uint8_t address) { return *((T *)&_addressSpace[address]); }

  // Writes a value of type T in the address space at the given address
  template <class T>
  void _set(uint8_t address, T value) { *((T *)&_addressSpace[address]) = value; }
};

}  // namespace blueboy

#endif /* SRC_BLUEBOY_INTERFACE_H_ */
