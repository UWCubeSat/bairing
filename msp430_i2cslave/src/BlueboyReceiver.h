/*
 * BlueboyReceiver.h
 */

#ifndef SRC_BLUEBOYRECEIVER_H_
#define SRC_BLUEBOYRECEIVER_H_

#include "eUSCI/i2c.h"

using eusci::I2CBus;

constexpr I2CBus::Handle BLUEBOY_BUS = I2CBus::B1;

struct Vector {
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

struct BlueboyData {
  union {
    char data[16];                    // raw bytes
    struct Vector acceleration;       // m/s^2
    struct Vector magnetic_field;     // uT
    struct Vector angular_velocity;   // rad/s

    union {
      struct Vector euler;            // radians
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

#endif /* SRC_BLUEBOYRECEIVER_H_ */
