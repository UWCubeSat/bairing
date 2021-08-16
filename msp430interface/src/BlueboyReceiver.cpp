/*
 * BlueboyReceiver.cc
 */

#include "BlueboyReceiver.h"

namespace blueboy {

constexpr uint8_t ADDRESS = 0x3A;

BlueboyInterface Interface(BLUEBOY_BUS, ADDRESS);

void InitializeBlueboyInterface() {
  I2CBus *bus = eusci::GetI2C(BLUEBOY_BUS);

  bus->Begin(ADDRESS);

  bus->OnReceive(&OnBlueboyReceive);
  bus->OnRequest(&OnBlueboyRequest);
  
  Interface.UpdateSensorData(BlueboyInterface::Magnetometer,  Vector3(0.0, 1.0, 2.0));
  Interface.UpdateSensorData(BlueboyInterface::Accelerometer, Vector3(3.0, 4.0, 5.0));
  Interface.UpdateSensorData(BlueboyInterface::Gyroscope,     Vector3(6.0, 7.0, 8.0));
}

void OnBlueboyReceive(int bufsize) {
  Interface.OnReceive(bufsize);
  /*
  I2CBus *bus = eusci::GetI2C(BLUEBOY_BUS);
  volatile uint8_t test;

  if (!executing && bus->Available()) {
    // not currently executing a command, receive the command byte
    command = bus->Read();
    test = command;
    executing = true;
  }
  */
}

void OnBlueboyRequest() {
  Interface.OnRequest();
  /*
  struct BlueboyData data;
  I2CBus *bus = eusci::GetI2C(BLUEBOY_BUS);
  queried++;
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
          data.magnetic_field.x = 0.0 * queried;
          data.magnetic_field.y = 0.0 * queried;
          data.magnetic_field.z = 0.0 * queried;
          break;
        case BlueboyDataType::Acc:
          // accelerometer
          data.acceleration.x = 1.1 * queried;
          data.acceleration.y = 1.1 * queried;
          data.acceleration.z = 1.1 * queried;
          break;
        case BlueboyDataType::Gyro:
          // gyroscope
          data.angular_velocity.x = 2.2 * queried;
          data.angular_velocity.y = 2.2 * queried;
          data.angular_velocity.z = 2.2 * queried;
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
  */
}

}  // namespace blueboy
