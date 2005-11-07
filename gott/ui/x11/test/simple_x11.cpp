
#include <boost/bind.hpp>
#include <gott/ui/window_base.hpp>
#include <gott/ui/x11/window.hpp>
#include <gott/ui/x11/uicontext.hpp>
#include <gott/util/string/string.hpp>
#include <gott/base/events/select_loop.hpp>

using namespace gott::ui::x11;
using namespace gott::events;

int main(int, char **){
  select_loop loop;
  uicontext con;
  {
    window win( con, gott::rect( 30,30,123,230), gott::string("simple_x11.cpp title"), size_t(gott::ui::window_flags::Defaults) );
    loop.add_read_fd( con.get_descriptor(), boost::bind( &uicontext::process_read, &con ) );
    loop.run();
  }
}
