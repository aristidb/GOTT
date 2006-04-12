#include <gott/events/select_loop.hpp>
#include <gott/events/main_loop.hpp>
#include <gott/events/timer_manager.hpp>
#include <gott/events/quit_manager.hpp>
#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <ctime>

using namespace gott::events;

const int width = 60;

void draw_bar(int pos) {
  std::cout << "\r|";
  for (int i = 0; i <= width - 2; ++i)
    if (i == pos)
      std::cout << '*';
    else
      std::cout << ' ';
  std::cout << '|';// << std::flush;
}

class bar_drawer {
  int pos;
  int dx;
  std::clock_t last;
public:
  bar_drawer() : pos(0), dx(1), last(std::clock()) {}

  bool operator() () {
    draw_bar(pos);
    if (pos + dx == -1 || pos + dx == width - 1)
      dx = -dx;
    pos += dx;
    
    std::clock_t next = clock();
    std::cout << ' ';
    std::cout.width(6);
    std::cout << (next - last) << std::flush;
    last = next;

    return true;
  }
};

int main() {
  boost::scoped_ptr<main_loop> loop(new select_loop);
  loop->feature<timer_manager>().add_periodic_timer(
        boost::posix_time::milliseconds(80),
        bar_drawer(),
        true);
  loop->feature<quit_manager>().enable_master();
  loop->run();
  std::cout << std::endl;
}
