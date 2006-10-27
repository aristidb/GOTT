#include <iostream>

#include <boost/parameter/keyword.hpp>
#include <boost/parameter/parameters.hpp>
#include <boost/parameter/macros.hpp>

namespace parameter = boost::parameter;

void open_window_impl(char const *title, int x = 0, int y = 0, 
    int dx = 100, int dy = 100) {
  std::cout << "Window titled \"" << title << "\" at (" << x << ',' << y << 
    ") with size (" << dx << ',' << dy << ")\n";
}

namespace {
BOOST_PARAMETER_KEYWORD(tag, title)
BOOST_PARAMETER_KEYWORD(tag, x_pos)
BOOST_PARAMETER_KEYWORD(tag, y_pos)
BOOST_PARAMETER_KEYWORD(tag, x_size)
BOOST_PARAMETER_KEYWORD(tag, y_size)
}

typedef parameter::parameters<
    tag::title,
    tag::x_pos,
    tag::y_pos,
    tag::x_size,
    tag::y_size
  > open_window_params;

BOOST_PARAMETER_FUN(void, open_window, 1, 5, open_window_params);

template<class T>
void open_window_with_named_params(T const &args) {
  open_window_impl(args[title], args[x_pos | 0], args[y_pos | 0],
      args[x_size | 100], args[y_size | 100]);
}

class window {
public:
  BOOST_PARAMETER_MEMFUN(void, open, 1, 5, open_window_params);

private:
  void open_impl(char const *t, int x, int y, int dx, int dy) {
    open_window(title = t, x_pos = x, y_pos = y, x_size = dx, y_size = dy);
  }
};

template<class T>
void window::open_with_named_params(T const &args) {
  open_window_impl(args[title], args[x_pos | 0], args[y_pos | 0],
      args[x_size | 100], args[y_size | 100]);
}

int main() {
  std::cout << "Simple boost::parameters test - to get the feeling...\n";
  open_window(x_pos = 10, title = "Dumme Sau!");
  window w;
  w.open("Andres Fenster", 15, 15, y_size = 400);
}
