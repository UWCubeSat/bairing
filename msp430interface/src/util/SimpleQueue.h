/*
 * SimpleQueue.h
 *
 * A simple FIFO queue template with push and pop functionality
 */

#ifndef SRC_UTIL_SIMPLEQUEUE_H_
#define SRC_UTIL_SIMPLEQUEUE_H_

namespace util {

template<class T>
class SimpleQueue {
public:
  SimpleQueue(): _start(0), _end(0), _size(0) { }
  bool Push(T item);
  T Pop();
  int Size();
  bool Empty();
private:
  constexpr static int INTERNALBUFLEN = 64;
  T _buf[INTERNALBUFLEN];         // internal buffer
  int _start, _end;               // start and end indices for circular queue
  int _size;
};

// pushes item to the end of the queue if below the internal limit
// returns true if the item was added, false otherwise
template <class T>
bool SimpleQueue<T>::Push(T item) {
  if (Size() < INTERNALBUFLEN) {
    _buf[_end] = item;
    _size++;
    _end++;
    if (_end >= INTERNALBUFLEN) {
      _end = 0;
    }
    return true;
  }
  return false;
}

// returns the element at the front of the queue
// precondition: size > 0
template <class T>
T SimpleQueue<T>::Pop() {
  T item = _buf[_start];
  if (!Empty()) {
    _size--;
    _start++;
    if (_start >= INTERNALBUFLEN) {
      _start = 0;
    }
  }
  return item;
}

// returns the current size of the queue
template <class T>
int SimpleQueue<T>::Size() {
  return _size;
}

// returns true iff the queue has 0 objects
template <class T>
bool SimpleQueue<T>::Empty() {
  return Size() <= 0;
}

}  // namespace util

#endif /* SRC_UTIL_SIMPLEQUEUE_H_ */
