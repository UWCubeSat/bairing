/*!
 * @file Blueboy.h
 * @author Sebastian S.
 * @brief Common data structures and command/telemetry constants for Blueboy
 */

#ifndef BLUEBOY_H_
#define BLUEBOY_H_

#include <stdint.h>

/**
 * Common data structures
 */

// 16-byte vector with 3 floats and 4 bytes of padding
// interpretable as a vector of x, y, z, or a vector of roll, pitch, heading
struct Vector {
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
  char reserved[4];
};

// 16-byte quaternion
struct Quaternion {
  float x;
  float y;
  float z;
  float w;
};

// interpretable either as 3 vectors of raw data, a vector of euler angles, or a quaternion
struct AttitudeData {
  union {
    struct {
      struct Vector magnetic;           // uT
      struct Vector acceleration;       // m/s^2
      struct Vector gyro;               // rad/s
    } raw;
	
    union {
      struct Vector euler;              // radians
      struct Quaternion quaternion;     // ???
    } orientation;
  };
};

// interpretable either as a vector of magnetometer, accelerometer, or gyroscope data, a vector of euler angles, or a quaternion
struct OneUData {
  union {
    union raw {
      struct Vector magnetic;           // uT
      struct Vector acceleration;       // m/s^2
      struct Vector gyro;               // rad/s
    } raw;
  
    union {
      struct Vector euler;              // radians
      struct Quaternion quaternion;     // ???
    } orientation;
  };
};

// device id
enum class Device {
  Own =   0x01,
  Test =  0x02
};

/**
 * Command and telemetry constants
 */

constexpr uint32_t SYNC_PATTERN = 0xDEADBEEF;

// Command IDs
enum class CommandID {
  Reset =             0x00,
  Echo =              0x01,
  
  BeginOwnAttitude =  0x10,
  EndOwnAttitude =    0x11,
  EndOwnAll =         0x1F,

  BeginTestAttitude = 0x20,
  EndTestAttitude =   0x21,
  EndTestAll =        0x2F,
  
  BeginCalibMag =     0xE0,
  EndCalibMag =       0xE1,
  ClearCalibMag =     0xE2,
  
  BeginCalibAcc =     0xE3,
  EndCalibAcc =       0xE4,
  ClearCalibAcc =     0xE5,
  
  BeginCalibGyro =    0xE6,
  EndCalibGyro =      0xE7,
  ClearCalibGyro =    0xE8,
  
  Invalid = 0xFF,
};

// Telemetry IDs
enum class TelemetryID {
  Status =  0x00,
  Message = 0x01,

  OwnAttitudeRaw =          0x10,
  OwnAttitudeEuler =        0x11,
  OwnAttitudeQuaternion =   0x12,

  TestAttitudeRaw =         0x10,
  TestAttitudeEuler =       0x11,
  TestAttitudeQuaternion =  0x12,
};

// data type offsets
enum class OneUDataType {
  Mag =         0x00,
  Acc =         0x01,
  Gyro =        0x02,
  Quaternion =  0x03,
  Euler =       0x04,
};

// mode to send attitude data in over bluetooth
enum class AttitudeMode {
  Raw = 0,
  Euler = 1,
  Quaternion = 2
};

#endif
