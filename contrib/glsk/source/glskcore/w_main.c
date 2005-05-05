
/*
**
**		filename:		w_main.c
**
**		description:	mainloop and general functions
**		target-system:	microsoft windows
**		version-info:   $Id$
**
*/


// system headers
#include <windows.h>
#include <gl/gl.h>

// global glsk header
#include <glsk/glsk.h>

// internal glsk headers
#include "w_main.h"
#include "w_window.h"
#include "w_input.h"





// instanciate the globals
_glsk_globals_t	global;

glsk_context_t* glsk_context_new()
{
	glsk_context_t* result = (glsk_context_t*)malloc( sizeof( struct glsk_context_s ) );

	result->handle = NULL;
	result->refcount = 0;
	
	return result;
}

void glsk_context_free( glsk_context_t* context )
{
	free( context );
}

void _glsk_init_mouse_event( glsk_mouse_event_t* event, int px, int py, int type, int button )
{
	event->px = px; 
	event->py = py;
	event->type = type;
	event->button = button;
}


void _glsk_handle_events()
{
	MSG message;

	while ( PeekMessage( &message, NULL, 0, 0, PM_REMOVE ) )
	{
		if ( message.message == WM_QUIT )
			return;

		TranslateMessage( &message );
		DispatchMessage( &message );	
	}
}


int glsk_main_run()
{
	MSG message;

	for( ;; ) if ( PeekMessage( &message, NULL, 0, 0, PM_REMOVE ) )
	{
		if ( message.message == WM_QUIT )
			return (int)message.wParam;

		TranslateMessage( &message );
		DispatchMessage( &message );

		_glsk_input_update();
	}
	else if ( global.idle_head )
	{
		glsk_idle_handler_t* node = global.idle_head;
		// process idle handler

		_glsk_input_update();

		while ( node )
		{
			node->function( node->context );
			node = node->next;
		}	
	}

	return 0;
}


void glsk_main_quit()
{
	PostQuitMessage( 0 );
}



int	_glsk_module_init( HINSTANCE hinstance )
{
	HRESULT hresult = 0;

	// init the global vars
	global.hinstance = hinstance;
	global.appname = TEXT( "glsk-wnd-cls" );

	global.idle_head = 0;

	global.flags = 0;

	// save the old display setting
	EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &global.previous_mode );

	// register the window class
	_glsk_register_class( hinstance, global.appname );

	// init direct input
	if ( _glsk_input_init() == 0 )
		return 0;

	// init opengl extensions
	if ( _glsk_init_extensions() == 0 )
		return 0;

	return 1;
}



int	_glsk_module_free( HINSTANCE hinstance )
{
	glsk_idle_handler_t* node = 0;
	glsk_idle_handler_t* temp = 0;


	// shutdown the extensions subsystem
	_glsk_free_extensions();

	// shutdown the input subsystem
	_glsk_input_free();

	// TODO: shutdown and free all remaining windows

	if ( global.flags & GLSK_GLOBAL_DEVMODE_CHANGED ) // change it back
		ChangeDisplaySettings( &global.previous_mode, 0 );



	node = global.idle_head;

	while ( node != 0 )
	{
		temp = node;
		node = node->next;

		free( node );
	}

	UnregisterClass( global.appname, hinstance );

	return 1;
}


int	glsk_set_resolution( int width, int height, int bpp )
{
	DEVMODE device_mode;

	if ( width * height * bpp == 0 )
	{
		// reset to our initial mode
		device_mode = global.previous_mode;
	}
	else
	{
		// select an appropriate mode
		ZeroMemory( &device_mode, sizeof(DEVMODE) );

		device_mode.dmSize			= sizeof(DEVMODE);
		device_mode.dmPelsWidth		= width;
		device_mode.dmPelsHeight	= height;
		device_mode.dmBitsPerPel	= bpp;
		device_mode.dmFields		= ( DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT );
	}

	if ( ChangeDisplaySettings( &device_mode, CDS_FULLSCREEN ) == DISP_CHANGE_SUCCESSFUL )
	{
		// set the devmode changed flag, so we can change back on module_free
		global.flags |= GLSK_GLOBAL_DEVMODE_CHANGED;

		return 1;
	}
	else
	{
		return 0;
	}
}


void	glsk_error_box( glsk_window_t* parent, const char* text )
{
	// start a messagebox with the given text
	MessageBox( (parent) ? parent->handle : NULL, text, "glsk-error", MB_ICONERROR );
}

void	glsk_print( const char* string )
{
	OutputDebugString( string );
}

/*

	module entry point

*/

int WINAPI	DllMain( HINSTANCE hinstance, DWORD reason, PVOID reserved )
{
	switch ( reason )
	{
		case DLL_PROCESS_ATTACH:
			return _glsk_module_init( hinstance );

		case DLL_PROCESS_DETACH:
			return _glsk_module_free( hinstance );

		default:
			return 0;
	}
}
