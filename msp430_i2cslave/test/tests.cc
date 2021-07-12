/*
 * tests.cc
 */

#include <cassert>
#include <util/SimpleQueue.h>

// #define NDEBUG

using util::SimpleQueue;

void testSimpleQueue() {
  constexpr int bufferlength = 64;
  SimpleQueue<int> q;
  int i;

  assert(q.Empty());
  assert(q.Size() == 0);

  // simple add pop
  for(i = 0; i < 10; i++) {
    q.Push(i);
  }
  assert(q.Size() == 10);
  for (i = 0; i < 10; i++) {
    int s = q.Pop();
    assert(s == i);
  }
  assert(q.Empty());

  // push pop > internal buffer length
  for(i = 0; i < 100; i++) {
    q.Push(i);
  }
  assert(q.Size() == bufferlength);
  i = 0;
  while (!q.Empty()) {
    int s = q.Pop();
    if (s != i) {
      assert(false);
    }
    i++;
  }
  assert(i == bufferlength);
  assert(q.Empty());
}

void main() {
  testSimpleQueue();
  for (;;) { }
}
