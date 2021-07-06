#ifndef SEND_PACKET_H_
#define SEND_PACKET_H_

class SendPacket {
 public:
  SendPacket(char *buf, uint8_t id);
  int AddBuf(const char *addbuf, int len);
  int AddByte(uint8_t addbyte);
  int AddShort(uint16_t addshort);
  int AddLong(uint32_t addlong);
  int AddFloat(float addfloat);
  int AddStr(const char *str, int len);
  int Send(SoftwareSerial serial);
 private:
  char *    _buf;
  uint16_t  _len;
  uint16_t  _off;
  uint8_t   _id;
};

SendPacket::SendPacket(char *buf, uint8_t id) {
  this->_buf = buf;
  this->_id = id;
  this->_len = 0;
  this->_off = sizeof(this->_len);

  this->AddByte(id);
}

int SendPacket::AddBuf(const char *addbuf, int len) {
  for (int i = 0; i < len; i++) {
    this->_buf[this->_off + i] = addbuf[i];
  }
  this->_off += len;
  this->_len += len;
  return len;
}

int SendPacket::AddByte(uint8_t addbyte) {
  return this->AddBuf((char *) &addbyte, sizeof(uint8_t));
}

int SendPacket::AddShort(uint16_t addshort) {
  return this->AddBuf((char *) &addshort, sizeof(uint16_t));
}

int SendPacket::AddLong(uint32_t addlong) {
  return this->AddBuf((char *) &addlong, sizeof(uint32_t));
}

int SendPacket::AddFloat(float addfloat) {
  return this->AddBuf((char *) &addfloat, sizeof(float));
}

int SendPacket::AddStr(const char *str, int len) {
  int added = this->AddBuf(str, len);
  added += this->AddByte('\0');
  return added;
}

// writes length to buffer at the start, sends full packet over given serial
int SendPacket::Send(SoftwareSerial serial) {
  *((uint16_t *) this->_buf) = this->_len;
  return serial.write(this->_buf, sizeof(this->_len) + this->_len);
}

#endif
