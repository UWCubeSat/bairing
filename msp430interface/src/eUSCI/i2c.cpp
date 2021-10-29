/*
 * i2c.cc
 */

#include "msp430plus.h"
#include "i2c.h"

using msp430::UCBx;

namespace eusci {

I2CBus I2CBuses[4] = {
  I2CBus(I2CBus::B0),
  I2CBus(I2CBus::B1),
  I2CBus(I2CBus::B2),
  I2CBus(I2CBus::B3),
};

I2CBus* GetI2C(I2CBus::Handle handle) {
  return &I2CBuses[(int) handle];
}

I2CStatus lastResult = I2CStatus::NoError;

/*
 * I2CBus Definitions
 */

// begin as master
void I2CBus::Begin() {
  _address = 0x00;          // no address since we're the master on the bus
  _mode = I2CMode::Master;

  gpioInit();

  struct UCBx *base = msp430::UCB[(int) _bus];

  base->ctlw0 |= UCSWRST;        // hold in reset state

  base->ctlw1 |= (UCMODE_3 | UCMST | UCSYNC);   // i2c mode, master

  base->ie |= (UCRXIE | UCTXIE | UCSTTIE | UCSTPIE | UCNACKIFG);    // enable interrupts for rx, tx, stop, start, nack

  base->ctlw0 &= ~UCSWRST;

  _enable_interrupts();         // enable general interrupts
}

// begin as slave
void I2CBus::Begin(uint8_t address) {
  _address = address;
  _mode = I2CMode::Slave;

  gpioInit();

  struct UCBx *base = msp430::UCB[(int) _bus];

  base->ctlw0 = UCSWRST;                       // reset, begin configuration

  base->ctlw0 |= UCMODE_3 | UCSYNC;            // i2c, slave mode (UCMST = 0)
  base->i2coa0 = UCOAEN | (_address & 0x7f);   // set own address, enable

  base->ctlw0 &= ~UCSWRST;                     // clear reset, enable UCB

  base->ie |= UCRXIE | UCTXIE | UCSTTIE | UCSTPIE;      // enable interrupt on receive, transmit, start and stop

  _enable_interrupts();                        // enable general interrupts
}

// as master, request quantity bytes from the slave at the given address
// if stop, send a stop condition at the end of the request; else, send a repeated start
I2CStatus I2CBus::RequestFrom(uint8_t address, int quantity, bool stop) {
  // TODO master
  //  make sure we're in receive mode
  struct UCBx *base = getBase();
  base->i2csa = address;
  static int bytesToRead = 0;
  static bool stopFlag = false;
  
  // only valid in master mode
  if (GetMode() != I2CMode::Master) return I2CStatus::InvalidOp;

  bytesToRead = quantity;
  stopFlag = stop;

  // autostop?
  if (stop) {
    base->ctlw0 |= UCSWRST;
    base->ctlw1 |= UCASTP_2;
    base->tbcnt = bytesToRead;
    base->ctlw0 &= ~UCSWRST;
  } else {
    base->ctlw0 |= UCSWRST;
    base->ctlw1 &= ~UCASTP_2;
    base->ctlw0 &= ~UCSWRST;
  }

  // send start condition, in receive mode
  base->ctlw0 &= ~(UCTR);
  base->ctlw0 |= (UCTXSTT);
  return I2CStatus::NoError;
}

// begin transmission with slave
I2CBus::I2CStatus I2CBus::BeginTransmission(uint8_t address) {
  // TODO master
    // steps: check if bus is busy
    //        send start condition
    //        send addresss + read/write bit
    //        wait for ack? use interrupt probably, if we don't get ack send repeated start condition
  struct UCBx *base = getBase();
  base->i2csa = address;

  // this call is only valid in master mode, so just return if it isn't configured that way
  if (GetMode() != I2CMode::Master) return I2CStatus::InvalidOp;

  // spin wait until our bus is released
  // TODO: add a timeout jic
  // return if we timeout with timeout error
  if (base->statw & UCBBUSY) return I2CStatus::Busy;

  // send start condition
  base->ctlw1 &= (UCTXSTT | UCTR);

  // wait for start to be sent
  // TODO: add some kind of timeout just in case?
  while (base->ctlw0 & UCTXSTT) { }
  return I2CStatus::NoError;
}

// end current transmission, return status
I2CBus::I2CStatus I2CBus::EndTransmission() {
  // this call is only valid in master mode, so just return if it isn't configured that way
  struct UCBx *base = getBase();
  if (GetMode() != I2CMode::Master) return I2CStatus::InvalidOp;

  // send stop condition
  base->ctlw0 |= (UCTXSTP);

  // wait for stop to be sent
  // TODO: add some kind of timeout just in case?
  while (base->ctlw0 & UCTXSTP) { }

  return I2CStatus::NoError;
}

// write a single byte during a transmission, or in response to a master
int I2CBus::Write(char byte) {
  return Write(&byte, 1);
}

// write a buffer of data during a transmission, or in response to a master
// return the number of bytes written
int I2CBus::Write(char *buffer, int length) {
  int written;
  for (written = 0; written < length; written++) {
    if (!_wbuf.Push(buffer[written])) {
      break;
    }
  }
  return written;
}

// returns the number of bytes available to read
int I2CBus::Available() {
  // return size of read buffer
  return _rbuf.Size();
}

// reads a single byte, received from RequestFrom or transmitted from the master
// pre: available() > 0
char I2CBus::Read() {
  return _rbuf.Pop();
}

// sets the clock frequency transmitted if a master
void I2CBus::SetClock(I2CClockFrequency freq) {
  // TODO master
  // configure clock divider
  // how can we do this without knowing the exact clock frequency? Hard to say.
  // Might be a good idea to have some kind of config in the header for masterclock frequency,
  // otherwise I don't know how to programmatically determine the clock frequency
}

// calls the given function whenever this bus receives data as a slave
void I2CBus::OnReceive(void (*onReceive)(int size)) {
  _onReceive = onReceive;
}

// calls the given function whenever this bus is requested data from the master
void I2CBus::OnRequest(void (*onRequest)()) {
  _onRequest = onRequest;
}

// returns the bus's current mode
I2CMode I2CBus::GetMode() {
  return _mode;
}

/*
 * Private I2CBus helper functions
 */

// i2c bus initialization parameters
struct I2CInitParameters {
  int pbase;
  int sdapin;
  int sclpin;
  uint8_t funcsel;
  I2CInitParameters(int base, int sda, int scl, uint8_t sel) : pbase(base), sdapin(sda), sclpin(scl), funcsel(sel) { }
};

const struct I2CInitParameters I2CPARAMS[4] = {
  I2CInitParameters(1, 6, 7, 0b10),  // B0, 1.6 and 1.7, secondary
  I2CInitParameters(5, 0, 1, 0b01),  // B1, 5.0 and 5.1, primary
  I2CInitParameters(7, 0, 1, 0b01),  // B2, 7.0 and 7.1, primary
  I2CInitParameters(6, 4, 5, 0b01),  // B3, 6.4 and 6.5, primary
};

// initializes gpio pins for this I2C bus
void I2CBus::gpioInit() {
  struct I2CInitParameters param = I2CPARAMS[(int) _bus];
  struct msp430::GPIOPx *i2cport = msp430::GPIOP[param.pbase];
  uint8_t mask = (1 << param.sdapin) | (1 << param.sclpin);

  // configure sel0
  if (param.funcsel & 0x1) {
    i2cport->sel0 |= mask;
  } else {
    i2cport->sel0 &= ~mask;
  }

  // configure sel1
  if (param.funcsel & 0x2) {
    i2cport->sel1 |= mask;
  } else {
    i2cport->sel1 &= ~mask;
  }

  // disable high-impedance mode to activate configured port settings
  PM5CTL0 &= ~LOCKLPM5;
}

// returns a pointer to this I2C bus's UCB register base
inline struct msp430::UCBx *I2CBus::getBase() {
  return msp430::UCB[(unsigned int) _bus];
}

// debug statistics, TODO remove eventually
int STARTS = 0;
int STOPS = 0;
int READS = 0;
int WRITES = 0;

// receives a byte if one is available and puts it on the read buffer
void I2CBus::receiveByte() {
  char byte = getBase()->rxbuf;
  READS++;

  _rbuf.Push(byte);
}

// completes the read buffer and sends it to the user
void I2CBus::completeReadBuffer() {
  // if there are currently bytes in the read buffer, process them
  if (!_rbuf.Empty()) {
    // will not happen on a non-repeated start
    _onReceive(_rbuf.Size());
  }
}

// interrupt service routine handler
void I2CBus::ISRHandler() {
  struct UCBx *base = getBase();

  switch (__even_in_range(base->iv, USCI_I2C_UCBIT9IFG)) {
    case USCI_NONE:                 // No interrupts
      break;
    case USCI_I2C_UCALIFG:          // Arbitration lost
      break;
    case USCI_I2C_UCNACKIFG:        // Nack received in master mode
      EndTransmission();

      // TODO: set result of transmission to NACK
      // increase error count
      break;
    case USCI_I2C_UCSTTIFG:         // Start condition received
      STARTS++;

      if (!_started) {
        _started = true;
      } else if (!_rbuf.Empty()) {
        // already started without stopping, so this is a repeated start and
        // we should complete the current buffer
        completeReadBuffer();
      }
      _requested = false;

      break;
    case USCI_I2C_UCSTPIFG:         // Stop condition received
      STOPS++;

      // sometimes the rx interrupt just sorta doesn't fire on the last
      // byte before a stop, shnag it here so we don't lose it
      if (base->ifg & UCRXIFG0) {
        receiveByte();
      }

      if (!_rbuf.Empty()) {
        completeReadBuffer();
      }

      _started = false;
      break;
    case USCI_I2C_UCRXIFG3:         // Complete byte received in slave mode on address 3
    case USCI_I2C_UCRXIFG2:         // Complete byte received in slave mode on address 2
    case USCI_I2C_UCRXIFG1:         // Complete byte received in slave mode on address 1
    case USCI_I2C_UCRXIFG0:         // Complete byte received in slave mode on address 0
      receiveByte();
      if ((GetMode() == I2CMode::Master) && (--bytesToRead == 0)) {
        if (stopFlag) {
          // end transmission
          EndTransmission();
        } else {
          // send repeated start
          BeginTransmission(base->i2csa);
        }
      }
      break;
    case USCI_I2C_UCTXIFG3:         // Transmit buffer empty in slave mode on address 3
    case USCI_I2C_UCTXIFG2:         // Transmit buffer empty in slave mode on address 2
    case USCI_I2C_UCTXIFG1:         // Transmit buffer empty in slave mode on address 1
    case USCI_I2C_UCTXIFG0:         // Transmit buffer empty in slave mode on address 0
      if (_wbuf.Empty()) {
        // no bytes currently on the write buffer, ask the user for some
        // expected that the user calls write() one or more times in onRequest
        _onRequest();
      }

      // only actually transmit when we have something to write
      if (!_wbuf.Empty()) {
        base->txbuf = _wbuf.Pop();
      }
    default:
      break;
  }
}

// interrupt handlers for USCI_B0-3
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void) {
  (&I2CBuses[0])->ISRHandler();
}

#pragma vector = USCI_B1_VECTOR
__interrupt void USCI_B1_ISR(void) {
  (&I2CBuses[1])->ISRHandler();
}

#pragma vector = USCI_B2_VECTOR
__interrupt void USCI_B2_ISR(void) {
  (&I2CBuses[2])->ISRHandler();
}

#pragma vector = USCI_B3_VECTOR
__interrupt void USCI_B3_ISR(void) {
  (&I2CBuses[3])->ISRHandler();
}

}// namespace EUSCI
