#include "utility.hpp"
#include "input.hpp"
#include "x11/application.hpp"
#include "x11/window.hpp"

using namespace gott::gui;
using namespace gott::gui::x11;
int main()
{
  Application app;
  pixelformat format;
  gott::gui::x11::Window a_window( app, rect(0,0,200,100),"Mein Titel", format, std::size_t(WindowFlags::Defaults) ); 

  app.run();
}

