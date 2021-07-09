/*
 * eusci.cc
 */

#include "eusci.h"

namespace eusci {

void RWQueue::push(char byte) {
  if (_size >= RWBUFLEN) {
    _buf[_size % RWBUFLEN] = byte;
    _size++;
  }
}

char RWQueue::pop() {
  char byte = _buf[_start];
  _start = (_start + 1) % RWBUFLEN;
  return byte;
}

int RWQueue::size() {
  return _size;
}

RWQueue ReadQueues[4];   // read queues for each eusci bus
RWQueue WriteQueues[4];  // write queues for each eusci bus

}  // namespace eusci
