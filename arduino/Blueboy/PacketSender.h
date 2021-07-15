#ifndef SEND_PACKET_H_
#define SEND_PACKET_H_

#include <Arduino.h>
#include <SoftwareSerial.h>

class PacketSender {
 public:
  // Initialize a packet sender to use the given external buffer
  PacketSender(char *buf, uint32_t sync): _buf(buf), _sync(sync), _len(0), _off(0), _id(0) { }

  // Prepare to send a new packet with the given ID
  void Begin(uint8_t id);
  
  // Add a buffer of len bytes to the packet, returning the number of bytes added
  int AddBuf(const char *addbuf, int len);
  
  // Add a byte to the packet, returning the number of bytes added
  int AddByte(uint8_t addbyte);
  
  // Add a short to the packet, returning the number of bytes added
  int AddShort(uint16_t addshort);
  
  // Add a long to the packet, returning the number of bytes added
  int AddLong(uint32_t addlong);
  
  // Add a float to the packet, returning the number of bytes added
  int AddFloat(float addfloat);
  
  // Add a null-terminated string to the packet, returning the number of bytes added
  int AddStr(const char *str);
  
  // Send the built packet over serial, with the proper ID and length
  int Send(SoftwareSerial& serial);
 private:
  char *    _buf;
  uint32_t  _sync;
  uint16_t  _len;
  uint16_t  _off;
  uint8_t   _id;
};

#endif
