#ifndef COMMAND_PROCESSOR_H_
#define COMMAND_PROCESSOR_H_

#include <Arduino.h>
#include <AltSoftSerial.h>
#include "PacketReceiver.h"

// Command IDs
enum class CommandID {
  Invalid = 0x00,
  Reset = 0x01,
  BeginLog = 0x02,
  EndLog = 0x04,
  Message = 0x69,
};

class CommandProcessor {
 public:
  // Initialize telemetry to use the given serial stream and packet sender
  CommandProcessor(AltSoftSerial& serial, uint32_t sync);

  // Update the command processor
  void Tick();
  
  // Bind the given function to the command with the given ID
  // Command must accept a data byte buffer and buffer length, and return whether that command was successful
  void Bind(CommandID cmd, bool (*cmdCallback)(const char *, uint16_t));

  // Call the function bound to the given command, returning true on success and false on failure
  bool Dispatch(CommandID cmd, const char *data, uint16_t dataLen);
 private:
  AltSoftSerial& _serial;

  
  char _rcvbuf[128];            // buffer containing data received from packets (without sync, length, id, etc.)
  PacketReceiver _receiver;     // internal packet receiver
  
  bool (*_resetCommand)(const char *data, uint16_t len);      // reset command callback
  bool (*_beginLogCommand)(const char *data, uint16_t len);   // begin logging command callback
  bool (*_endLogCommand)(const char *data, uint16_t len);     // end logging command callback
  bool (*_messageCommand)(const char *data, uint16_t len);    // message command callback
  bool (*_invalidCommand)(const char *data, uint16_t len);    // invalid command callback
};
#endif
