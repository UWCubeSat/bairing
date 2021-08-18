/*!
 * @file BlueboyInterface.h
 * @author Sebastian S.
 * @brief Interface class and other definitions to allow for interfacing with Blueboy
 */

#ifndef SRC_BLUEBOY_INTERFACE_H_
#define SRC_BLUEBOY_INTERFACE_H_

#include <cinttypes>
#include "eUSCI/i2c.h"

using eusci::I2CBus;

namespace blueboy {

/*!
 * @struct Vector3
 * @brief A 3D vector interpretable as x, y, z, or roll, pitch, heading
 * 
 * A 16-byte vector with 3 floats and 4 bytes of padding.
 */
struct Vector3 {
  union {
    struct {
      float x;
      float y;
      float z;
    };
    struct {
      float roll;
  	  float pitch;
  	  float heading;
  	};
  };
  char __padding[4];  // get to a total size of 16 bytes
  
  Vector3(float x, float y, float z) : x(x), y(y), z(z) { }
};

/*!
 * @struct Quaternion
 * @brief A quaternion interpretable as x, y, z, w
 */
struct Quaternion {
  float x;
  float y;
  float z;
  float w;
  Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
};

/*
 * Register Addresses
 */

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

/*!
 * @class BlueboyInterface
 * @brief An example class that implements the Blueboy interface protocol
 */
class BlueboyInterface {
public:
  /*!
   * @enum Sensor
   * Represents one of onboard sensors
   */
  enum Sensor : uint8_t {
    Magnetometer = 0,
    Accelerometer = 1,
    Gyroscope = 2
  };

  /*!
   * @brief Constructs a BlueboyInterface
   * @param handle I2C handle for the interface to use
   * @param whoami Byte identifier for the interface
   */
  BlueboyInterface(I2CBus::Handle handle, uint8_t whoami);

  /*!
   * @brief I2C data receive callback
   * @param bufsize Number of bytes available in the receive buffer of the I2C bus identified by the stored handle
   */
  void OnReceive(int bufsize);
  
  /*!
   * @brief I2C data request callback
   */
  void OnRequest();
  
  /*!
   * @return true if a reset has been requested
   */
  bool ShouldReset();
  
  /*!
   * @return true if the system should be in operation
   */
  bool ShouldOperate();

  /*!
   * @return true if the given sensor should be calibrating
   * @param sensor The sensor to check for calibration
   */
  bool ShouldCalibrate(Sensor sensor);

  /*!
   * @brief Sets the health of the given sensor
   * @param sensor The sensor to check for health
   * @param healthy The health value to set for the sensor
   */
  void UpdateSensorHealth(Sensor sensor, bool healthy);

  /*!
   * @brief Updates the stored data for the given sensor
   * @param sensor The sensor to update the data of
   * @param data A Vector3 containing data to update stored data with
   */
  void UpdateSensorData(Sensor sensor, const struct Vector3& data);

  /*!
   * @brief Updates the stored orientation in Euler angles
   * @param data A Vector3 containing data to update stored data with
   */
  void UpdateEulerData(const struct Vector3& eulers);
  
  /*!
   * @brief Updates the stored orientation as a quaternion
   * @param data A Quaternion containing data to update stored data with
   */
  void UpdateQuaternionData(const struct Quaternion& quaternion);

  /*!
   * @brief Updates a stored general float
   * @param index The index of the general float value to update
   * @param val The float value to write to that general float
   *
   * Allows 0 <= index <= 3
   */
  void UpdateGeneralFloatData(int index, float val);

  /*!
   * @brief Updates a stored general buffer
   * @param index The index of the general buffer to update
   * @param buffer The buffer containing data to write to that general buffer
   *
   * Allows 0 <= index <= 2
   */
  void UpdateGeneralBufferData(int index, const char *buffer);
protected:
  uint8_t _addressSpace[256];     // 256 byte address space
  I2CBus::Handle _handle;         // i2c bus handle to use
  uint8_t _reqAddr;               // address requested (first byte written by master)
  bool _receivingAddr;            // whether the interface is currently expecting an address

  /*!
   * @brief Fetches a value of type T in the address space at the given address
   * @param address Lowest address of the value to read
   * @return sizeof(T) bytes containing the value at the given address as type T
   */
  template <class T>
  T _get(uint8_t address) { return *((T *)&_addressSpace[address]); }

  /*!
   * @brief Writes a value of type T in the address space at the given address
   * @param address Lowest address of the value to write
   */
  template <class T>
  void _set(uint8_t address, T value) { *((T *)&_addressSpace[address]) = value; }
};

}  // namespace blueboy

#endif /* SRC_BLUEBOY_INTERFACE_H_ */
