/*
 * i2c.h
 */

#ifndef SRC_EUSCI_I2C_H_
#define SRC_EUSCI_I2C_H_

#include <cinttypes>
#include <msp430plus.h>
#include "eusci.h"

namespace eusci {

enum class I2CMode {
  Master,
  Slave
};

enum class I2CBusHandle {
  B0 = 0,
  B1 = 1,
  B2 = 2,
  B3 = 3
};

enum class I2CClockFrequency {
  // TODO set up some preset clock speeds
};

// I2C bus with buffered read/write
class I2CBus {
 public:
  I2CBus(I2CBusHandle bus): _bus(bus), _address(0) { }
  void Begin();                         // begin as master
  void Begin(uint8_t address);          // begin as slave
  int RequestFrom(uint8_t address,      // as master, request from slave
                   int quantity,
                   bool stop);
  void BeginTransmission(uint8_t address);  // begin transmission with slave
  int EndTransmission();                    // end current transmission, return status
  int Write(char byte);                     // write a byte during a transmission, or in response to a master
  int Write(char *buffer, int length);      // write a buffer of data
  int Available();                          // returns the number of bytes available to read
  char Read();                              // reads a single byte, received from RequestFrom or transmitted from the master
  void SetClock(I2CClockFrequency freq);    // sets the clock frequency transmitted if a master

  void BindReceiveCallback(void (*onReceive)(int));     // calls the given function whenever this bus receives data as a slave
  void BindRequestCallback(void (*onRequest)(int));     // calls the given function whenever this bus is requested data from the master
 private:
  I2CBusHandle _bus;                    // bus handle
  void (*_onReceive)(int);              // function called when slave receives transmitted bytes from the master
  void (*_onRequest)(int);              // function called when slave
  I2CMode _mode;                        // current I2C mode
  bool _transmitting;                   // true iff currently in a transmission
  uint8_t _address;                     // slave address
};

extern I2CBus I2C[4];

}  // namespace eusci

#endif /* SRC_EUSCI_I2C_H_ */
