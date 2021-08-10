/*
 * BlueboyReceiver.h
 */

#ifndef SRC_BLUEBOYRECEIVER_H_
#define SRC_BLUEBOYRECEIVER_H_

#include <BlueboyInterface.h>
#include "eUSCI/i2c.h"

using eusci::I2CBus;

namespace blueboy {

constexpr I2CBus::Handle BLUEBOY_BUS = I2CBus::B1;

void InitializeBlueboyInterface();
void OnBlueboyReceive(int bufsize);
void OnBlueboyRequest();

}  // namespace blueboy

#endif /* SRC_BLUEBOYRECEIVER_H_ */
