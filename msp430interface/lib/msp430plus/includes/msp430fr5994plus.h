/*
 * msp430fr5994plus.h
 *
 * An expansion to the included msp430fr5994.h; bundling certain registers into
 * more convenient structures
 */

#ifndef SRC_MSP430FR5994PLUS_H_
#define SRC_MSP430FR5994PLUS_H_

#include <msp430fr5994.h>
#include <cinttypes>

namespace msp430 {

// generates padding of n bytes; with id as an arbitrary unique
// identifier for each instance of padding per namespace
// intended to be used to align bytes in register structs
#define __PADDING(id, n) char __PAD_##id[n]

// represents a block of UCBx registers; starting at some base address

struct UCBx {
  uint16_t ctlw0;    // (0x0000)
  uint16_t ctlw1;    // (0x0002)
  __PADDING(0, 0x2);
  uint16_t brw;      // (0x0006)
  uint16_t statw;    // (0x0008)
  uint16_t tbcnt;    // (0x000A)
  uint16_t rxbuf;    // (0x000C)
  uint16_t txbuf;    // (0x000E)
  __PADDING(1, 0x4);
  uint16_t i2coa0;   // (0x0014)
  uint16_t i2coa1;   // (0x0016)
  uint16_t i2coa2;   // (0x0018)
  uint16_t i2coa3;   // (0x001A)
  uint16_t addrx;    // (0x001C)
  uint16_t addmask;  // (0x001E)
  uint16_t i2csa;    // (0x0020)
  __PADDING(2, 0x8);
  uint16_t ie;       // (0x002A)
  uint16_t ifg;      // (0x002C)
  uint16_t iv;       // (0x002E)
};

// array of pointers to volatile UCB register bases
extern struct UCBx *volatile UCB[4];

// represents an odd-numbered gpio port base
struct GPIOPx {
  uint8_t in;        // (0x0000)
  __PADDING(0, 0x1);
  uint8_t out;       // (0x0002)
  __PADDING(1, 0x1);
  uint8_t dir;       // (0x0004)
  __PADDING(2, 0x1);
  uint8_t ren;       // (0x0006)
  __PADDING(3, 0x3);
  uint8_t sel0;      // (0x000A)
  __PADDING(4, 0x1);
  uint8_t sel1;      // (0x000C)
  __PADDING(5, 0x9);
  uint8_t selc;      // (0x0016)
  __PADDING(6, 0x1);
  uint8_t ies;       // (0x0018)
  __PADDING(7, 0x1);
  uint8_t ie;        // (0x001A)
  __PADDING(8, 0x1);
  uint8_t ifg;       // (0x001C)
};

/*
// represents an even-numbered gpio port base, each field offset by 1 byte
struct GPIOP_EVN {
  __PADDING(EVEN, 0x1);
  uint8_t in;        // (0x0001)
  __PADDING(0, 0x1);
  uint8_t out;       // (0x0003)
  __PADDING(1, 0x1);
  uint8_t dir;       // (0x0005)
  __PADDING(2, 0x1);
  uint8_t ren;       // (0x0007)
  __PADDING(3, 0x3);
  uint8_t sel0;      // (0x000B)
  __PADDING(4, 0x1);
  uint8_t sel1;      // (0x000D)
  __PADDING(5, 0x9);
  uint8_t selc;      // (0x0017)
  __PADDING(6, 0x1);
  uint8_t ies;       // (0x0019)
  __PADDING(7, 0x1);
  uint8_t ie;        // (0x001B)
  __PADDING(8, 0x1);
  uint8_t ifg;       // (0x001C)
};
*/

// array of pointers to volatile UCB register bases
extern struct GPIOPx *volatile GPIOP[10];

}  // namespace msp430

#endif /* SRC_MSP430FR5994PLUS_H_ */
