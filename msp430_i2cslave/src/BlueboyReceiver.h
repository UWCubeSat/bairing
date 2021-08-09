/*
 * BlueboyReceiver.h
 */

#ifndef SRC_BLUEBOYRECEIVER_H_
#define SRC_BLUEBOYRECEIVER_H_

#include <BlueboyInterface.h>
#include "eUSCI/i2c.h"

using eusci::I2CBus;

namespace blueboy {

constexpr I2CBus::Handle BLUEBOY_BUS = I2CBus::B1;

struct BlueboyData {
  union {
    char data[16];                    // raw bytes
    struct Vector3 acceleration;       // m/s^2
    struct Vector3 magnetic_field;     // uT
    struct Vector3 angular_velocity;   // rad/s

    union {
      struct Vector3 euler;            // radians
      struct Quaternion quaternion;   // ???
    } orientation;
  };
};

// data type offsets
enum class BlueboyDataType {
  Mag         = 0x00,
  Acc         = 0x01,
  Gyro        = 0x02,
  Quaternion  = 0x03,
  Euler       = 0x04,
};

void OnBlueboyReceive(int bufsize);
void OnBlueboyRequest();

}  // namespace blueboy

#endif /* SRC_BLUEBOYRECEIVER_H_ */
