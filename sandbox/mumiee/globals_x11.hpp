
#ifndef GOTT_GUI_GLOBALS_X11_INCLUDED
#define GOTT_GUI_GLOBALS_X11_INCLUDED

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <X11/extensions/xf86vmode.h>
#include "gl_context.hpp"
#include "window.hpp"

namespace gott{namespace gui{

class window;
struct globals
{
  Display* connection;
	int	screen;
	Cursor blank_cursor;
	Atom object_atom;
	Atom protocols_atom;
	Atom delete_atom;

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
	char*	extensions_buffer;
	int extensions_count;
	char** extensions;
	
	struct timeval timer_start;
	
	//glsk_opengl_interface_t	gl;

  globals(  );
  ~globals();


  bool init_timer();
  bool init_input();
  bool init_cursor();
  bool init_extensions();
  void reset_videomode();
  window * decode_window_object( Window handle );

  void process_event( window * win, XEvent const& event );
};
extern globals global_data;

// other special structures:
struct gl_context::os_specific
{
  GLXContext handle;
};

struct window::os_specific
{
  Window			handle;	
	GLXWindow		drawable;
};



}}

#endif

