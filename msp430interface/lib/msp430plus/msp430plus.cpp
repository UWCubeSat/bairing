/*
 * msp430fr5994plus.cc
 *
 */

#include <msp430plus.h>

namespace msp430 {

// if board has an EUSCI B module, initialize as many UCBx register bases as there are
// buses on the board
#ifdef __MSP430_HAS_EUSCI_Bx__
struct UCBx *volatile UCB[4] = {
  #ifdef __MSP430_HAS_EUSCI_B0__
  (struct UCBx *) EUSCI_B0_BASE, // UCB0
  #endif

  #ifdef __MSP430_HAS_EUSCI_B1__
  (struct UCBx *) EUSCI_B1_BASE, // UCB1
  #endif

  #ifdef __MSP430_HAS_EUSCI_B2__
  (struct UCBx *) EUSCI_B2_BASE, // UCB2
  #endif

  #ifdef __MSP430_HAS_EUSCI_B3__
  (struct UCBx *) EUSCI_B3_BASE, // UCB3
  #endif
};
#endif

struct GPIOPx *volatile GPIOP[10] = {
  (struct GPIOPx *) nullptr,  // P0 doesn't exist
  (struct GPIOPx *)  P1_BASE,
  (struct GPIOPx *) (P2_BASE + 0x1),  // offset by 1 byte
  (struct GPIOPx *)  P3_BASE,
  (struct GPIOPx *) (P4_BASE + 0x1),  // offset by 1 byte
  (struct GPIOPx *)  P5_BASE,
  (struct GPIOPx *) (P6_BASE + 0x1),  // offset by 1 byte
  (struct GPIOPx *)  P7_BASE,
  (struct GPIOPx *) (P8_BASE + 0x1),  // offset by 1 byte
  (struct GPIOPx *)  P9_BASE
};

}
