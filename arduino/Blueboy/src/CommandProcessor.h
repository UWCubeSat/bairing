/*!
 * @file CommandProcessor.h
 * @author Sebastian S.
 * @brief Definition for the CommandProcessor class and adjacent utility types.
 */

#ifndef COMMAND_PROCESSOR_H_
#define COMMAND_PROCESSOR_H_

#include <Arduino.h>
#include <AltSoftSerial.h>
#include "util/PacketReceiver.h"
#include "Blueboy.h"

/*!
 * @typedef CommandCallback
 * Defines a function pointer type that accepts a command ID, data buffer, and buffer length for a command callback.
 */
typedef bool (*CommandCallback)(CommandID id, const char *data, uint16_t len);

/*!
 * @class CommandProcessor
 * @brief Reads bytes over a serial stream, recognizes commands, and dispatches them through bound callbacks.
 */
class CommandProcessor {
 public:
  /*!
   * @brief CommandProcessor constructor
   * @param serial Reference to the AltSoftSerial stream to read bytes from
   * @param sync Sync pattern in little endian to recognize as the start of a packet
   */
  CommandProcessor(AltSoftSerial& serial, uint32_t sync);

  /*!
   * @brief Updates the command processor
   */
  void Tick();
  
  /*!
   * @brief Binds the given function to the command with the given ID
   * @param cmd Command ID to bind the callback to
   * @param cmdCallback Callback to be invoked when cmd is received
   */
  void Bind(CommandID cmd, CommandCallback cmdCallback);

  /*!
   * @brief Calls the function bound to the given command
   * @param cmd Command ID to invoke the callback of
   * @param data Buffer of bytes to pass to the command callback as data
   * @param len Length of the data buffer
   * @return True iff the command callback successfully executed.
   */
  bool Dispatch(CommandID cmd, const char *data, uint16_t dataLen);
 private:
  AltSoftSerial& _serial;       // serial stream to read command bytes from
  
  char _rcvbuf[64];             // buffer containing data received from packets (without sync, length, id, etc.)
  PacketReceiver _receiver;     // internal packet receiver
  
  CommandCallback _reset;              // reset command callback
  CommandCallback _echo;               // message command callback
  CommandCallback _beginOwnAttitude;   // begin logging own attitude command callback
  CommandCallback _endOwnAttitude;     // end logging own attitude command callback
  CommandCallback _beginTestAttitude;  // begin logging test attitude command callback
  CommandCallback _endTestAttitude;    // end logging test attitude command callback
  CommandCallback _beginCalibrate;     // begin calibrating a sensor
  CommandCallback _endCalibrate;       // end calibrating a sensor
  CommandCallback _clearCalibrate;     // clear calibration data for a sensor
  CommandCallback _invalid;            // invalid command callback
};
#endif
