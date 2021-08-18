/*!
 * @file OneUDriver.h
 * @author Sebastian S.
 * @brief Implementation of OneUDriver.h
 */

#include <Arduino.h>
#include <Wire.h>
#include "OneUDriver.h"

/*!
 * @var uint8_t ONEU_ADDR
 * I2C address of the 1U test system.
 */
constexpr uint8_t ONEU_ADDR = 0x3A;

OneUDriver::OneUDriver() { }

/*!
 * @brief Translates a reading type into its corresponding data register address
 * @param type Sensor type to get the address of
 * @return Address of the sensor's data
 *
 * @todo Make these into constants
 */
static uint8_t addrFromType(sensors_type_t type) {
  switch (type) {
    case SENSOR_TYPE_MAGNETIC_FIELD:
      return 0x80;
    case SENSOR_TYPE_ACCELEROMETER:
      return 0x8C;
    case SENSOR_TYPE_GYROSCOPE:
      return 0x98;
    default:
      return 0;
  }
}

/*!
 * @brief Translates a reading type into the corresponding bit within a byte
 * @param type Sensor type to get the bit position of
 * @return Bit position of the sensor type, or -1 on an unrecognized sensor
 */
static int bitFromType(sensors_type_t type) {
  switch (type) {
    case SENSOR_TYPE_MAGNETIC_FIELD:
      return 0;
    case SENSOR_TYPE_ACCELEROMETER:
      return 1;
    case SENSOR_TYPE_GYROSCOPE:
      return 2;
    default:
      return -1;
  }
}

/*!
 * @brief Starts a transaction with the 1U over I2C, sending the register address with repeated start
 * @param addr Address of the register to access
 * @return True if the transaction was successfully carried out
 */
static bool requestAddress(uint8_t addr) {
  Wire.beginTransmission(ONEU_ADDR);
  Wire.write(addr);
  uint8_t status = Wire.endTransmission(false);
  if (status) {
    // not a success
    Serial.print(F("  OneUDriver failed to access address "));
    Serial.print(addr, HEX);
    Serial.print(F(": "));
    Serial.println(status);
    return false;
  }
  return true;
}

/*!
 * @brief Reads bytes from the 1U starting at the given register address
 * @param addr Address of the first register to access
 * @param len Number of bytes to read
 * @param buf Buffer to write read data into
 * @param stop True if a stop should be sent following this transaction, false for a repeated start
 * @return True if the transaction was successfully carried out
 */
static bool readAddress(uint8_t addr, uint8_t len, char *buf, bool stop = true) {
  if (!requestAddress(addr)) {
    return false;
  }
  
  uint8_t received = Wire.requestFrom(ONEU_ADDR, len, (uint8_t) stop);
  if (received != len) {
    Serial.print(F("  OneUDriver failed to request "));
    Serial.print(len);
    Serial.print(F(" bytes from address "));
    Serial.print(addr, HEX);
    Serial.print(F(": "));
    Serial.println(received);
    return false;
  }
  
  for (int i = 0; i < len; i++) {
    // there should be len bytes in the Wire buffer
    if (Wire.available()) {
      buf[i] = Wire.read();
    }
  }
  
  return true;
}

/*!
 * @brief Transmits bytes to the 1U starting at the given register address
 * @param addr Address of the first register to write to
 * @param len Number of bytes to write
 * @param buf Buffer containing data to write
 * @param stop True if a stop should be sent following this transaction, false for a repeated start
 * @return True if the transaction was successfully carried out
 */
static bool writeAddress(uint8_t addr, uint8_t len, const char *buf, bool stop = true) {
  if (!requestAddress(addr)) {
    return false;
  }
  
  Wire.beginTransmission(ONEU_ADDR);
  Wire.write(buf, len);
  uint8_t status = Wire.endTransmission(stop);
  if (status) {
    // not a success
    Serial.print(F("  OneUDriver failed to transmit "));
    Serial.print(len);
    Serial.print(F(" bytes from address "));
    Serial.print(addr, HEX);
    return false;
  }
  return true;
}

bool OneUDriver::Initialize() {
  
  return true;
}

bool OneUDriver::GetEventRaw(sensors_event_t *event, sensors_type_t type) {
  uint8_t addr = addrFromType(type);
  if (addr) {
    char *raw = (char *) event->data;
    
    bool result = readAddress(addr, 12, raw);
    
    delay(1);  // next read returns garbage data unless a miniscule delay exists, TODO figure out why instead of using this hack
    
    return result;
  }
  return false;
}

// tell the 1U to start calibrating a sensor
void OneUDriver::BeginCalibration(sensors_type_t type) {    
  uint8_t byte;  
  int bit = bitFromType(type);
  if (bit == -1) {
    return;
  }
  
  if (!_currCalibration) {
    
    if (!readAddress(0x11, 1, &byte, false)) {
      return;
    }
    
    byte |= (1 << bit);
    
    if (!writeAddress(0x11, 1, &byte, true)) {
      return;
    }
    _currCalibration = type;
  }
}

// tell the 1U to stop calibrating a sensor
void OneUDriver::EndCalibration() {
  uint8_t byte;
  int bit = bitFromType(_currCalibration);
  if (bit == -1) {
    return;
  }
  
  if (!readAddress(0x11, 1, &byte, false)) {
    return;
  }

  byte &= ~(1 << bit);

  if (!writeAddress(0x11, 1, &byte, true)) {
    return;
  }
  
  _currCalibration = 0;
}