/*
 * A proof of concept for a test system mounted to Blueboy, abiding by the current iteration of the protocol
 */

#include <msp430plus.h>
#include <cstdint>

#include "eUSCI/i2c.h"
#include "BlueboyReceiver.h"

int main(void) {
  WDTCTL = WDTPW | WDTHOLD;         // stop watchdog timer
  blueboy::InitializeBlueboyInterface();

  for (;;) { }
}
