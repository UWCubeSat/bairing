/*
 * i2c.cc
 */

#include "msp430plus.h"
#include "i2c.h"

namespace eusci {

I2CBus I2C[4] = {
  I2CBus(msp430::UCB[0]),
  I2CBus(msp430::UCB[1]),
  I2CBus(msp430::UCB[2]),
  I2CBus(msp430::UCB[3]),
};

// begin as master
void I2CBus::Begin() {
  // TODO master
}

// begin as slave
void I2CBus::Begin(uint8_t address) {
  this->_address = address;
  this->_mode = I2CMode::Slave;

  // modify mode register
  // modify own address register
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
