/*!
 * @file BlueboyReceiver.cpp
 * @author Sebastian S.
 * @brief Implementation of BlueboyReceiver.h
 */

#include "BlueboyReceiver.h"

namespace blueboy {

/*!
 * @var uint8_t ADDRESS
 * I2C address for the Blueboy interface to listen for
 */
constexpr uint8_t ADDRESS = 0x3A;

BlueboyInterface Interface(BLUEBOY_BUS, ADDRESS);

void InitializeBlueboyInterface() {
  I2CBus *bus = eusci::GetI2C(BLUEBOY_BUS);

  bus->Begin(ADDRESS);

  bus->OnReceive(&OnBlueboyReceive);
  bus->OnRequest(&OnBlueboyRequest);
  
  // for testing
  Interface.UpdateSensorData(BlueboyInterface::Magnetometer,  Vector3(0.0, 1.0, 2.0));
  Interface.UpdateSensorData(BlueboyInterface::Accelerometer, Vector3(3.0, 4.0, 5.0));
  Interface.UpdateSensorData(BlueboyInterface::Gyroscope,     Vector3(6.0, 7.0, 8.0));
}

void OnBlueboyReceive(int bufsize) {
  Interface.OnReceive(bufsize);
}

void OnBlueboyRequest() {
  Interface.OnRequest();
}

}  // namespace blueboy
