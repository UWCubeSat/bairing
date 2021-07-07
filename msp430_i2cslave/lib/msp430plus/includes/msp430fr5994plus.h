/*
 * msp430fr5994plus.h
 *
 * An expansion to the included msp430fr5994.h; bundling certain registers into
 * more convenient structures
 */

#ifndef SRC_MSP430FR5994PLUS_H_
#define SRC_MSP430FR5994PLUS_H_

#include <msp430fr5994.h>
#include <stdint.h>

namespace msp430 {

// generates padding of n bytes; with id as an arbitrary unique
// identifier for each instance of padding per namespace
// intended to be used to align bytes in register structs
#define __PADDING(id, n) char __PAD_##id[n]

// represents a block of UCBx registers; starting at some base address

struct UCBx {
  uint16_t UCBxCTLW0;    // (0x0000)
  uint16_t UCBxCTLW1;    // (0x0002)
  __PADDING(0, 0x2);
  uint16_t UCBxBRW;      // (0x0006)
  uint16_t UCBxSTATW;    // (0x0008)
  uint16_t UCBxTBCNT;    // (0x000A)
  uint16_t UCBxRXBUF;    // (0x000C)
  uint16_t UCBxTXBUF;    // (0x000E)
  __PADDING(1, 0x4);
  uint16_t UCBxI2COA0;   // (0x0014)
  uint16_t UCBxI2COA1;   // (0x0016)
  uint16_t UCBxI2COA2;   // (0x0018)
  uint16_t UCBxI2COA3;   // (0x001A)
  uint16_t UCBxADDRX;    // (0x001C)
  uint16_t UCBxADDMASK;  // (0x001E)
  uint16_t UCBxI2CSA;    // (0x0020)
  __PADDING(2, 0x8);
  uint16_t UCBxIE;       // (0x002A)
  uint16_t UCBxIFG;      // (0x002C)
  uint16_t UCBxIV;       // (0x002E)
};

// array of pointers to volatile UCB register bases
/*
struct UCBx *volatile UCB[4] = {
  (struct UCBx *) EUSCI_B0_BASE, // UCB0
  (struct UCBx *) EUSCI_B1_BASE, // UCB1
  (struct UCBx *) EUSCI_B2_BASE, // UCB2
  (struct UCBx *) EUSCI_B3_BASE, // UCB3
};
*/

extern struct UCBx *volatile UCB[4];

}  // namespace msp430

#endif /* SRC_MSP430FR5994PLUS_H_ */
