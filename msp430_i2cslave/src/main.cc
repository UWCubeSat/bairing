#include <msp430plus.h>
#include <cstdint>

#include "eUSCI/i2c.h"

// own address as I2C slave
constexpr uint16_t SLAVE_ADDR = 0x12;

void gpioInit()
{
    // I2C pins
    // UCB1 SDA: 5.0
    // UCB1 SCL: 5.1
    // Primary module function (SEL1 = 0, SEL0 = 1)
    // See Table 9-31 in msp430fr599x datasheet
    P5SEL0 |= BIT0 | BIT1;
    P5SEL1 &= ~(BIT0 | BIT1);

    // disable high-impedance mode to activate configured port settings
    PM5CTL0 &= ~LOCKLPM5;
}

void i2cInit() {
    UCB1CTLW0 = UCSWRST;                // reset, begin configuration

    UCB1CTLW0 |= UCMODE_3 | UCSYNC;     // i2c, slave mode (UCMST = 0)
    UCB1I2COA0 = UCOAEN | SLAVE_ADDR;   // set own address

    UCB1CTLW0 &= ~UCSWRST;              // clear reset, enable UCB
    UCB1IE |= UCRXIE;

    _enable_interrupts();
}

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	        // stop watchdog timer

	gpioInit();
	i2cInit();

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
