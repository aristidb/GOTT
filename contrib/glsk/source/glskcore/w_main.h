
/*
**
**		filename:		w_main.h
**
**		description:	global definitions
**		target-system:	microsoft windows
**		version-info:   $Id$
**
*/

#include <glsk/glsk.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glext.h>
#include "w_extensions.h"

/*

	Render Context

*/

struct glsk_context_s
{
	int				refcount;
	HGLRC			handle;
};


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

	Module-wide Flags

	Note:

*/


#define GLSK_GLOBAL_DEVMODE_CHANGED			1

/*

	Module-wide/Global Settings

	Note:

*/

typedef struct _glsk_globals_s
{
	HINSTANCE						hinstance;
	TCHAR*							appname;

	glsk_idle_handler_t*			idle_head;

	char*							extensions_buffer;
	char**							extensions;
	int								extensions_count;

	DEVMODE							previous_mode;

	int								flags;

#ifdef _DEBUG
#define DEBUG_BUFFER_SIZE 1024
	char							debug_buffer[ DEBUG_BUFFER_SIZE ];
#endif


	_glsk_extensions_t		function;

}
_glsk_globals_t;

extern _glsk_globals_t	global;	// this defines, but doesn't declare the globals settings

/*

	Internal Functions

*/

int					_glsk_module_init( HINSTANCE hinstance );
int					_glsk_module_free( HINSTANCE hinstance );

void				_glsk_handle_events();
void				_glsk_init_mouse_event( glsk_mouse_event_t* mouse_event, int px, int py, int type, int button );


/*

	Debug Code

*/

#ifdef _DEBUG

#include <stdio.h>

#define DEBUG_PRINT0( x ) _snprintf( global.debug_buffer, DEBUG_BUFFER_SIZE, (x) ); OutputDebugString( global.debug_buffer );
#define DEBUG_PRINT1( x, a ) _snprintf( global.debug_buffer, DEBUG_BUFFER_SIZE, (x), (a) ); OutputDebugString( global.debug_buffer );
#define DEBUG_PRINT2( x, a, b ) _snprintf( global.debug_buffer, DEBUG_BUFFER_SIZE, (x), (a), (b) ); OutputDebugString( global.debug_buffer );
#define DEBUG_PRINT3( x, a, b, c ) _snprintf( global.debug_buffer, DEBUG_BUFFER_SIZE, (x), (a), (b), (c) ); OutputDebugString( global.debug_buffer );

#else

#define DEBUG_PRINT0( x )
#define DEBUG_PRINT1( x, a )
#define DEBUG_PRINT2( x, a, b )
#define DEBUG_PRINT3( x, a, b, c )

#endif

