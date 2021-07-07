/*
 * msp430plus.h
 */

#ifndef __MSP430PLUS_H_
#define __MSP430PLUS_H_

#if defined (__MSP430FR5994__)
#include "msp430fr5994plus.h"

/********************************************************************
 *  msp430 generic
 ********************************************************************/
#elif defined (__MSP430GENERIC__)
#error "msp430 generic device does not have a default include file"

#elif defined (__MSP430XGENERIC__)
#error "msp430X generic device does not have a default include file"

/********************************************************************
 *
 ********************************************************************/
#else
#error "Failed to match a default include file"
#endif

#endif /* LIB_MSP430PLUS_MSP430PLUS_H_ */
