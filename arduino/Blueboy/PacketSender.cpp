#include "PacketSender.h"

void PacketSender::Begin(uint8_t id) {
  _id = id;
  _len = 0;
  _off = sizeof(_sync) + sizeof(_len);

  AddByte(id);
}

int PacketSender::AddBuf(const char *addbuf, int len) {
  for (int i = 0; i < len; i++) {
    _buf[_off + i] = addbuf[i];
  }
  _off += len;
  _len += len;
  return len;
}

int PacketSender::AddByte(uint8_t addbyte) {
  return AddBuf((char *) &addbyte, sizeof(uint8_t));
}

int PacketSender::AddShort(uint16_t addshort) {
  return AddBuf((char *) &addshort, sizeof(uint16_t));
}

int PacketSender::AddLong(uint32_t addlong) {
  return AddBuf((char *) &addlong, sizeof(uint32_t));
}

int PacketSender::AddFloat(float addfloat) {
  return AddBuf((char *) &addfloat, sizeof(float));
}

int PacketSender::AddStr(const char *str) {
  int added = AddBuf(str, strlen(str));
  added += AddByte('\0');
  return added;
}

// writes length to buffer at the start, sends full packet over given serial
int PacketSender::Send(SoftwareSerial& serial) {
  *((uint32_t *) _buf) = _sync;  // add sync pattern
  
  *((uint16_t *) (_buf + sizeof(_sync))) = _len;  // add length
  
  return serial.write(_buf, sizeof(_sync) + sizeof(_len) + _len);
}
