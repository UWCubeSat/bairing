#ifndef PACKET_RECEIVER_H_
#define PACKET_RECEIVER_H_

#include <Arduino.h>

// Receives packets in the form 
// [Length: 2 bytes] [ID: 1 byte] [Data: Length - 1]
// Where Length is the byte size of the entire packet, excluding itself
class PacketReceiver {
 public:
  // Initializes a packet receiver to use the given external buffer, listening for the given sync pattern
  PacketReceiver(char *buf, uint32_t sync): _dataBuf(buf), _sync(sync),
                                            _pattern(0), _plen(0), _id(0),
                                            _toRead(0), _offset(0), _mode(Length) { }
  
  // Prepare to receive a new packet
  void Begin();
  
  // Add the given byte to the buffer, returning true if adding the byte
  // completed the packet and false otherwise
  bool AddByte(uint8_t byte);
  
  // Return true iff the current packet is completed; i.e, there are no more bytes to receive
  bool Completed();
  
  // Return the 2-byte data length, which is the length of the data itself excluding the id
  uint16_t GetPacketDataLength();
  
  // Return the ID byte
  uint8_t GetPacketID();

  // Print information about the current packet, whether one ahs been received, the id, length and data
  void PrintPacketInfo();
 private:
  enum ReadMode { Syncing, Length, ID, Data};
  ReadMode  _mode;

  uint32_t  _sync;        // sync pattern
  uint32_t  _pattern;     // last four bytes received
  
  char *    _dataBuf;     // data buf
  uint16_t  _plen;        // packet length
  uint8_t   _id;
  uint16_t  _toRead;
  uint16_t  _offset;
};

#endif
