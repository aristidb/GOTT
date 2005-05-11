#include <sys/time.h>
#include <iostream>
#include <stdexcept>
#include <boost/cstdint.hpp>
#include "globals_x11.hpp"
#include "pixelformat.hpp"
#include "window.hpp"

namespace gott{namespace gui{
globals global_data;

bool globals::is_extension_supported( std::string const& str )
{
  return true;
}

bool globals::init_cursor()
{
	char 		data = 0;
	XColor 		dummy;
	Pixmap 		blank_pixmap = XCreateBitmapFromData( connection,
		RootWindow( connection, screen ), &data, 1, 1);

	blank_cursor = XCreatePixmapCursor( connection, blank_pixmap,
								blank_pixmap, &dummy, &dummy, 0, 0);
	
	XFreePixmap ( connection, blank_pixmap );
}

globals::globals( )
{
  XF86VidModeModeInfo** 	video_modes = 0;
	int						video_mode_count = 0;
	
	// grab our current connection to the server
	connection = XOpenDisplay( 0 );
	
	// grab the default screen for this workstation
  screen = DefaultScreen( connection );

	// create an atom for windows to store the glsk_window_t pointer
	object_atom = XInternAtom( connection, "GOTT_GUI_OBJECT", false );
	delete_atom = XInternAtom( connection, "GOTT_GUI_DELETE_WINDOW", false );
	
	protocols_atom = XInternAtom( connection, "WM_PROTOCOLS", false );

	flags = Clear;

	glXQueryVersion( connection, &(glx_version[ 0 ]), &(glx_version[ 1 ]) );
	
	if ( ( glx_version[ 0 ] <= 1 ) && ( glx_version[ 1 ] < 3 ) )
		glx_fallback_mode = 1;
	else
		glx_fallback_mode = 0;
	
	focus_window = 0;

	init_cursor();
	
	// get all available modes
	XF86VidModeGetAllModeLines( connection, screen, &video_mode_count, &video_modes );

	// save desktop-resolution
	desktop_video_mode = *(video_modes[ 0 ]);
	
	// free those modes
	XFree( video_modes );
	
  std::cout << "intit_input" << std::endl;
	init_input();

	//init_gl_extensions( &gl );

  try
  {
    gl_context con;
    pixelformat format;
    window win( con, format, std::size_t(window::Decoration) );
    std::cout << "window openend" << std::endl;
    // FIXME!
    //_glsk_init_extensions_string();
    application::get_instance().handle_pending_messages();
    std::cout << "closing test window" << std::endl;
  }
  catch( std::runtime_error & e)
  {
    std::cout << e.what() << std::endl;
  }

  init_timer();
}


bool globals::init_timer()
{
	gettimeofday( &timer_start, 0 );
  return true;
}

bool globals::init_input()
{
}

bool globals::init_extensions()
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
	window* result = 0;
	Atom return_type;
	int	return_format;
	unsigned long return_count;
	unsigned long return_rest;
	unsigned char* data = 0;
	
	if ( handle == None )
		return 0;
	
	XSetErrorHandler( decode_error_handler );
	
	if ( XGetWindowProperty( connection, handle,
				object_atom, 0, sizeof( window* )/sizeof(boost::int32_t), 0,
				object_atom, &return_type, &return_format, &return_count,
				&return_rest, &data ) == Success )
	{
		// decode the object pointer
    result = *(reinterpret_cast<window**>(&data));
		XFree( data );			
	}
	else
	{
		result = 0;
	}
	
	XSetErrorHandler( 0 );
	
	if( flags & globals::DecodeError )
	{
		flags &= ~DecodeError;
	
    for( application::window_iterator it = application::get_instance().begin(), e = application::get_instance().end() ;
        it != e; ++it )
			if ( (*it)->os->handle == handle )
				return *it;
	}
	
	return result;
}

}}
