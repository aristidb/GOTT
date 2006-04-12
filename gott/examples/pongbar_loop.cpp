#include <gott/events/select_loop.hpp>
#include <gott/events/main_loop.hpp>
#include <gott/events/timer_manager.hpp>
#include <gott/events/quit_manager.hpp>
#include <boost/scoped_ptr.hpp>
#include <iostream>

using namespace gott::events;

const int width = 80;

void draw_bar(int pos) {
  std::cout << "\r|";
  for (int i = 0; i <= width - 2; ++i)
    if (i == pos)
      std::cout << '*';
    else
      std::cout << ' ';
  std::cout << "|\r" << std::flush;
}

class bar_drawer {
  int pos;
  int dx;
public:
  bar_drawer() : pos(0), dx(1) {}

  void operator() () {
    draw_bar(pos);
    if (pos + dx == -1 || pos + dx == width - 1)
      dx = -dx;
    pos += dx;
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
