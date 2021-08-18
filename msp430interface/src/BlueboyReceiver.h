/*!
 * @file BlueboyReceiver.h
 * @author Sebastian S.
 * @brief Bindings interacting with a BlueboyInterface as part of a proof of concept
 */

#ifndef SRC_BLUEBOYRECEIVER_H_
#define SRC_BLUEBOYRECEIVER_H_

#include <BlueboyInterface.h>
#include "eUSCI/i2c.h"

using eusci::I2CBus;

/*!
 * @namespace blueboy
 * Namespace containing constants, classes, and structs for the Blueboy interface
 */
namespace blueboy {

/*!
 * @var I2CBus::Handle BLUEBOY_BUS
 * eUSCI bus to interface with Blueboy over
 */
constexpr I2CBus::Handle BLUEBOY_BUS = I2CBus::B1;

/*!
 * @brief Initializes the Blueboy Interface, starting a connection over I2C
 */
void InitializeBlueboyInterface();

/*!
 * @brief I2C data receive callback
 */
void OnBlueboyReceive(int bufsize);

/*!
 * @brief I2C data request callback
 */
void OnBlueboyRequest();

}  // namespace blueboy

#endif /* SRC_BLUEBOYRECEIVER_H_ */
