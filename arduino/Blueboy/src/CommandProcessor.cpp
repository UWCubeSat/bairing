/*!
 * @file CommandProcessor.cpp
 * @author Sebastian S.
 * @brief Implementation of CommandProcessor.h
 */

#include "CommandProcessor.h"

/*!
 * @fn NOOPCMD
 * Definition of a no-op command callback, to be used as the "default" command behavior before any binding.
 */
constexpr bool NOOPCMD (CommandID cmd, const char *data, uint16_t len) { return false; }

CommandProcessor::CommandProcessor(AltSoftSerial& serial, uint32_t sync) : _serial(serial), _receiver(PacketReceiver(_rcvbuf, sync)) {
  _reset =               &NOOPCMD;
  _echo =                &NOOPCMD;
  _beginOwnAttitude =    &NOOPCMD;
  _endOwnAttitude =      &NOOPCMD;
  _beginTestAttitude =   &NOOPCMD;
  _endTestAttitude =     &NOOPCMD;
  _invalid =             &NOOPCMD;
  _beginCalibrate =      &NOOPCMD;
  _endCalibrate =        &NOOPCMD;
  _clearCalibrate =      &NOOPCMD;
  _receiver.Begin();
}

void CommandProcessor::Bind(CommandID cmd, CommandCallback cmdCallback) {
  switch(cmd) {
    case CommandID::Reset:
      _reset = cmdCallback;
      break;

    case CommandID::Echo:
      _echo = cmdCallback;
      break;

    case CommandID::BeginOwnAttitude:
      _beginOwnAttitude = cmdCallback;
      break;

    case CommandID::EndOwnAttitude:
      _endOwnAttitude = cmdCallback;
      break;

    case CommandID::BeginTestAttitude:
      _beginTestAttitude = cmdCallback;
      break;

    case CommandID::EndTestAttitude:
      _endTestAttitude = cmdCallback;
      break;
    
    case CommandID::BeginCalibMag:
    case CommandID::BeginCalibAcc:
    case CommandID::BeginCalibGyro:
      _beginCalibrate = cmdCallback;
      break;

    case CommandID::EndCalibMag:
    case CommandID::EndCalibAcc:
    case CommandID::EndCalibGyro:
      _endCalibrate = cmdCallback;
      break;
    
    case CommandID::ClearCalibMag:
    case CommandID::ClearCalibAcc:
    case CommandID::ClearCalibGyro:
      _clearCalibrate = cmdCallback;

    case CommandID::Invalid:
    default:
      _invalid = cmdCallback;
      break;
  }
}

bool CommandProcessor::Dispatch(CommandID cmd, const char *data, uint16_t dataLen) {
  switch(cmd) {
    case CommandID::Reset:
      return _reset(cmd, data, dataLen);
      break;

    case CommandID::Echo:
      return _echo(cmd, data, dataLen);
      break;

    case CommandID::BeginOwnAttitude:
      return _beginOwnAttitude(cmd, data, dataLen);
      break;

    case CommandID::EndOwnAttitude:
      return _endOwnAttitude(cmd, data, dataLen);
      break;

    case CommandID::BeginTestAttitude:
      return _beginTestAttitude(cmd, data, dataLen);
      break;

    case CommandID::EndTestAttitude:
      return _endTestAttitude(cmd, data, dataLen);
      break;
    
    case CommandID::BeginCalibMag:
    case CommandID::BeginCalibAcc:
    case CommandID::BeginCalibGyro:
      return _beginCalibrate(cmd, data, dataLen);
      break;

    case CommandID::EndCalibMag:
    case CommandID::EndCalibAcc:
    case CommandID::EndCalibGyro:
      return _endCalibrate(cmd, data, dataLen);
      break;
    
    case CommandID::ClearCalibMag:
    case CommandID::ClearCalibAcc:
    case CommandID::ClearCalibGyro:
      return _clearCalibrate(cmd, data, dataLen);

    case CommandID::Invalid:
    default:
      return _invalid(cmd, data, dataLen);
      break;
  }
}

void CommandProcessor::Tick() {
  while (_serial.available()) {
    // read a single byte at a time, add it to a packet being built
    char readbyte = _serial.read();
    if (_receiver.AddByte(readbyte)) {
      // a full packet was just received
      _receiver.PrintPacketInfo();
      
      CommandID cmd = (CommandID) _receiver.GetPacketID();
      const char *data = _receiver.GetPacketData();
      uint16_t datalen = _receiver.GetPacketDataLength();
      Dispatch(cmd, data, datalen);
      
      _receiver.Begin();  // restart packet receiver
    }
  }
}
