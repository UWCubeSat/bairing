#ifndef SEND_PACKET_H_
#define SEND_PACKET_H_

#include <SoftwareSerial.h>
#include <stdint.h>

class PacketSender {
 public:
  // Initializes a packet sender to use the given external buffer
  PacketSender(char *buf): _buf(buf), _len(0), _off(0), _id(0) { }

  // Prepares to send a new packet with the given ID
  void Begin(uint8_t id);
  
  // Adds a buffer of len bytes to the packet, returning the number of bytes added
  int AddBuf(const char *addbuf, int len);
  
  // Adds a byte to the packet, returning the number of bytes added
  int AddByte(uint8_t addbyte);
  
  // Adds a short to the packet, returning the number of bytes added
  int AddShort(uint16_t addshort);
  
  // Adds a long to the packet, returning the number of bytes added
  int AddLong(uint32_t addlong);
  
  // Adds a float to the packet, returning the number of bytes added
  int AddFloat(float addfloat);
  
  // Adds a null-terminated string to the packet, returning the number of bytes added
  int AddStr(const char *str);
  
  // Sends the built packet over serial, with the proper ID and length
  int Send(SoftwareSerial serial);
 private:
  char *    _buf;
  uint16_t  _len;
  uint16_t  _off;
  uint8_t   _id;
};

#endif
