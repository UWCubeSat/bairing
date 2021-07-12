#include <msp430plus.h>
#include <cstdint>

#include "eUSCI/i2c.h"

using eusci::I2CBus;
using eusci::I2CMode;

const I2CBus::Handle BUS = I2CBus::B1;

/*
// I2C buffered receive callback, invoked whenever an entire buffer has been received
void onReceiveBytes(char *buf, int bufsize) {
  // Debugging to ensure all bytes are properly received
  volatile int starts = eusci::STARTS;
  volatile int stops = eusci::STOPS;
  volatile int reads = eusci::READS;
  volatile int error = 0;

  if (bufsize != 16) {
    error = bufsize;
  } else {
    for (int i = 0; i < 16; i++) {
      if (buf[i] != i) {
        error = buf[i];
      }
    }
  }
}
*/

void onReceiveBytes(int bufsize) {
  volatile int error = 0;
  volatile int starts = eusci::STARTS;
  volatile int stops = eusci::STOPS;
  volatile int reads = eusci::READS;
  I2CBus *bus = eusci::GetI2C(BUS);
  if (bufsize != bus->Available()) {
    error = bus->Available();
  }

  if (bus->Available() != 16) {
    error = bus->Available();
  }

  char i = 0;
  char read;
  while (bus->Available()) {
    read = bus->Read();
    if (read != i) {
      error = read;
    }
    i++;
  }
}

void onRequestI2C() {
  I2CBus *bus = eusci::GetI2C(BUS);
}

int main(void) {
  volatile int test = 0;
	WDTCTL = WDTPW | WDTHOLD;	        // stop watchdog timer

	I2CBus *bus = eusci::GetI2C(BUS);
	bus->Begin(0x3A);

	bus->BindReceiveCallback(&onReceiveBytes);

	for (;;) { }
}
