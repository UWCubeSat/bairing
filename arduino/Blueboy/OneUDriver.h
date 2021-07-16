// Driver for the 1U test system over I2C

#ifndef ONEU_DRIVER_H_
#define ONEU_DRIVER_H_

#include <Arduino.h>
#include <Wire.h>

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

struct OneUData {
  union {
    char data[16];                    // raw bytes
    struct Vector acceleration;       // m/s^2
    struct Vector magnetic_field;     // uT
    struct Vector angular_velocity;   // rad/s
    
    union Orientation {
      struct Vector euler;            // radians
      struct Quaternion quaternion;   // ???
    } orientation;
  };
};

constexpr uint8_t REQUEST_BASE =  0x10;

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

  bool ReadData(OneUDataType type, struct OneUData *data);
 private:
  uint8_t _address;
};

bool OneUDriver::ReadData(OneUDataType type, struct OneUData *data) {
  Wire.beginTransmission(_address);
  Wire.write(REQUEST_BASE + ((uint8_t) type));
  int status = Wire.endTransmission(false);  // don't stop
  if (status != 0) {  // if not a success
    Serial.print("  OneUDriver failed to request data of type ");
    Serial.print((int) type, HEX);
    Serial.print(": ");
    Serial.println(status);
    return false;
  }
  uint8_t size = sizeof(struct OneUData);
  uint8_t received = Wire.requestFrom(_address, size);
  if (received != size) {
    Serial.print("  OneUDriver failed to receive data of type ");
    Serial.print((int) type, HEX);
    Serial.print(": ");
    Serial.println(received);
    return false;
  }
  
  for (int i = 0; Wire.available() && i < size; i++) {
    ((char *) data)[i] = Wire.read();
  }

  return true;
}

#endif
