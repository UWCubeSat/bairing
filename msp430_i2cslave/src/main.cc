#include <msp430plus.h>
#include <cstdint>

#include "eUSCI/i2c.h"

using eusci::I2CBusHandle;

// i2c bus initialization parameters
struct I2CInitParameters {
  int pbase;
  int sdapin;
  int sclpin;
  uint8_t funcsel;
  I2CInitParameters(int base, int sda, int scl, uint8_t sel): pbase(base), sdapin(sda), sclpin(scl), funcsel(sel) { }
};

struct I2CInitParameters I2CPARAMS[4] = {
  I2CInitParameters(1, 6, 7, 0b10),  // B0, 1.6 and 1.7, secondary
  I2CInitParameters(5, 0, 1, 0b01),  // B1, 5.0 and 5.1, primary
  I2CInitParameters(7, 0, 1, 0b01),  // B2, 7.0 and 7.1, primary
  I2CInitParameters(6, 4, 5, 0b01),  // B3, 6.4 and 6.5, primary
};

void gpioInit(I2CBusHandle bus) {
    struct I2CInitParameters param = I2CPARAMS[(int) bus];
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

void i2cInit(I2CBusHandle bus, uint8_t address) {
    struct msp430::UCBx *i2cbus = msp430::UCB[(int) bus];
    i2cbus->ctlw0 = UCSWRST;                       // reset, begin configuration

    i2cbus->ctlw0 |= UCMODE_3 | UCSYNC;            // i2c, slave mode (UCMST = 0)
    i2cbus->i2coa0 = UCOAEN | (address & 0x7f);    // set own address

    i2cbus->ctlw0 &= ~UCSWRST;                     // clear reset, enable UCB
    i2cbus->ie |= UCRXIE;                          // enable interrupt on receive

    _enable_interrupts();                       // enable general interrupts
}

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	        // stop watchdog timer

	I2CBusHandle bus = I2CBusHandle::B1;

	gpioInit(bus);
	i2cInit(bus, 0x3A);

	for (;;) { }
}

#pragma vector = USCI_B1_VECTOR
__interrupt void USCI_B1_ISR(void) {
    volatile uint8_t rx_val;
    switch(__even_in_range(UCB1IV, USCI_I2C_UCBIT9IFG)) {
        case USCI_NONE:          break;         // Vector 0: No interrupts
        case USCI_I2C_UCALIFG:   break;         // Vector 2: ALIFG
        case USCI_I2C_UCNACKIFG: break;         // Vector 4: NACKIFG
        case USCI_I2C_UCSTTIFG:  break;         // Vector 6: STTIFG
        case USCI_I2C_UCSTPIFG:  break;         // Vector 8: STPIFG
        case USCI_I2C_UCRXIFG3:  break;         // Vector 10: RXIFG3
        case USCI_I2C_UCTXIFG3:  break;         // Vector 12: TXIFG3
        case USCI_I2C_UCRXIFG2:  break;         // Vector 14: RXIFG2
        case USCI_I2C_UCTXIFG2:  break;         // Vector 16: TXIFG2
        case USCI_I2C_UCRXIFG1:  break;         // Vector 18: RXIFG1
        case USCI_I2C_UCTXIFG1:  break;         // Vector 20: TXIFG1
        case USCI_I2C_UCRXIFG0:                 // Vector 22: RXIFG0
            // must read from RXBUF to continue
            rx_val = UCB1RXBUF;
            break;
        case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0
        default: break;
    }
}
