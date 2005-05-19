#include "x11application.hpp"


Application::Application()
  : display( XOpenDisplay(0) )
{
  if( display == 0 )
    throw std::runtime_error("Could not open default x11 connection");

  screen = DefaultScreen( display );

  protocols_atom = XInternAtom( connection, "WM_PROTOCOLS", false );
  if( protocols_atom == None )
    throw std::runtime_error("Could not create atoms");


  int major, minor;
  if( false == glXQueryVersion( connection, &major, &minor ) )
    throw std::runtime_error("Could not query glXVersion");
	
	old_glx == ( major <= 1 ) && ( minor < 3 );


}
