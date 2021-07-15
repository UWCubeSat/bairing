#include "CommandProcessor.h"

void CommandProcessor::Bind(CommandID cmd, bool (*cmdCallback)(const char *, uint16_t)) {
  switch(cmd) {
    case CommandID::Reset:
      _resetCommand = cmdCallback;
      break;
    case CommandID::BeginLog:
      _beginLogCommand = cmdCallback;
      break;
    case CommandID::EndLog:
      _endLogCommand = cmdCallback;
      break;
    case CommandID::Message:
      _messageCommand = cmdCallback;
      break;
    default:
      // invalid id
      break;
  }
}

bool CommandProcessor::Dispatch(CommandID cmd, const char *data, uint16_t dataLen) {
  switch(cmd) {
    case CommandID::Reset:
      return _resetCommand(data, dataLen);
    case CommandID::BeginLog:
      return _beginLogCommand(data, dataLen);
    case CommandID::EndLog:
      return _endLogCommand(data, dataLen);
    case CommandID::Message:
      return _messageCommand(data, dataLen);
    default:
      // invalid id
      break;
  }
}
