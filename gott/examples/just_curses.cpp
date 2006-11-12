// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:

#include <gott/ui/ncurses/uicontext.hpp>
#include <gott/ui/ncurses/window.hpp>
#include <gott/plugin.hpp>
#include <gott/events/main_loop.hpp>
#include <gott/events/fd_manager.hpp>
#include <gott/events/quit_manager.hpp>
#include <gott/string/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <iostream>

using namespace boost::posix_time;
using namespace gott::events;
using namespace gott::plugin;
using namespace gott::ui;

int main(int argc, char **argv) {
  plugin_handle<main_loop> loop(
    with_feature<fd_manager>() && with_feature<quit_manager>());
  loop->feature<quit_manager>().enable_master();

  gott::rect reg;
  {
    ncurses::uicontext context(*loop, 0);
    reg = context.region().get();
    ncurses::window w1(context, gott::rect(10,10,90,90), "10x10 -> w:90xh:90", window_flags::Defaults);
    ncurses::window w2(context, gott::rect(150,50,50,50), "150x50 -> w:50xh:50", window_flags::Defaults);

    loop->run();
  }
  std::cout << "Quit gracefully." << std::endl;
  std::cout << "Terminal: " << reg.width << " " << reg.height << std::endl; 
}
