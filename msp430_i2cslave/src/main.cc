/*
 * A test of the buffered I2C slave library, which receives some number of bytes from the master and
 * echoes them back in reverse order.
 *
 */

#include <msp430plus.h>
#include <cstdint>

#include "eUSCI/i2c.h"

using eusci::I2CBus;
using eusci::I2CMode;

const I2CBus::Handle BUS = I2CBus::B1;

char receiveBuffer[64];
int received;

// called whenever the slave has received a full message in read mode
void onReceiveBytes(int bufsize) {
  volatile int error = 0;
  volatile int starts = eusci::STARTS;
  volatile int stops = eusci::STOPS;
  volatile int reads = eusci::READS;

  I2CBus *bus = eusci::GetI2C(BUS);

  char i = 0;
  char read;
  received = 0;
  while (bus->Available()) {
    read = bus->Read();

    receiveBuffer[received] = read;
    received++;
    i++;
  }
}

// called whenever the master requests a message of data from the slave
void onRequestI2C() {
  I2CBus *bus = eusci::GetI2C(BUS);
  for (int i = received - 1; i >= 0; i--) {
    bus->Write(receiveBuffer[i]);
  }
}

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	        // stop watchdog timer

	I2CBus *bus = eusci::GetI2C(BUS);
	bus->Begin(0x3A);

	bus->OnReceive(&onReceiveBytes);
	bus->OnRequest(&onRequestI2C);

	for (;;) { }
}
