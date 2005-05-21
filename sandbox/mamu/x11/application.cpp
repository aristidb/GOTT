#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <X11/extensions/xf86vmode.h>

#include "application.hpp"


namespace gott{ namespace gui{ namespace x11{

Application::Application()
  : display( XOpenDisplay(0) )
{
  if( display == 0 )
    throw std::runtime_error("Could not open default x11 connection");

  screen = DefaultScreen( display );

  protocols_atom = XInternAtom( display, "WM_PROTOCOLS", false );
  if( protocols_atom == None )
    throw std::runtime_error("Could not create atoms");


  int major, minor;
  if( false == glXQueryVersion( display, &major, &minor ) )
    throw std::runtime_error("Could not query glXVersion");
	
	old_glx = ( major <= 1 ) && ( minor < 3 );
}

}}}
