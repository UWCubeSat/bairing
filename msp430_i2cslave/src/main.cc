/*
 * A proof of concept for a test system mounted to Blueboy, abiding by the current iteration of the protocol
 */

#include <msp430plus.h>
#include <cstdint>

#include "eUSCI/i2c.h"
#include "BlueboyReceiver.h"

using eusci::I2CBus;
using eusci::I2CMode;
using namespace blueboy;

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	        // stop watchdog timer

	I2CBus *bus = eusci::GetI2C(BLUEBOY_BUS);
	bus->Begin(0x3A);

	bus->OnReceive(&OnBlueboyReceive);
	bus->OnRequest(&OnBlueboyRequest);

	for (;;) { }
}
