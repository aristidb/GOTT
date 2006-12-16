#include <iostream>
#include <queue>
#include <boost/thread.hpp>

using namespace boost;
using namespace std;

class mqueue {
public:
  void put(int val) {
    mutex::scoped_lock lock(mut);
    msg.push(val);
    not_empty.notify_one();
  }
  
  int get() {
    mutex::scoped_lock lock(mut);
    while (msg.empty())
      not_empty.wait(lock);
    int ret = msg.front();
    msg.pop();
    return ret;
  }

  bool empty() const {
    mutex::scoped_lock lock(mut);
    return msg.empty();
  }

private:
  queue<int> msg;

private:
  mutable mutex mut;
  condition not_empty;
};

mqueue stuff;

void do_consume() {
  int val;
  while ((val = stuff.get()) != 42)
    cout << val << endl;
}

void do_produce() {
  int val;
  while (cin >> val) 
    stuff.put(val);
  stuff.put(42);
}

int main() {
  thread consumer(&do_consume);
  thread producer(&do_produce);
  consumer.join();
  producer.join();
}
