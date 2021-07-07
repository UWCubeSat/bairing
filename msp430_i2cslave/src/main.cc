#include <msp430plus.h>
#include <cstdint>

#include "eUSCI/i2c.h"

const uint8_t I2C_ADDR = 0x12;

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	        // stop watchdog timer

	/*
    P1DIR |= BIT0;                      // pin 0 as output
	*/

	UCB0CTL1 |= UCSWRST;                // clear reset, enable UCB

    // UCB0CTLW0: UCSYNC = 1 and UCMST = 0 on reset
	UCB0CTLW0 |= UCMODE_3;              // i2c slave mode
	UCB0I2COA0 = UCOAEN | I2C_ADDR;     // set own address

	// UCB0 SDA: 1.6
	// UCB0 SCL: 1.7
	// Tertiary function
    P1SEL1 |= BIT6 | BIT7;
	P1SEL0 |= BIT6 | BIT7;
    PM5CTL0 &= ~LOCKLPM5;       // unlock PLM

    UCB0CTL1 &= ~UCSWRST;       // clear reset, enable UCB

    uint8_t lastRead = 0x00;

	for (;;) {
	    // P1OUT ^= BIT0;
	    // __delay_cycles(100000);

	    // while stop condition hasn't been reached
	    while (!(UCB0IFG & UCSTPIFG)) {
	        // spin while haven't received a byte
	        while (!(UCB0IFG & UCRXIFG0));

	        lastRead = UCB0RXBUF;
	    }
	}
	return 0;
}
