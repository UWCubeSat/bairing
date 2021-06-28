#ifndef SEND_PACKET_H_
#define SEND_PACKET_H_

class SendPacket {
 public:
  SendPacket(char *buf, uint8_t id);
  int addBuf(const char *addbuf, int len);
  int addByte(uint8_t addbyte);
  int addShort(uint16_t addshort);
  int addLong(uint32_t addlong);
  int addFloat(float addfloat);
  int addStr(const char *str, int len);
  int send(SoftwareSerial serial);
 private:
  char *buf;
  uint16_t len;
  uint16_t off;
  uint8_t id;
};

SendPacket::SendPacket(char *buf, uint8_t id) {
  this->buf = buf;
  this->id = id;
  this->len = 0;
  this->off = sizeof(this->len);

  this->addByte(id);
}

int SendPacket::addBuf(const char *addbuf, int len) {
  for (int i = 0; i < len; i++) {
    this->buf[this->off + i] = addbuf[i];
  }
  this->off += len;
  this->len += len;
  return len;
}

int SendPacket::addByte(uint8_t addbyte) {
  return this->addBuf((char *) &addbyte, sizeof(uint8_t));
}

int SendPacket::addShort(uint16_t addshort) {
  return this->addBuf((char *) &addshort, sizeof(uint16_t));
}

int SendPacket::addLong(uint32_t addlong) {
  return this->addBuf((char *) &addlong, sizeof(uint32_t));
}

int SendPacket::addFloat(float addfloat) {
  return this->addBuf((char *) &addfloat, sizeof(float));
}

int SendPacket::addStr(const char *str, int len) {
  int added = this->addBuf(str, len);
  added += this->addByte('\0');
  return added;
}

// writes length to buffer at the start, sends full packet over given serial
int SendPacket::send(SoftwareSerial serial) {
  *((uint16_t *) this->buf) = this->len;
  return serial.write(this->buf, sizeof(this->len) + this->len);
}

#endif
