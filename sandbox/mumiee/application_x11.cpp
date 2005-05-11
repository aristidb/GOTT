#include <X11/Xlib.h>
#include <boost/cstdint.hpp>
#include <iostream>
#include "application.hpp"
#include "globals_x11.hpp"

namespace gott{namespace gui{
  
application::status application::handle_pending_messages()
{
  while( XPending( global_data.connection ) > 0 )
  {
    XEvent					event;
    XNextEvent( global_data.connection, &event );

    window * win = global_data.decode_window_object( event.xany.window );

    if( win== 0 )
    {
      std::cerr << "Unable to decode window" << std::endl;
      break;
    }

    // handle this message
    global_data.process_event( win, event ); // RETURN VALUE?
  }
  
  XSync( global_data.connection, 1 );
  return windows.empty() ? End : Continue; 
}




void application::quit()
{
}


}}

