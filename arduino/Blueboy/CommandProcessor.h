#ifndef COMMAND_PROCESSOR_H_
#define COMMAND_PROCESSOR_H_

#include <stdint.h>

// Command IDs
enum class CommandID {
  Reset = 0x01,
  BeginLog = 0x02,
  EndLog = 0x04,
  Message = 0x69,
};

constexpr bool NOOPCMD (const char *data, uint16_t len) { return false; }  // no-op command, default before binding

class CommandProcessor {
 public:
  CommandProcessor(): _resetCommand(&NOOPCMD), _beginLogCommand(&NOOPCMD), _endLogCommand(&NOOPCMD), _messageCommand(&NOOPCMD) {}
  
  // Binds the given function to the command with the given ID
  // Command must accept a data byte buffer and buffer length, and return whether that command was successful
  void Bind(CommandID cmd, bool (*cmdCallback)(const char *, uint16_t));

  // Calls the function bound to the given command, returning true on success and false on failure
  bool Dispatch(CommandID cmd, const char *data, uint16_t dataLen);
 private:
  bool (*_resetCommand)(const char *data, uint16_t len);      // reset command callback
  bool (*_beginLogCommand)(const char *data, uint16_t len);   // begin logging command callback
  bool (*_endLogCommand)(const char *data, uint16_t len);     // end logging command callback
  bool (*_messageCommand)(const char *data, uint16_t len);    // message command callback
};
#endif
