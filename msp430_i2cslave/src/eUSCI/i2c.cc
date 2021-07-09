/*
 * i2c.cc
 */

#include "msp430plus.h"
#include "i2c.h"

using msp430::UCBx;

namespace eusci {

I2CBus I2C[4] = {
  I2CBus(I2CBusHandle::B0),
  I2CBus(I2CBusHandle::B1),
  I2CBus(I2CBusHandle::B2),
  I2CBus(I2CBusHandle::B3),
};

/*
// i2c bus initialization parameters
struct I2CInitParameters {
  int pbase;
  int sdapin;
  int sclpin;
  I2CInitParameters(int base, int sda, int scl): pbase(base), sdapin(sda), sclpin(scl) { }
};

struct I2CInitParameters I2CPARAMS[4] = {
  I2CInitParameters(1, 6, 7),  // B0, 1.6 and 1.7
  I2CInitParameters(5, 0, 1),  // B1, 5.0 and 5.1
  I2CInitParameters(7, 0, 1),  // B2, 7.0 and 7.1
  I2CInitParameters(6, 4, 5),  // B3, 6.4 and 6.5
};
*/

// begin as master
void I2CBus::Begin() {
  // TODO master
}

// begin as slave
void I2CBus::Begin(uint8_t address) {
  _address = address;
  _mode = I2CMode::Slave;

  struct UCBx *bus = msp430::UCB[(int) _bus];

  bus->ctlw0 = UCSWRST;                       // reset, begin configuration

  bus->ctlw0 |= UCMODE_3 | UCSYNC;            // i2c, slave mode (UCMST = 0)
  bus->i2coa0 = UCOAEN | (_address & 0x7f);   // set own address

  bus->ctlw0 &= ~UCSWRST;                     // clear reset, enable UCB
  bus->ie |= UCRXIE;                          // enable interrupt on receive

  _enable_interrupts();                       // enable general interrupts
}

// as master, request from slave
int I2CBus::RequestFrom(uint8_t address, int quantity, bool stop) {
  // TODO master
  return -1;
}

// begin transmission with slave
void I2CBus::BeginTransmission(uint8_t address) {
  // TODO master
}

// end current transmission, return status
int I2CBus::EndTransmission() {
  // TODO master
  return -1;
}

// write a single byte during a transmission, or in response to a master
int I2CBus::Write(char byte) {
  // queue a byte to be written
  return -1;
}

// write a buffer of data
int I2CBus::Write(char *buffer, int length) {
  // queue bytes to be written
  return -1;
}

// returns the number of bytes available to read
int I2CBus::Available() {
  // return size of read buffer
  return -1;
}

// reads a single byte, received from RequestFrom or transmitted from the master
// pre: available() > 0
char I2CBus::Read() {
  // pop first element from read buffer, if available
  return '\0';
}

// sets the clock frequency transmitted if a master
void I2CBus::SetClock(I2CClockFrequency freq) {
  // TODO master
  // configure clock divider
}

// calls the given function whenever this bus receives data as a slave
void I2CBus::BindReceiveCallback(void (*onReceive)(int)) {
  this->_onReceive = onReceive;
}

// calls the given function whenever this bus is requested data from the master
void I2CBus::BindRequestCallback(void (*onRequest)(int)) {
  this->_onRequest = onRequest;
}

// TODO set up interrupts to queue incoming bytes and

}  // namespace EUSCI
