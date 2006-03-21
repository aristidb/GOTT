#include <gott/thread/message_queue.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include <iostream>

gott::thread::message_queue<int> element_transfer;

void producer() {
  std::ifstream infile("numbers.txt");
  if (!infile)
    infile.open("examples/numbers.txt");
  if (!infile)
    std::cerr << 
      "Could not find file numbers.txt or examples/numbers.txt, sorry.\n";
  int num;
  while (infile >> num) {
    if (num < 0)
      break;
    element_transfer.push(num);
  }
  element_transfer.push(-1);
}

void consumer() {
  int num;
  while ((num = element_transfer.pop()) >= 0)
    std::cout << (num * 2) << std::endl;
}

int main() {
  boost::thread producer_thread(&producer);
  boost::thread consumer_thread(&consumer);
  producer_thread.join();
  consumer_thread.join();
}
