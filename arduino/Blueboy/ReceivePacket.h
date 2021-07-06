#ifndef RECEIVE_PACKET_H_
#define RECEIVE_PACKET_H_

class ReceivePacket {
 public:
  ReceivePacket(char *buf): _dataBuf(buf), _plen(0), _id(0), _toRead(0), _offset(0), _mode(Length) { }
  void Begin(uint16_t off = 0);
  bool AddByte(uint8_t byte);
  bool Completed();
  uint16_t GetPacketDataLength();
  uint8_t GetPacketID();
 private:
  enum ReadMode { Length, ID, Data };
  ReadMode  _mode;
  
  char *    _dataBuf;     // data buf
  uint16_t  _plen;        // packet length
  uint8_t   _id;
  uint16_t  _toRead;
  uint16_t  _offset;
};

void ReceivePacket::Begin(uint16_t off) {
  this->_offset = off;
  this->_plen = 0;
  this->_id = 0;
  this->_toRead = sizeof(this->_plen);
  this->_mode = Length;
}

// adds the given byte to the buffer, returning true if adding the byte
// completed the packet and false otherwise
bool ReceivePacket::AddByte(uint8_t readbyte) {
  if (this->_toRead == 0) {
    return false;
  }
  switch (this->_mode) {
    case Length:
      this->_plen = ((uint16_t) readbyte << 8) | (this->_plen >> 8);
      this->_toRead--;
      
      if (this->_toRead == 0) {
        // we just added the last length field byte, move to data on next cycle
        this->_toRead = this->_plen;
        this->_mode = ID;
      }
      break;
    case ID:
      this->_id = readbyte;
      this->_toRead--;
      if (this->_toRead == 0) {
        // we received a packet with no data, we're done
        return true;
      } else {
        this->_mode = Data;
      }
      break;
    case Data:
      this->_dataBuf[this->_offset++] = readbyte;
      this->_toRead--;
      if (this->_toRead == 0) {
        // just added last data byte, we're done
        return true;
      }
      break;
    default:
      break;
  }
  return false;
}

bool ReceivePacket::Completed() {
  return this->_toRead == 0;
}

uint16_t ReceivePacket::GetPacketDataLength() {
  return this->_plen - sizeof(this->_id);  // packet length minus id field
}

uint8_t ReceivePacket::GetPacketID() {
  return this->_id;
}

#endif
