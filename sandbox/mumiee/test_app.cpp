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
  window a_window( con, format, window::Decoration | window::Defaults | window::KeyEvents | window::MouseEvents | window::Visible, "The Main Window") ;
  cout << "a window created" << endl;
  window second_window( con, format, window::Decoration | window::Defaults | window::KeyEvents | window::MouseEvents | window::Visible, "A broken window with shared context \\o/" );
  //window third_window( con, format, window::Splash | window::KeyEvents | window::MouseEvents | window::Visible, "Splashscreen", &a_window) ;
  
  application::get_instance().run();
  cout << "run done!" << endl;
}
