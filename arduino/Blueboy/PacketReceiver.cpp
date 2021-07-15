#include "PacketReceiver.h"
#include "hardwareSerial.h"

constexpr bool DEBUG = false;

void PacketReceiver::Begin() {
  _offset = 0;
  _plen = 0;
  _id = 0;
  _mode = Syncing;

  if (DEBUG) Serial.println("Beginning packet receive");
}

// adds the given byte to the buffer, returning true if adding the byte
// completed the packet and false otherwise
bool PacketReceiver::AddByte(uint8_t readbyte) {
  if (_toRead == 0 && _mode != Syncing) {
    return false;
  }
  switch (_mode) {
    case Syncing:
      _pattern = ((uint32_t) readbyte << 24) | (_pattern >> 8);

      if (DEBUG) Serial.print("Syncing: ");
      if (DEBUG) Serial.println(_pattern, HEX);
      
      if (_pattern == _sync) {
        _mode = Length;
        _toRead = sizeof(_plen);
        if (DEBUG) Serial.println("Sync found, moving to Length");
      }
      break;
    case Length:
      _plen = ((uint16_t) readbyte << 8) | (_plen >> 8);
      _toRead--;

      if (DEBUG) Serial.print("Length: ");
      if (DEBUG) Serial.println(_plen, HEX);
      
      if (_toRead == 0) {
        if (_plen == 0) {
          // invalid packet length, go back go back
          _mode = Syncing;
          
          if (DEBUG) Serial.println("Received a packet length of 0, bad packet");
        } else {
          // we just added the last length field byte, move to data on next cycle
          _toRead = _plen;
          _mode = ID;
  
          if (DEBUG) Serial.println("Length complete, moving to ID");
        }
      }
      break;
    case ID:
      _id = readbyte;
      _toRead--;
      if (_toRead == 0) {
        // we received a packet with just an ID and no data, we're done
        if (DEBUG) Serial.println("Packet with no data detected, complete");
        return true;
      } else {
        if (DEBUG) Serial.println("ID complete, moving to Data");
        _mode = Data;
      }
      break;
    case Data:
      _dataBuf[_offset++] = readbyte;
      _toRead--;

      if (DEBUG) Serial.print("Data: ");
      if (DEBUG) Serial.println(readbyte, HEX);
      
      if (_toRead == 0) {
        // just added last data byte, we're done
        if (DEBUG) Serial.println("Data complete, packet complete");
        return true;
      }
      break;
    default:
      // this shouldn't happen
      break;
  }
  return false;
}

bool PacketReceiver::Completed() {
  return _toRead == 0;
}

const char *PacketReceiver::GetPacketData() {
  return _dataBuf;
}

uint16_t PacketReceiver::GetPacketDataLength() {
  return _plen - sizeof(_id);  // packet length minus id field
}

uint8_t PacketReceiver::GetPacketID() {
  return _id;
}

void PacketReceiver::PrintPacketInfo() {
  int datalen = GetPacketDataLength();
  
  Serial.println("Packet received");
  
  Serial.print("  ID: ");
  Serial.println(_id);
  
  Serial.print("  Data length: ");
  Serial.println(datalen);
  
  Serial.print("  Received data: ");
  if (datalen == 0) {
    Serial.println("None");
  } else {
    for (int i = 0; i < _plen - sizeof(_id); i++) {
      Serial.print(_dataBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}
