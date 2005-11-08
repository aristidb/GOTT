#include <cstdlib>
#include <iostream>
#include <ctime>
#include <boost/bind.hpp>
#include <gott/ui/window_base.hpp>
#include <gott/ui/x11/window.hpp>
#include <gott/ui/x11/uicontext.hpp>
#include <gott/util/string/string.hpp>
#include <gott/base/events/select_loop.hpp>

using namespace gott::ui::x11;
using namespace gott::events;
using namespace boost::posix_time;

deadline_timer resize_window( window *win ) {
  std::cout <<"Got Fired" << std::endl;
  return deadline_timer( microsec_clock::local_time() + seconds(10), boost::bind( &resize_window, win) );
}


int main(int, char **){
  std::srand(std::time(0));
  select_loop loop;
  uicontext con;
  {
    window win( con, gott::rect( 30,30,123,230), gott::string("simple_x11.cpp title"), size_t(gott::ui::window_flags::Defaults) );
    loop.add_read_fd( con.get_descriptor(), boost::bind( &uicontext::process_read, &con ) );
    loop.add_timer( deadline_timer( microsec_clock::local_time() + seconds(10), boost::bind( &resize_window, &win ) ) );
    loop.run();
  }
}
