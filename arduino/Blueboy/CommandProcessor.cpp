#include "CommandProcessor.h"

// no-op command, used as the default behavior for commands before being bound
constexpr bool NOOPCMD (const char *data, uint16_t len) { return false; }

CommandProcessor::CommandProcessor(SoftwareSerial& serial, PacketReceiver& receiver) : _serial(serial), _receiver(receiver) {
  _resetCommand =     &NOOPCMD;
  _beginLogCommand =  &NOOPCMD;
  _endLogCommand =    &NOOPCMD;
  _messageCommand =   &NOOPCMD;
  _invalidCommand =   &NOOPCMD;
  
  _receiver.Begin();
}

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
    case CommandID::Invalid:
    default:
      _invalidCommand = cmdCallback;
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
    case CommandID::Invalid:
    default:
      dataLen = (uint16_t) cmd;
      return _invalidCommand(data, dataLen);
      break;
  }
}

void CommandProcessor::Tick() {
  while (_serial.available()) {
    char readbyte = _serial.read();
    if (_receiver.AddByte(readbyte)) {
      _receiver.PrintPacketInfo();
      
      CommandID command = (CommandID) _receiver.GetPacketID();
      const char *data = _receiver.GetPacketData();
      uint16_t datalen = _receiver.GetPacketDataLength();
      Dispatch(command, data, datalen);
      
      _receiver.Begin();  // restart packet receiver
    }
  }
}
