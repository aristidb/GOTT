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

int Application::get_screen() const
{ 
  return screen;
}

void Application::register_window( Window * ref )
{
}

void Application::remove_window( Window *ref )
{
}

Display* Application::get_display()
{
  return display;
}

bool Application::use_fallback()const
{
  return old_glx;
}

Atom Application::get_atom( char const * atom_name )
{
  return XInternAtom( display, atom_name, 0 );
}


}}}
