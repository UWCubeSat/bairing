#include "PacketReceiver.h"
#include "hardwareSerial.h"

constexpr bool DEBUG = false;

void PacketReceiver::Begin() {
  this->_offset = 0;
  this->_plen = 0;
  this->_id = 0;
  this->_mode = Syncing;

  if (DEBUG) Serial.println("Beginning packet receive");
}

// adds the given byte to the buffer, returning true if adding the byte
// completed the packet and false otherwise
bool PacketReceiver::AddByte(uint8_t readbyte) {
  if (this->_toRead == 0 && this->_mode != Syncing) {
    return false;
  }
  switch (this->_mode) {
    case Syncing:
      this->_pattern = ((uint32_t) readbyte << 24) | (this->_pattern >> 8);

      if (DEBUG) Serial.print("Syncing: ");
      if (DEBUG) Serial.println(this->_pattern, HEX);
      
      if (this->_pattern == this->_sync) {
        this->_mode = Length;
        this->_toRead = sizeof(this->_plen);
        if (DEBUG) Serial.println("Sync found, moving to Length");
      }
      break;
    case Length:
      this->_plen = ((uint16_t) readbyte << 8) | (this->_plen >> 8);
      this->_toRead--;

      if (DEBUG) Serial.print("Length: ");
      if (DEBUG) Serial.println(this->_plen, HEX);
      
      if (this->_toRead == 0) {
        if (this->_plen == 0) {
          // invalid packet length, go back go back
          this->_mode = Syncing;
          
          if (DEBUG) Serial.println("Received a packet length of 0, bad packet");
        } else {
          // we just added the last length field byte, move to data on next cycle
          this->_toRead = this->_plen;
          this->_mode = ID;
  
          if (DEBUG) Serial.println("Length complete, moving to ID");
        }
      }
      break;
    case ID:
      this->_id = readbyte;
      this->_toRead--;
      if (this->_toRead == 0) {
        // we received a packet with no data, we're done
        if (DEBUG) Serial.println("Packet with no data detected, complete");
        return true;
      } else {
        if (DEBUG) Serial.println("ID complete, moving to Data");
        this->_mode = Data;
      }
      break;
    case Data:
      this->_dataBuf[this->_offset++] = readbyte;
      this->_toRead--;

      if (DEBUG) Serial.print("Data: ");
      if (DEBUG) Serial.println(readbyte, HEX);
      
      if (this->_toRead == 0) {
        // just added last data byte, we're done
        if (DEBUG) Serial.println("Data complete, packet complete");
        return true;
      }
      break;
    default:
      break;
  }
  return false;
}

bool PacketReceiver::Completed() {
  return this->_toRead == 0;
}

uint16_t PacketReceiver::GetPacketDataLength() {
  return this->_plen - sizeof(this->_id);  // packet length minus id field
}

uint8_t PacketReceiver::GetPacketID() {
  return this->_id;
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
