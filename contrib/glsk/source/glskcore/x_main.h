
/*
**
**		filename:		x_main.h
**
**		description:	main header
**		target-system:	X-Windows
**		version-info:   $Id$
**
*/


/*

	Context Structure
	
*/

struct glsk_context_s
{
	GLXContext		handle;
	int				refcount;
};

/*

	Window Structure

*/

struct glsk_window_s
{
	Window			handle;	
	GLXWindow		drawable;
	
	glsk_context_t*	context;

	int				width;
	int				height;
	int				uwidth;
	int				uheight;
	
	int				px;
	int				py;
	
	int				flags;
	
	char*			title;

	void*			userdata;
	
	struct
	{
		glsk_callback_window_pixelformat_t	pixelformat_select;
		glsk_callback_window_string_t		char_event;
		glsk_callback_window_coord_t		configure_event;
		glsk_callback_window_voidp_t		redraw_event;
		glsk_callback_window_coord_t		create_event;
		glsk_callback_window_voidp_t		destroy_event;
		glsk_callback_window_voidp_t		close_event;
		glsk_callback_window_mouse_event_t	mouse_event;
	}
	callback;
	
	glsk_window_t*	next;
	glsk_window_t*	prev;
};

#define GLSK_WINDOW_VISIBLE				1
#define GLSK_WINDOW_HAS_DECORATION		2
#define GLSK_WINDOW_FULLSCREEN			4
#define GLSK_WINDOW_OPEN				8

/*

	Idle Handler

	Note: the system can handle an arbitary number of idle functions using this linked list

*/


struct glsk_idle_handler_s
{
	glsk_idle_handler_t*	prev;
	glsk_idle_handler_t*	next;

	glsk_callback_voidp_t	function;
	void*					context;
};


/*

	Mouse Event Structure

	Note: mouse event keeping coordinates, type and buttons

*/

struct glsk_mouse_event_s
{
	int						px;
	int						py;
	int						type;
	int						button;
};



/*

	Globals

*/

typedef struct glsk_global_s
{
	Display*				connection;
	int						screen;
	Cursor					blank_cursor;
	Atom					object_atom;
	Atom					protocols_atom;
	Atom					delete_atom;
	int						flags;
	
	XF86VidModeModeInfo 	desktop_video_mode;

	glsk_idle_handler_t*	idle_head;
	glsk_window_t*			window_head;
	
	glsk_window_t*			focus_window;
	
	int						glx_version[ 2 ];
	int						glx_fallback_mode;
		
	char*					extensions_buffer;
	int						extensions_count;
	char**					extensions;
	
	int						open_window_count;
}
glsk_global_t;

extern glsk_global_t global;

//#define GLSK_MAIN_QUIT 						1
#define GLSK_MAIN_DECODE_ERROR				2
#define GLSK_MAIN_VIDMODE_CHANGED			4

#define TRUE 1
#define FALSE 0


int 					_glsk_init() __attribute__((constructor));
void					_glsk_free() __attribute__((destructor));


void					_glsk_init_cursor();
int						_glsk_process_event( glsk_window_t* window, XEvent* event );
