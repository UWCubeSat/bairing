#ifndef COMMAND_PROCESSOR_H_
#define COMMAND_PROCESSOR_H_

#include <Arduino.h>
#include <AltSoftSerial.h>
#include "PacketReceiver.h"

// Command IDs
enum class CommandID {
  Reset =       0x00,
  Echo =        0x01,
  
  BeginOwnAttitude =  0x10,
  EndOwnAttitude =    0x11,
  EndOwnAll =         0x1F,

  BeginTestAttitude = 0x20,
  EndTestAttitude =   0x21,
  EndTestAll =        0x2F,
  
  Invalid = 0xFF,
};

typedef bool (*CommandCallback)(CommandID id, const char *data, uint16_t len);

class CommandProcessor {
 public:
  // Initialize telemetry to use the given serial stream and packet sender
  CommandProcessor(AltSoftSerial& serial, uint32_t sync);

  // Update the command processor
  void Tick();
  
  // Bind the given function to the command with the given ID
  // Command must accept a data byte buffer and buffer length, and return whether that command was successful
  void Bind(CommandID cmd, CommandCallback cmdCallback);

  // Call the function bound to the given command, returning true on success and false on failure
  bool Dispatch(CommandID cmd, const char *data, uint16_t dataLen);
 private:
  AltSoftSerial& _serial;

  
  char _rcvbuf[128];            // buffer containing data received from packets (without sync, length, id, etc.)
  PacketReceiver _receiver;     // internal packet receiver
  
  CommandCallback _reset;              // reset command callback
  CommandCallback _echo;               // message command callback
  CommandCallback _beginOwnAttitude;   // begin logging own attitude command callback
  CommandCallback _endOwnAttitude;     // end logging own attitude command callback
  CommandCallback _beginTestAttitude;  // begin logging test attitude command callback
  CommandCallback _endTestAttitude;    // end logging test attitude command callback
  CommandCallback _invalid;            // invalid command callback
};
#endif
