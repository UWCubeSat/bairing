/*
 * i2c.h
 *
 * Asynchronous I2C library with buffered read/write for the msp430,
 * with an interface loosely based on Arduino's Wire.h
 *
 * TODO
 *   - Slave write
 *   - Master read
 *   - Master write
 *   - Toggling asynchronous <-> synchronous
 *   - Blocking
 */

#ifndef SRC_EUSCI_I2C_H_
#define SRC_EUSCI_I2C_H_

#include <cinttypes>
#include <msp430plus.h>
#include "eusci.h"
#include "util/SimpleQueue.h"

using util::SimpleQueue;

namespace eusci {

enum class I2CMode {
  Unused,
  Master,
  Slave
};

enum class I2CClockFrequency {
  // TODO set up some preset clock speeds
};

constexpr int I2C_RWBUFLEN = 32;          // size of the read/write buffers in bytes

// I2C bus with buffered read/write
class I2CBus {
 public:
  // Bus handles, identifier for the eUSCI_B bus being used by this I2CBus
  enum Handle {
    B0 = 0,
    B1 = 1,
    B2 = 2,
    B3 = 3
  };

  I2CBus(Handle bus):
    _bus(bus), _address(0), _mode(I2CMode::Unused), //, _rsize(0), _wsize(0), _windex(0),
    _transmitting(false), _started(false),
    _onReceive([](int size) { }), _onRequest([]() { }) { }
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

  void BindReceiveCallback(void (*_onReceive)(int size));  // calls the given function whenever this bus receives data as a slave
  void BindRequestCallback(void (*_onRequest)());          // calls the given function whenever this bus is requested data from the master
  I2CMode GetMode();                        // returns the bus's current mode

  void ISRHandler();                        // interrupt service routine handler

  /*
  I2CBus() = delete;
  I2CBus(const I2CBus&) = delete;
  I2CBus(const I2CBus&&) = delete;
  */
 private:
  /*
  char _rbuf[RWBUFLEN];                 // read buffer
  char _wbuf[RWBUFLEN];                 // write buffer
  int _rsize, _wsize, _windex;          // current sizes of the read and write buffers, current index in write buffer
  */
  SimpleQueue<char> _rbuf;              // read buffer
  SimpleQueue<char> _wbuf;              // write buffer

  Handle _bus;                          // bus handle
  void (*_onReceive)(int size);         // function called when slave receives transmitted bytes from the master
  void (*_onRequest)();                 // function called when slave receives request for transmission to master
  I2CMode _mode;                        // current I2C mode
  bool _transmitting;                   // true iff currently in a transmission
  bool _started;                        // whether an initial start condition has been received
  uint8_t _address;                     // slave address

  // private helper functions
  inline struct msp430::UCBx *getBase();  // returns a pointer to this I2C bus's UCB register base
  void gpioInit();                        // initializes gpio pins for this I2C bus
  void receiveByte();                     // receives a byte if one is available and puts it on the read buffer
  void completeReadBuffer();              // completes the read buffer and sends it to the user
};

// returns a pointer to the I2C bus associated with the given handle
I2CBus *GetI2C(I2CBus::Handle handle);

// debug statistics, TODO remove eventually
extern int STARTS, STOPS, READS;

}  // namespace eusci

#endif /* SRC_EUSCI_I2C_H_ */
