#include <gott/plugin.hpp>
#include <gott/events/main_loop.hpp>
#include <gott/events/timer_manager.hpp>
#include <gott/events/quit_manager.cpp>
#include <gott/string/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <iostream>

using namespace boost::posix_time;
using namespace gott::events;
using namespace gott::plugin;

static bool print_message(gott::string const &message) {
  std::cout << message << std::endl;
  return true;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " DURATION [MESSAGE]" << std::endl;
    return 1;
  }
  time_duration duration(duration_from_string(argv[1]));
  gott::string message;
  if (argc == 2)
    message = gott::string("yes", gott::ascii);
  else
    message = gott::string(argv[2], gott::ascii);
  std::cerr << "Will print \"" << message << 
    "\" after every " << duration << std::endl;

  plugin_handle<main_loop> loop(
    with_feature<timer_manager>() && with_feature<quit_manager>());
  loop->feature<quit_manager>().enable_master();

  loop->feature<timer_manager>().add_periodic_timer(
    duration,
    boost::bind(&print_message, message));
  loop->run();
}
