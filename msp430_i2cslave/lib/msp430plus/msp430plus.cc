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

}
