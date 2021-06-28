#ifndef RECEIVE_PACKET_H_
#define RECEIVE_PACKET_H_

class ReceivePacket {
 public:
  ReceivePacket(char *buf): dataBuf(buf), plen(0), id(0), toRead(0), offset(0), mode(Length) { }
  void begin(uint16_t off = 0);
  bool addByte(uint8_t byte);
  bool completed();
  uint16_t getPacketDataLength();
  uint8_t getPacketID();
 private:
  enum ReadMode { Length, ID, Data };
  ReadMode mode;
  
  char *dataBuf;        // data buf
  uint16_t plen;        // packet length
  uint8_t id;
  uint16_t toRead;
  uint16_t offset;
};

void ReceivePacket::begin(uint16_t off) {
  this->offset = off;
  this->plen = 0;
  this->id = 0;
  this->toRead = sizeof(this->plen);
  this->mode = Length;
}

// adds the given byte to the buffer, returning true if adding the byte
// completed the packet and false otherwise
bool ReceivePacket::addByte(uint8_t readbyte) {
  if (this->toRead == 0) {
    return false;
  }
  switch (this->mode) {
    case Length:
      this->plen = ((uint16_t) readbyte << 8) | (this->plen >> 8);
      this->toRead--;
      
      if (this->toRead == 0) {
        // we just added the last length field byte, move to data on next cycle
        this->toRead = this->plen;
        this->mode = ID;
      }
      break;
    case ID:
      this->id = readbyte;
      this->toRead--;
      this->mode = Data;
      break;
    case Data:
      this->dataBuf[this->offset++] = readbyte;
      this->toRead--;
      if (this->toRead == 0) {
        // just added last data byte, we're done
        return true;
      }
      break;
    default:
      break;
  }
  return false;
}

bool ReceivePacket::completed() {
  return this->toRead == 0;
}

uint16_t ReceivePacket::getPacketDataLength() {
  return this->plen - sizeof(this->id);  // packet length minus id field
}

uint8_t ReceivePacket::getPacketID() {
  return this->id;
}

#endif

/* TESTING CODE
char buf[256];
ReceivePacket buffer = ReceivePacket(buf);

void setup() {
  Serial.begin(9600);
  buffer.begin();
}

void loop() {
  if (Serial.available()) {
    if (!buffer.completed()) {
      buffer.addByte(Serial.read());
    }
    if (buffer.completed()) {
      Serial.print("Length: ");
      Serial.println(buffer.len());

      Serial.write(buffer.data(), buffer.len());
      Serial.println();
      
      buffer.begin();
    }
  }
}
*/
