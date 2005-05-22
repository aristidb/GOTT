#include <iostream>
#include <sstream>
#include <stdexcept>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <X11/extensions/xf86vmode.h>

#include "application.hpp"
#include "window.hpp"

using namespace std;


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

  init_cursor();

  try
  {
    pixelformat format;
    rect region(0,0,1,1);
    Window win( *this, region, "glx-test-window", format, std::size_t(WindowFlags::Defaults) );
    win.set_render_context();
    init_extensions();
    handle_pending_messages();
  }
  catch( std::runtime_error & e)
  {
    std::cout << e.what() << std::endl;
  }

}

void Application::handle_pending_messages()
{
}

int Application::get_screen() const
{ 
  return screen;
}

void Application::init_extensions() 
{
{
    if( glGetString( GL_EXTENSIONS ) == 0 )
      throw runtime_error("screew you"); 
    istringstream in( reinterpret_cast<const char*>(glGetString( GL_EXTENSIONS ) ) );
    copy( istream_iterator<string>(in), istream_iterator<string>(), 
        insert_iterator<set<string> >( extensions, extensions.begin() ) ); 
  }{
    istringstream in( glXQueryExtensionsString( display, screen ) );
    copy( istream_iterator<string>(in), istream_iterator<string>(), 
        insert_iterator<set<string> >( extensions, extensions.begin() ) ); 
  }

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

bool Application::is_extension_supported( std::string const& str ) const
{
  return extensions.find(str) != extensions.end();
}

void Application::init_cursor()
{
	char 		data = 0;
	XColor 		dummy;
	Pixmap 		blank_pixmap = XCreateBitmapFromData( display,
		RootWindow( display, screen ), &data, 1, 1);
  if( blank_pixmap == None )
    throw std::runtime_error("Could not create Bitmap from data");

	blank_cursor = XCreatePixmapCursor( display, blank_pixmap,
								blank_pixmap, &dummy, &dummy, 0, 0);
	
	XFreePixmap( display, blank_pixmap );
}


}}}
