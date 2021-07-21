// Driver for the 1U test system over I2C

#ifndef ONEU_DRIVER_H_
#define ONEU_DRIVER_H_

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>

struct Quaternion {
  float x;
  float y;
  float z;
  float w;
};

struct OneUData {
  union {
    sensors_vec_t magnetic;           // uT
    sensors_vec_t acceleration;       // m/s^2
    sensors_vec_t gyro;               // rad/s
    
    union Orientation {
      sensors_vec_t euler;            // radians
      struct Quaternion quaternion;   // ???
    } orientation;
  };
};

// data type offsets
enum class OneUDataType {
  Mag         = 0x00,
  Acc         = 0x01,
  Gyro        = 0x02,
  Quaternion  = 0x03,
  Euler       = 0x04,
};

class OneUDriver {
 public: 
  OneUDriver(uint8_t address): _address(address) { }

  bool Initialize();
  bool ReadData(OneUDataType type, struct OneUData *data);
 private:
  uint8_t _address;
};

#endif
