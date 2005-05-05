
/*
**
**		filename:		w_window.h
**
**		description:	global definitions
**		target-system:	microsoft windows
**		version-info:   $Id$
**
*/


/*

	Window Flags

	Note: these flags are set or not set for each window

*/

#define GLSK_WINDOW_HAS_DECORATION		1		// this means the window has a border and a titlebar
#define GLSK_WINDOW_FULLSCREEN			2		// if the window is in fullscreen mode, the client area matches the screenarea
#define GLSK_WINDOW_IS_VISIBLE			4

/*

	Window Structure

	Note: the structure representing a window with a GL rendercontext

*/

struct glsk_window_s
{
	HWND			handle;			// handle to the physical window
	HDC				dc;				// the client device context
	//HGLRC			rc;				// the opengl rendering context
	glsk_context_t*	context;

	// event handling
	struct
	{
		glsk_callback_window_string_t		char_event;				// WM_CHAR
		glsk_callback_window_voidp_t		redraw_event;			// WM_PAINT
		glsk_callback_window_coord_t		create_event;			// WM_CREATE
		glsk_callback_window_coord_t		configure_event;		// WM_SIZE
		glsk_callback_window_voidp_t		close_event;			// WM_CLOSE
		glsk_callback_window_voidp_t		destroy_event;			// WM_DESTROY
		glsk_callback_window_pixelformat_t	pixelformat_select;		// called on pixelformat selection
		glsk_callback_window_mouse_event_t	mouse_event;			// mouse
	}
	callback;

	void*			userdata;		// general pointer reserved for external usage

	int				flags;			// the flags as described above

	int				width;			// dimension x
	int				height;			// dimension y

	// this is the size as set by the user - not the current size
	int				uwidth;			// dimension x
	int				uheight;		// dimension y

	// position
	int				px;				// x-position
	int				py;				// y-position

	char*			title;			// title shown in the titlebar
};


/*

	Internal Functions

*/



void				_glsk_register_class( HINSTANCE hinstance, TCHAR* appname );

int					_glsk_open_window( glsk_window_t* window, HINSTANCE hinstance, TCHAR* appname, glsk_context_t* context );



void				_glsk_window_translate_client_to_screen( glsk_window_t* window, RECT* rectangle );

// this uses windows' coord system with y+ -> down
void				_glsk_window_move_relative( glsk_window_t* window, int dx, int dy );
