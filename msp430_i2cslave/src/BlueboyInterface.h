/*
 * Blueboy.h
 *
 *  Created on: Aug 4, 2021
 *      Author: Sebastian S.
 *
 *  Interface with Blueboy
 */

#ifndef SRC_BLUEBOY_INTERFACE_H_
#define SRC_BLUEBOY_INTERFACE_H_

#include <cinttypes>

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
constexpr uint8_t Battery = 0x00;
constexpr uint8_t WhoAmI = 0x00;
constexpr uint8_t SensorHealth = 0x00;

// Control
constexpr uint8_t Operation = 0x00;
constexpr uint8_t Calibration = 0x00;

// Data
constexpr uint8_t RawMag = 0x00;
constexpr uint8_t RawAcc = 0x00;
constexpr uint8_t RawGyro = 0x00;
constexpr uint8_t Euler = 0x00;
constexpr uint8_t Quaternion = 0x00;

constexpr uint8_t GenFloats = 0x00;
constexpr uint8_t GenBufs = 0x00;

class BlueboyInterface {
public:
  enum Sensor : uint8_t {
    Magnetometer = 0,
    Accelerometer = 1,
    Gyroscope = 2
  };

  BlueboyInterface() { }

  void OnReceive(int bufsize);
  void OnRequest();

  bool ShouldReset();  // true if a reset has been requested
  bool ShouldOperate();  // true if the system should be in operation

  bool ShouldCalibrate(Sensor sensor);  // true if the given sensor should be calibrating

  void UpdateSensorHealth(Sensor sensor, bool healthy);  // Sets the health of the given sensor

  void UpdateSensorData(Sensor sensor, const struct Vector3& data);

  void UpdateEulerData(const struct Vector3& eulers);
  void UpdateQuaternionData(const struct Quaternion& quaternion);

  void UpdateGeneralFloatData(int index, float val);  // Updates the index'th general float

  void UpdateGeneralBufferData(int index, const char *buffer);  // Updates the index'th general buffer
protected:
  uint8_t _addressSpace[256];     // 256 byte address space
  uint8_t _reqAddr;               // address requested (first byte written by master)

  // Fetches a value of type T in the address space at the given address
  template <class T>
  T _get(uint8_t address) { return *((T *)&_addressSpace[address]); }

  // Writes a value of type T in the address space at the given address
  template <class T>
  void _set(uint8_t address, T value) { *((T *)&_addressSpace[address]) = value; }
};

}  // namespace blueboy

#endif /* SRC_BLUEBOY_INTERFACE_H_ */
