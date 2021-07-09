/*
 * eusci.h
 */

#ifndef SRC_EUSCI_EUSCI_H_
#define SRC_EUSCI_EUSCI_H_

#include <msp430plus.h>

namespace eusci {

constexpr int RWBUFLEN = 32;    // I2CBus read/write buffer lengths

// circular FIFO queue for eUSCI buses
class RWQueue {
 public:
  RWQueue(): _start(0), _size(0) { }
  void push(char byte);
  char pop();
  int size();
 private:
  char _buf[RWBUFLEN];
  int _start, _size;
};

extern RWQueue ReadQueues[4];   // read queues for each eusci bus
extern RWQueue WriteQueues[4];  // write queues for each eusci bus

}  // eusci

#endif /* SRC_EUSCI_EUSCI_H_ */
