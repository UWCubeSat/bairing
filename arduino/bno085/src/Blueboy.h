/*!
 * @file Blueboy.h
 * @author Sebastian S.
 * @brief Common data structures and command/telemetry constants for Blueboy.
 */

#ifndef BLUEBOY_H_
#define BLUEBOY_H_

#include <stdint.h>

/*
 * Common data structures
 */

/*!
 * @struct Vector
 * @brief A 3D vector interpretable as x, y, z, or roll, pitch, heading
 * 
 * A 16-byte vector with 3 floats and 4 bytes of padding.
 */
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

/*!
 * @struct Quaternion
 * @brief A quaternion interpretable as x, y, z, w
 */
struct Quaternion {
  float x;
  float y;
  float z;
  float w;
};

// interpretable either as 3 vectors of raw data, a vector of euler angles, or a quaternion
/*!
 * @struct AttitudeData
 * @brief A representation of a single attitude data packet.
 *
 * Interpretable either as three vectors of raw data, orientation in euler angles, or orientation as a quaternion.
 */
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

/*! 
 * @enum Device
 * Device id
 */
enum class Device {
  /*!
   * @var Device Device::Own
   * Represents the Blueboy system itself.
   */
  Own =   0x01,
  
  /*!
   * @var Device Device::Test
   * Represents the mounted test system.
   */
  Test =  0x02
};

/*
 * Command and telemetry constants
 */

/*!
 * @var uint32_t SYNC_PATTERN
 * The 32-bit sync pattern that should be sent or recognized before every packet. Bytes should be sent/received in little endian.
 */
constexpr uint32_t SYNC_PATTERN = 0xDEADBEEF;

/*! 
 * @enum CommandID
 * IDs of commands that can be recognized.
 */
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

/*! 
 * @enum TelemetryID
 * IDs of telemetry packets to use.
 */
enum class TelemetryID {
  Status =                  0x00,
  Message =                 0x01,

  OwnAttitudeRaw =          0x10,
  OwnAttitudeEuler =        0x11,
  OwnAttitudeQuaternion =   0x12,

  TestAttitudeRaw =         0x10,
  TestAttitudeEuler =       0x11,
  TestAttitudeQuaternion =  0x12,
};

/*! 
 * @enum AttitudeMode
 * Mode to send attitude data in over bluetooth.
 */
enum class AttitudeMode {
  Raw =         0x00,
  Euler =       0x01,
  Quaternion =  0x02
};

#endif
