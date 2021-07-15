#include "PacketSender.h"

void PacketSender::Begin(uint8_t id) {
  this->_id = id;
  this->_len = 0;
  this->_off = sizeof(this->_len);

  this->AddByte(id);
}

int PacketSender::AddBuf(const char *addbuf, int len) {
  for (int i = 0; i < len; i++) {
    this->_buf[this->_off + i] = addbuf[i];
  }
  this->_off += len;
  this->_len += len;
  return len;
}

int PacketSender::AddByte(uint8_t addbyte) {
  return this->AddBuf((char *) &addbyte, sizeof(uint8_t));
}

int PacketSender::AddShort(uint16_t addshort) {
  return this->AddBuf((char *) &addshort, sizeof(uint16_t));
}

int PacketSender::AddLong(uint32_t addlong) {
  return this->AddBuf((char *) &addlong, sizeof(uint32_t));
}

int PacketSender::AddFloat(float addfloat) {
  return this->AddBuf((char *) &addfloat, sizeof(float));
}

int PacketSender::AddStr(const char *str) {
  int added = this->AddBuf(str, strlen(str));
  added += this->AddByte('\0');
  return added;
}

// writes length to buffer at the start, sends full packet over given serial
int PacketSender::Send(SoftwareSerial serial) {
  *((uint16_t *) this->_buf) = this->_len;
  return serial.write(this->_buf, sizeof(this->_len) + this->_len);
}
