// Driver for the 1U test system over I2C

#ifndef ONEU_DRIVER_H_
#define ONEU_DRIVER_H_

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>

#include "../Blueboy.h"

class OneUDriver {
 public: 
  OneUDriver(uint8_t address): _address(address) { }

  bool Initialize();
  bool ReadData(OneUDataType type, struct OneUData *data);
 private:
  uint8_t _address;
};

#endif
