#include <gott/events/select_loop.hpp>
#include <gott/events/main_loop.hpp>
#include <gott/events/timer_manager.hpp>
#include <gott/events/quit_manager.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/if.hpp>
#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <signal.h>

using namespace gott::events;
using namespace boost::lambda;

const int width = 80;

void draw_bar(std::ostream &out, int pos) {
  const int width = 80;
  out << '|';
  for (int i = 0; i < width - 2; ++i)
    if (i == pos)
      out << '*';
    else
      std::cout << ' ';
  out << "|\r" << std::flush;
}

int main() {
  boost::scoped_ptr<main_loop> loop(new select_loop);
  int i = 0;
  int dx = 1;
  loop->feature<timer_manager>().add_timer(periodic_timer(
        boost::posix_time::milliseconds(100),
        (
         bind(&draw_bar, var(std::cout), var(i)),
         var(i) += var(dx),
         if_then((var(i) == 0 || var(i) == width - 2), var(dx) = -var(dx))
        )));
  loop->feature<quit_manager>().enable_master();
  loop->run();
  std::cout << std::endl;
}
