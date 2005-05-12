#include <iostream>
#include "application.hpp"

namespace gott{ namespace gui{

application::application()
{
}
  
void application::run()
{
  while( handle_pending_messages() == Continue )
  {
    handle_idle();
  }
}

void application::handle_idle()
{
  process_idle();
}

application& application::get_instance()
{
  static application instance;
  return instance;
}

void application::add_window( window * win )
{
  std::cout << "Adding window:" << win<<std::endl; 
  windows.push_back( win );
}

void application::remove_window( window * win )
{
  window_iterator it = std::find(windows.begin(), windows.end(), win );
  if( it != windows.end() )
    windows.erase(it);
  std::cout << "Errasing window:" << win<<std::endl; 
}
 
application::window_iterator application::begin() {return windows.begin();}
application::window_iterator application::end() {return windows.end();}

}}
