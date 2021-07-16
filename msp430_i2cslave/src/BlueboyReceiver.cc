/*
 * BlueboyReceiver.cc
 */

#include "BlueboyReceiver.h"

static bool executing = false;
static uint8_t command;

void OnBlueboyReceive(int bufsize) {
  I2CBus *bus = eusci::GetI2C(BLUEBOY_BUS);
  volatile uint8_t test;

  if (!executing && bus->Available()) {
    // not currently executing a command, receive the command byte
    command = bus->Read();
    test = command;
    executing = true;
  }
}

void OnBlueboyRequest() {
  struct BlueboyData data;
  I2CBus *bus = eusci::GetI2C(BLUEBOY_BUS);
  if (executing) {
    if (command == 0x01) {
      // reset
    } else if (command == 0x02) {
      // start
    } else if (command == 0x04) {
      // stop
    } else if (command == 0x08) {
      // calibrate
    } else if ((command & 0xF0) == 0x10) {
      BlueboyDataType type = (BlueboyDataType) (command & 0x0F);
      switch (type) {
        case BlueboyDataType::Mag:
          // magnetometer
          data.magnetic_field.x = 0.0;
          data.magnetic_field.y = 0.0;
          data.magnetic_field.z = 0.0;
          break;
        case BlueboyDataType::Acc:
          // accelerometer
          data.acceleration.x = 1.1;
          data.acceleration.y = 1.1;
          data.acceleration.z = 1.1;
          break;
        case BlueboyDataType::Gyro:
          // gyroscope
          data.angular_velocity.x = 2.2;
          data.angular_velocity.y = 2.2;
          data.angular_velocity.z = 2.2;
          break;
        case BlueboyDataType::Quaternion:
          // orientation, quaternion
          data.orientation.quaternion.x = 3.3;
          data.orientation.quaternion.y = 3.3;
          data.orientation.quaternion.z = 3.3;
          break;
        case BlueboyDataType::Euler:
          // orientation, euler
          data.orientation.euler.x = 4.4;
          data.orientation.euler.y = 4.4;
          data.orientation.euler.z = 4.4;
          break;
      }

      bus->Write(data.data, sizeof(data));
    }
    executing = false;
  }
}


