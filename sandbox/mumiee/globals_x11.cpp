#include <sys/time.h>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <boost/cstdint.hpp>
#include "globals_x11.hpp"
#include "pixelformat.hpp"
#include "window.hpp"

using namespace std;

namespace gott{namespace gui{
globals global_data;

bool globals::is_extension_supported( std::string const& str )
{
  return extensions.find(str) != extensions.end();
}

void globals::init_cursor()
{
	char 		data = 0;
	XColor 		dummy;
	Pixmap 		blank_pixmap = XCreateBitmapFromData( connection,
		RootWindow( connection, screen ), &data, 1, 1);
  if( blank_pixmap == None )
    throw std::runtime_error("Could not create Bitmap from data");

	blank_cursor = XCreatePixmapCursor( connection, blank_pixmap,
								blank_pixmap, &dummy, &dummy, 0, 0);
	
	XFreePixmap ( connection, blank_pixmap );
}

void globals::set_property( window const& w, Atom a, std::string const& utf8string)
{
}

globals::globals( )
  : connection(None), screen(0), flags(Clear), focus_window(0)
{
  XF86VidModeModeInfo** 	video_modes = 0;
	int						video_mode_count = 0;
	
	// grab our current connection to the server
	connection = XOpenDisplay( 0 );
  if( connection == 0 )
    throw std::runtime_error("Could not open connection");
	
	// grab the default screen for this workstation
  screen = DefaultScreen( connection );

	// create an atom for windows to store the glsk_window_t pointer
	delete_atom = XInternAtom( connection, "GOTT_GUI_DELETE_WINDOW", false );
	
	protocols_atom = XInternAtom( connection, "WM_PROTOCOLS", false );
  if( protocols_atom == None )
    throw std::runtime_error("Could not create atoms");

	if( false == glXQueryVersion( connection, &(glx_version[ 0 ]), &(glx_version[ 1 ]) ) )
    throw std::runtime_error("Could not query glXVersion");
	
	if ( ( glx_version[ 0 ] <= 1 ) && ( glx_version[ 1 ] < 3 ) )
		glx_fallback_mode = 1;
	else
		glx_fallback_mode = 0;

  std::cout << "GLX-Version " << glx_version[0] << "."<< glx_version[1] << std::endl;
	

	init_cursor();
	
	// get all available modes
	if( false == XF86VidModeGetAllModeLines( connection, screen, &video_mode_count, &video_modes ) )
    throw std::runtime_error("Could not query available Video Modes");

	// save desktop-resolution
	desktop_video_mode = *(video_modes[ 0 ]);
	
	// free those modes
	XFree( video_modes );
	
	init_input();

	init_gl_interface();

  try
  {
    gl_context con;
    pixelformat format;
  std::cout << "GLX-Version " << glx_version[0] << "."<< glx_version[1] << std::endl;
    window win( con, format, std::size_t(window::Decoration| window::Open | window::Visible) );
  std::cout << "GLX-Version " << glx_version[0] << "."<< glx_version[1] << std::endl;
    win.set_rendercontext();
  std::cout << "GLX-Version " << glx_version[0] << "."<< glx_version[1] << std::endl;
    std::cout << "window openend" << std::endl;
  std::cout << "GLX-Version " << glx_version[0] << "."<< glx_version[1] << std::endl;
    // FIXME!
    init_extensions();
    application::get_instance().handle_pending_messages();
  std::cout << "GLX-Version " << glx_version[0] << "."<< glx_version[1] << std::endl;
    std::cout << "closing test window" << std::endl;
  }
  catch( std::runtime_error & e)
  {
    std::cout << e.what() << std::endl;
  }

  init_timer();
}

void globals::init_gl_interface()
{
  
}

void globals::init_extensions()
{
  {
    if( glGetString( GL_EXTENSIONS ) == 0 )
      throw runtime_error("screew you"); 
    istringstream in( reinterpret_cast<const char*>(glGetString( GL_EXTENSIONS ) ) );
    copy( istream_iterator<string>(in), istream_iterator<string>(), 
        insert_iterator<set<string> >( extensions, extensions.begin() ) ); 
  }{
    istringstream in( glXQueryExtensionsString( connection, screen ) );
    copy( istream_iterator<string>(in), istream_iterator<string>(), 
        insert_iterator<set<string> >( extensions, extensions.begin() ) ); 
  }
}


void globals::init_timer()
{
	gettimeofday( &timer_start, 0 );
}

void globals::init_input()
{
}

globals::~globals()
{
  reset_videomode();

  XCloseDisplay( global_data.connection );
  connection = None;
}

void globals::reset_videomode()
{
  if( flags & VideoModeChanged )
	{
		XF86VidModeSwitchToMode( connection, screen, &desktop_video_mode );
		XF86VidModeSetViewPort( connection, screen, 0, 0 );
		
		flags &= ~VideoModeChanged;
	}

}


namespace {

int decode_error_handler(Display* display, XErrorEvent* event)
{
  global_data.flags |= globals::DecodeError;
  return 1;
}

}

window * globals::decode_window_object( Window handle )
{
  /*
	window* result = 0;
	Atom return_type;
	int	return_format;
	unsigned long return_count;
	unsigned long return_rest;
	unsigned char* data = 0;
	
	
	XSetErrorHandler( decode_error_handler );
	
	if ( XGetWindowProperty( connection, handle,
				object_atom, 0, sizeof( window* )/sizeof(boost::int32_t), 0,
				object_atom, &return_type, &return_format, &return_count,
				&return_rest, &data ) == Success )
	{
    result = *(reinterpret_cast<window**>(data));
		XFree( data );			
	}
	else
	{
		result = 0;
	}
	
	XSetErrorHandler( 0 );
	*/

	if ( handle == None )
		return 0;

  for( application::window_iterator it = application::get_instance().begin(), e = application::get_instance().end() ;
      it != e; ++it )
    if ( (*it)->os->handle == handle )
      return *it;

	return 0;
}

}}
