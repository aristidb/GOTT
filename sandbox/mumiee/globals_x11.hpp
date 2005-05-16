
#ifndef GOTT_GUI_GLOBALS_X11_HPP_INCLUDED
#define GOTT_GUI_GLOBALS_X11_HPP_INCLUDED

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <X11/extensions/xf86vmode.h>
#include <set>
#include <string>
#include "gl_context.hpp"
#include "window.hpp"

namespace gott{namespace gui{

class window;

namespace impl{
struct key_table
{
  unsigned char t[2][256];
  key_table();
  key_code translate_key( KeySym const& sym ) const;
};
}


struct globals
{
  Display* connection;
	int	screen;
	Cursor blank_cursor;
	Atom protocols_atom, delete_atom;
  impl::key_table keys;

  enum Flags{
    Clear = 0
    , DecodeError = 1
    , VideoModeChanged = 2
  };
  std::size_t flags;
	
	XF86VidModeModeInfo 	desktop_video_mode;

	window*			focus_window;
	
	int	glx_version[ 2 ];
	int	glx_fallback_mode;
	
  bool is_extension_supported( std::string const& ext );
  std::set<std::string> extensions;
	
	struct timeval timer_start;
	
	//glsk_opengl_interface_t	gl;

  globals(  );
  ~globals();


  void init_timer();
  void init_input();
  void init_cursor();
  void init_extensions();
  void init_gl_interface();
  void reset_videomode();
  window * decode_window_object( Window handle );

  void process_event( window * win, XEvent & event );

  inline Atom get_atom( const char * prop ) { return XInternAtom(connection, prop, false ); }

  void set_property( window const& w, Atom a, std::string const& utf8string );
  void get_property( window const& w, Atom a, std::string& utf8string );

};
extern globals global_data;

// other special structures:
struct gl_context::os_specific
{
  os_specific();
  ~os_specific();
  GLXContext handle;
};

struct window::os_specific
{
  enum Protocols { DeleteWindow, Focus, Ping, ContextHelp };
  os_specific();
  ~os_specific();

  Window			handle;	
	GLXWindow		drawable;
  Atom protocols[4];
  Atom wm_name, wm_icon_name, wm_type;
};

}}

#endif

