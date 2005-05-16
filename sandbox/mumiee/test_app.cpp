#include "application.hpp"
#include "window.hpp"
#include "gl_context.hpp"
#include <iostream>

void redraw() { std::cout << "redraw" << std::endl; }
void size() { std::cout << "size" << std::endl; }
int main()
{
  using namespace std;
  using namespace gott::gui;
  cout << "a window" << endl;
  gl_context con;
  pixelformat format;
  window a_window( con, format, window::Decoration | window::Defaults | window::KeyEvents | window::MouseEvents | window::Visible ) ;
  cout << "a window created" << endl;
  
  application::get_instance().run();
  cout << "run done!" << endl;
}
