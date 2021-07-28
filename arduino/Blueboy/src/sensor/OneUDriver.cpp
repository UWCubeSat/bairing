#include "OneUDriver.h"

constexpr uint8_t REQUEST_BASE =  0x10;

bool OneUDriver::Initialize() {
  // initialize here!
  return true;
}

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
