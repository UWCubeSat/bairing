/*!
 * @file main.cpp
 * @author Sebastian S.
 * @brief Main file for the Blueboy Interface proof of concept
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
