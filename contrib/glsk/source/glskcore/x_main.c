
/*
**
**		filename:		x_main.c
**
**		description:	main file
**		target-system:	X-Windows
**		version-info:   $Id$
**
*/


// system includes
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#include <X11/extensions/xf86vmode.h>

// global include
#include <glsk/glsk.h>

// internal includes
#include "x_main.h"
#include "x_pixelformat.h"






// instanciate the globals
glsk_global_t	global;

void
_glsk_init_cursor()
{
	char 		data = 0;
	XColor 		dummy;
	Pixmap 		blank_pixmap = XCreateBitmapFromData( global.connection,
		RootWindow( global.connection, global.screen ), &data, 1, 1);

	global.blank_cursor = XCreatePixmapCursor( global.connection, blank_pixmap,
								blank_pixmap, &dummy, &dummy, 0, 0);
	
	XFreePixmap ( global.connection, blank_pixmap );
}

int
_glsk_init()
{
	XF86VidModeModeInfo** 	video_modes = 0;
	int						video_mode_count = 0;
	glsk_window_t*			window = 0;
	
	// grab our current connection to the server
	global.connection = XOpenDisplay( 0 );
	
	// grab the default screen for this workstation
	global.screen = DefaultScreen( global.connection );

	// create an atom for windows to store the glsk_window_t pointer
	global.object_atom = XInternAtom( global.connection, "GLSK_OBJECT", FALSE );
	global.delete_atom = XInternAtom( global.connection, "GLSK_DELETE_WINDOW", FALSE );
	
	global.protocols_atom = XInternAtom( global.connection, "WM_PROTOCOLS", TRUE );

	global.flags = 0;

	glXQueryVersion( global.connection, &(global.glx_version[ 0 ]), &(global.glx_version[ 1 ]) );
	
	if ( ( global.glx_version[ 0 ] <= 1 ) && ( global.glx_version[ 1 ] < 3 ) )
		global.glx_fallback_mode = 1;
	else
		global.glx_fallback_mode = 0;
	
	global.idle_head = 0;
	global.window_head = 0;
	global.focus_window = 0;
	global.open_window_count = 0;

	_glsk_init_cursor();
	
	// get all available modes
	XF86VidModeGetAllModeLines( global.connection, global.screen, &video_mode_count, &video_modes );

	// save desktop-resolution
	global.desktop_video_mode = *(video_modes[ 0 ]);
	
	// free those modes
	XFree( video_modes );
	
	if ( !_glsk_input_init() )
		return 0;
	
	window = glsk_window_new();
	
	if ( window )
	{
		glsk_context_t* context = glsk_context_new();
		
		if ( glsk_window_open( window, context ) )
		{
			glsk_window_select_rendercontext( window );
			_glsk_init_extensions_string();
			glsk_window_destroy( window );
			
			glsk_main_run();
		}
		
		glsk_context_free( context );
	}
	
	glsk_window_free( window );
		
	return 1;
}

void
_glsk_reset_vidmode()
{
	if ( global.flags & GLSK_MAIN_VIDMODE_CHANGED )
	{
		XF86VidModeSwitchToMode( global.connection, global.screen, &global.desktop_video_mode );
		XF86VidModeSetViewPort( global.connection, global.screen, 0, 0 );
		
		global.flags &= ~GLSK_MAIN_VIDMODE_CHANGED;
	}
}

void
_glsk_free()
{	
	while ( global.window_head )
	{
		glsk_window_t*	window = global.window_head;
		
		if ( window->flags & GLSK_WINDOW_OPEN )
		{
			printf( "(glsk) warning: undestroyed window found\n" );
			//glsk_window_destroy( window ); // FIXME: this seems to cause problems
		}
		
		glsk_window_free( window );
	}

	_glsk_reset_vidmode();
	
	XCloseDisplay( global.connection );
	global.connection = None;
}

glsk_context_t* glsk_context_new()
{
	glsk_context_t* result = (glsk_context_t*)malloc( sizeof( struct glsk_context_s ) );
	
	result->handle = None;
	result->refcount = 0;
	
	return result;
}

void glsk_context_free( glsk_context_t* context )
{
	free( context );
}

int
glsk_set_resolution( int width, int height, int bpp )
{
	XF86VidModeModeInfo** 	video_modes = 0;
	int						video_mode_count = 0;
	int						i;
	
	// get all available modes
	XF86VidModeGetAllModeLines( global.connection, global.screen, &video_mode_count, &video_modes );
	
	for ( i = 0; i < video_mode_count; ++i )
	{
		// search for a matching mode
		if ( ( video_modes[ i ]->hdisplay == width ) &&
			 ( video_modes[ i ]->vdisplay == height ) )
		{
			// this one matches our resolution, set it
			XF86VidModeModeInfo* mode = video_modes[ i ];
			
			printf( "(glsk) selecting video-mode: %i x %i\n", mode->hdisplay, mode->vdisplay );
			
			XF86VidModeSwitchToMode( global.connection, global.screen, mode );
			XF86VidModeSetViewPort( global.connection, global.screen, 0, 0 );

			XFree( video_modes );
			
			global.flags |= GLSK_MAIN_VIDMODE_CHANGED;

			return 1;	
		}
	}
	
	XFree( video_modes );
	
	printf( "(glsk) unable to select video-mode: %i x %i\n", width, height );
	
	return 0;
}

glsk_window_t*
glsk_window_new()
{
	glsk_window_t* window = (glsk_window_t*)malloc( sizeof( struct glsk_window_s ) );
	
	window->handle = None;
	window->context = 0;
	window->drawable = None;
	
	window->title = 0;
	
	window->callback.pixelformat_select = 0;
	window->callback.char_event = 0;
	window->callback.configure_event = 0;
	window->callback.redraw_event = 0;
	window->callback.create_event = 0;
	window->callback.destroy_event = 0;
	window->callback.close_event = 0;
	window->callback.mouse_event = 0;

	window->width = 400;
	window->height = 300;
	window->px = 0;
	window->py = 0;
	
	window->flags = 0;
	
	// attach this to the list of windows
	if ( global.window_head )
	{
		window->prev = 0;
		window->next = global.window_head;
		global.window_head->prev = window;
		global.window_head = window;
	}
	else
	{
		window->prev = window->next = 0;
		global.window_head = window;
	}
	
	return window;
}

void
glsk_window_free( glsk_window_t* window )
{
	if ( window == 0 )
		return;
	
	free( window->title );
	
	if ( global.window_head == window )
	{
		global.window_head = window->next;
	}
	else
	{
		// since we are not the head, we have to have a previous
		window->prev->next = window->next;
		
		if ( window->next )
			window->next->prev = window->prev;
	}
	
	free( window );
}

int
glsk_window_open( glsk_window_t* window, glsk_context_t* context )
{
	XSetWindowAttributes	attributes;
	unsigned int			attributes_mask = 0;
	XVisualInfo*			visual_info = 0;
	GLXFBConfig				fb_config;
	glsk_pixelformat_t		pixelformat;
	Window					root_window = RootWindow( global.connection,
														global.screen );
	Atom 					wm_delete_atom = XInternAtom( global.connection, "WM_DELETE_WINDOW", TRUE );
	
	if ( global.glx_version[ 0 ] < 1 )
		return 0;

	// init the pixelformat and get the associated visual info
	_glsk_pixelformat_init( &pixelformat );

	// run the user-handler
	if ( window->callback.pixelformat_select )
		window->callback.pixelformat_select( window, &pixelformat );

	if ( global.glx_fallback_mode == 0 )
	{
		// select the appropriate fbconfig
		fb_config = _glsk_pixelformat_get_fbconfig( &pixelformat );
		
		// did we find a suitable fb_config?
		if ( fb_config == None )
			return 0;
		
		visual_info = glXGetVisualFromFBConfig( global.connection,	fb_config );
	}
	else
	{
		visual_info = _glsk_pixelformat_get_visualinfo( &pixelformat );
	}
		
	// did we find a matching visual info too?
	if ( visual_info == 0 )
	{
		printf( "(glsk) unable to find requested visual.\n" );
		return 0;
	}
	
	// init the colormap
	attributes.colormap = XCreateColormap( global.connection, root_window,
					visual_info->visual, AllocNone );

	attributes.border_pixel = 0;

	attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
		StructureNotifyMask | PointerMotionMask | FocusChangeMask;

	attributes.override_redirect = TRUE;

	attributes_mask = CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect;

	// create the physical window
	window->handle = XCreateWindow( global.connection, root_window, window->px, window->py,
		window->width, window->height, 0, visual_info->depth, InputOutput, visual_info->visual, 
		attributes_mask, &attributes );

	if ( window->handle == None )
		return 0;
	
	//XSync( global.connection, 0 );
	//_glsk_wait_for_map_notify( window->handle );
	
	glsk_window_set_position( window, window->px, window->py );
	
	// flag this window as open
	window->flags |= GLSK_WINDOW_OPEN;
	
	XSetWMProtocols( global.connection, window->handle, &wm_delete_atom, 1 );

	// set the object pointer -TODO: make this 64-bit compatible
	//printf( "Created window with address: %p\n", window );
	XChangeProperty( global.connection, window->handle, global.object_atom, global.object_atom,
		32, PropModeReplace, (unsigned char*)&window, sizeof( glsk_window_t* )/sizeof(int) );

	// fallback if we are below GLX version 1.3 (damn ATI drivers)
	if ( global.glx_fallback_mode == 0 ) // GLX >= 1.3
	{
		if ( context->handle == None )
		{
			// create the GL context
			context->handle = glXCreateNewContext( global.connection, fb_config, GLX_RGBA_TYPE, 0, TRUE );
		
			if ( context->handle == None )
				return 0;
			
			if ( !glXIsDirect( global.connection, context->handle ) )
				return 0;
		}
		
		window->drawable = glXCreateWindow( global.connection, fb_config, window->handle, 0 );
		
		if ( window->drawable == None )
			return 0;
	}
	else // GLX <= 1.2
	{
		
		printf( "(glsk) warning: using glx version < 1.3 fallback\n" );
		
		if ( context->handle == None )
		{
			context->handle = glXCreateContext( global.connection, visual_info, 0, TRUE );
			
			if ( context->handle == None )
				return 0;
	
			if ( !glXIsDirect( global.connection, context->handle ) )
				return 0;
		}
	}

	// bind the context
	window->context = context;
	context->refcount++;	
	
	if ( window->flags & GLSK_WINDOW_HAS_DECORATION )
		glsk_window_set_decorated( window, 1 );
	
	
	if ( window->title )
		glsk_window_set_title( window, window->title );

	if ( window->callback.create_event && 
		 !window->callback.create_event( window, window->width, window->height ) )
	{
		return 0;
	}

	global.open_window_count++;
	
	return 1;		
}

int
glsk_window_select_rendercontext( glsk_window_t* window )
{
	if ( global.glx_fallback_mode == 0 )
	{
		// GLX >= 1.3
		return glXMakeContextCurrent( global.connection, window->drawable,
					window->drawable, window->context->handle );
	}
	else
	{
		// GLX <= 1.2
		return glXMakeCurrent( global.connection, window->handle, window->context->handle );
	}
}

void
glsk_window_swap_buffers( glsk_window_t* window )
{
	if ( global.glx_fallback_mode == 0 )
	{
		// GLX >= 1.3
		glXSwapBuffers( global.connection, window->drawable );
	}
	else
	{
		// GLX <= 1.2
		glXSwapBuffers( global.connection, window->handle );
	}
}

void
glsk_window_destroy( glsk_window_t* window )
{
	if ( (window->handle != None) && ( window->flags & GLSK_WINDOW_OPEN ) )
	{
		XEvent event;
	
		event.xclient.type = ClientMessage;
		event.xclient.window = window->handle;
		event.xclient.display = global.connection;
		event.xclient.message_type = global.delete_atom;
		event.xclient.format = 32;
	
		XSendEvent( global.connection, window->handle, FALSE, 0, &event );
	}
}

void _glsk_window_destroy( glsk_window_t* window )
{
	if ( (window->flags & GLSK_WINDOW_OPEN) == 0 )
		return;
	
	// call the destroy event handler
	if ( window->callback.destroy_event )
	{
		window->callback.destroy_event( window, 0 );
	}
	else
	{
		// default behaviour is to quit
		glsk_main_quit();
	}
		
	// glx 1.3 and higher
	if ( global.glx_fallback_mode == 0 )
	{
		// TODO: this unbinding is not quite right, could be considered a side effect
		// unbind our GL context, if it's bound
		if ( glXGetCurrentDrawable() == window->drawable ) 
		{
			glXMakeContextCurrent( global.connection, None, None, None );
		}
	
		window->context->refcount--;
		
		if ( window->context->refcount == 0 )
		{
			if ( glXGetCurrentContext() == window->context->handle ) 
			{
				glXMakeContextCurrent( global.connection, None, None, None );
			}
			
			// destroy our GL context
			glXDestroyContext( global.connection, window->context->handle );
			window->context->handle = None;
		}
		
		// destroy the gl window
		glXDestroyWindow( global.connection, window->drawable );
		window->drawable = None;
					
		// destroy the window
		XDestroyWindow( global.connection, window->handle );
	}
	else // glx 1.2 and less
	{
		// unbind gl context
		if ( glXGetCurrentContext() == window->context->handle )
			glXMakeCurrent( global.connection, None, None );
		
		window->context->refcount--;
		
		if ( window->context->refcount == 0 )
		{
			// destroy our GL context
			glXDestroyContext( global.connection, window->context->handle );
			window->context->handle = None;
		}

		// destroy the window
		XDestroyWindow( global.connection, window->handle );
	}
	
	window->context = 0;
	window->flags &= ~GLSK_WINDOW_OPEN;
	window->handle = None;
	
	global.open_window_count--;
}

void _glsk_send_mouse_event( glsk_window_t* window, int px, int py, int type, int button )
{
	if ( window->callback.mouse_event )
	{
		glsk_mouse_event_t	event;
		
		event.px = px; 
		event.py = py;
		event.type = type;
		event.button = button;
		
		window->callback.mouse_event( window, &event );
	}
}

int _glsk_process_event( glsk_window_t* window, XEvent* event )
{
	/*if ( event->type != 6 )
		printf( "Got event: %i\n", event->type );*/
	
	switch( event->type )
	{
		case ReparentNotify:
		{
			if ( (window->flags & GLSK_WINDOW_HAS_DECORATION) == 0 )
			{
				XSetWindowAttributes	attributes;
				
				// apparently, we just released our decoration - turn on the redirect override_redirect
				attributes.override_redirect = TRUE;
		
				XChangeWindowAttributes( global.connection, window->handle, CWOverrideRedirect, &attributes );
			}
			
			return 1;
		};
		
		case MotionNotify:
		{
			if ( _glsk_input_update_pointer_move( event->xmotion.x, event->xmotion.y, window ) == 0 )
				_glsk_send_mouse_event( window, event->xmotion.x, window->height-event->xmotion.y,
							GLSK_ME_TYPE_MOVE, 0 );
			return 1;
		};

		case ButtonPress:
		{
			XSetInputFocus( global.connection, window->handle, RevertToPointerRoot, CurrentTime );
			
			if ( _glsk_input_update_pointer_button( event->xbutton.button, 1 ) == 0 )
				_glsk_send_mouse_event( window, event->xbutton.x, window->height-event->xbutton.y,
					GLSK_ME_TYPE_BUTTONPRESSED, event->xbutton.button );
			
			return 1;
		};

		case ButtonRelease:
		{
			if ( _glsk_input_update_pointer_button( event->xbutton.button, 0 ) == 0 )
				_glsk_send_mouse_event( window, event->xbutton.x, window->height-event->xbutton.y,
					GLSK_ME_TYPE_BUTTONRELEASED, event->xbutton.button );
			
			return 1;
		};

		case MapNotify:
		{
			XSetInputFocus( global.connection, window->handle, RevertToPointerRoot, CurrentTime );
			return 1;
		};
		
		case DestroyNotify:
		{
			window->handle = None;
			return 1;
		};

		case KeyPress:
		{
			if ( window && window->callback.char_event )
			{
				KeySym			key_symbol;
				char			buffer[ 5 ] = { '\0', '\0', '\0', '\0', '\0' };

				XLookupString( &event->xkey, buffer, 4, &key_symbol, NULL );

				if ( key_symbol )
				{
					_glsk_input_key_event( key_symbol, 1 );
					
					if ( (key_symbol>>8) == 0 )
						window->callback.char_event( window, buffer );
				}				
			}
			return 1;
		}
		
		case KeyRelease:
		{
			KeySym key_symbol = XKeycodeToKeysym( global.connection, event->xkey.keycode, 0 );
			
			if ( key_symbol )
				_glsk_input_key_event( key_symbol, 0 );
			
			return 1;
		}

		case ConfigureNotify:
		{
			XWindowAttributes root_attribs;
			Window root_window = RootWindow( global.connection, global.screen );
		
			if ( window == 0 )
				return 1;
						
			XGetWindowAttributes( global.connection, root_window, &root_attribs );
			
			window->width = event->xconfigure.width;
			window->height = event->xconfigure.height;
			
			if ( (window->flags & GLSK_WINDOW_FULLSCREEN) == 0 )
			{
				window->uwidth = window->width;
				window->uheight = window->height;
				window->px = event->xconfigure.x;
				window->py = root_attribs.height - event->xconfigure.height - event->xconfigure.y;
			}
			
			//printf( "window configure: x:%i y:%i width:%i height:%i \n",
			//	window->px, window->py, window->width, window->height );

			if ( window->callback.configure_event )
				window->callback.configure_event( window, window->width, window->height );

			return 1;
		}

		case Expose:
		{
			if ( window == 0 )
				return 1;

			if ( window->callback.redraw_event )
				window->callback.redraw_event( window, 0 );

			return 1;
		}
		
		case ClientMessage:
		{
			if ( event->xclient.message_type == global.protocols_atom )
			{				
				// the close event
				
				if ( window->callback.close_event )
					window->callback.close_event( window, 0 );
				else
					glsk_window_destroy( window );
			}
			else if ( event->xclient.message_type == global.delete_atom )
			{
				_glsk_window_destroy( window );
			}
			else
			{
				printf( "unrecognized client-message type: %s \n",
						XGetAtomName( global.connection, event->xclient.message_type ) );
			}
			
			return 1;
		};
		
		case UnmapNotify:
		{			
			return 1;
		};
		
		case FocusIn:
		{
			if ( (global.focus_window != 0) && (global.focus_window != window) )
			{
				printf( "(glsk) Warning: another window is still focused.\n" );
			}

			global.focus_window = window;
			
			_glsk_input_update_pointer_owner();
			
			return 1;
		};
		
		case FocusOut:
		{
			if ( global.focus_window && (global.focus_window != window) )
			{
				printf( "(glsk) Warning: losing focus without having focus.\n" );
			}
			
			global.focus_window = 0;
			
			_glsk_input_update_pointer_owner();
			
			return 1;
		};

		default:
			// unrecognized event -> terminate
			printf( "(glsk) unrecognized event: %i - stopping.\n", event->type );
			return 0;
	};
}

int
_glsk_decode_window_object_error( Display* display, XErrorEvent* event )
{
	global.flags |= GLSK_MAIN_DECODE_ERROR;
	
	return 1;
}

glsk_window_t*
_glsk_decode_window_object( Window handle )
{
	glsk_window_t*			result = 0;
	Atom					return_type;
	int						return_format;
	unsigned long			return_count;
	unsigned long			return_rest;
	unsigned char*			data = 0;
	
	if ( handle == None )
		return 0;
	
	XSetErrorHandler( _glsk_decode_window_object_error );
	
	if ( XGetWindowProperty( global.connection, handle,
				global.object_atom, 0, sizeof( glsk_window_t* )/sizeof(int), 0,
				global.object_atom, &return_type, &return_format, &return_count,
				&return_rest, (unsigned char**)(&data) ) == Success )
	{
		// decode the object pointer
		result = (glsk_window_t*)(*((glsk_window_t**)data));
		XFree( data );			
	}
	else
	{
		result = 0;
	}
	
	XSetErrorHandler( 0 );
	
	if ( global.flags & GLSK_MAIN_DECODE_ERROR )
	{
		global.flags &= ~GLSK_MAIN_DECODE_ERROR;
		
		// it wasn't able to decode the pointer, try a manual lookup
		result = global.window_head;
		
		while ( result )
		{
			if ( result->handle == handle )
				return result;
			
			result = result->next;
		}
	}
	
	return result;
}

int
glsk_main_run()
{
	XEvent					event;
	glsk_window_t*			window = 0;
	glsk_idle_handler_t*	node = 0;

	
	while ( global.open_window_count ) if ( XPending( global.connection ) > 0 )
	{
		XNextEvent( global.connection, &event );
		
		window = _glsk_decode_window_object( event.xany.window );

		if ( window == 0 )
		{
			printf( "(glsk) unable to decode window pointer: %p\n", window );
			break;
		}

		// handle this message
		if ( _glsk_process_event( window, &event ) == 0 )
		{
			// break if needed
			break;
		}
	}
	else
	{
		_glsk_input_update();
		
		if ( node = global.idle_head )
		{
			while ( node )
			{
				node->function( node->context );
				node = node->next;
			}
		}
	}
	
	XSync( global.connection, 1 );
}

void
glsk_main_quit()
{
	//global.flags |= GLSK_MAIN_QUIT;
	glsk_window_t* window = 0;
	
	for ( window = global.window_head; window != 0; window = window->next )
		glsk_window_destroy( window );
}

void
glsk_error_box( glsk_window_t* parent, const char* text )
{
	// TODO: use X
	printf( "%s\n", text );
}

void	glsk_print( const char* string )
{
	printf( string );
}

int _glsk_count_words( const char* string )
{
	int result = 0;
	char last = ' ';

	if ( string == 0 )
		return 0;

	while ( *string != 0 )
	{
        if ( ( *string != ' ' ) && ( last == ' ' ) )
			++result;

		last = *string;
		++string;
	}

	return result;
}

int _glsk_string_compare( const void* a, const void* b )
{
	return strcmp( *(char**)a, *(char**)b );
}

int _glsk_init_extensions_string( )
{
	size_t buffer_length;
	int index = 0;
	
	buffer_length = strlen( glGetString( GL_EXTENSIONS ) ) +
			strlen( glXQueryExtensionsString( global.connection, global.screen ) ) + 1;
	
	// reserve memory for the string
	global.extensions_buffer = (char*)malloc( buffer_length );

	// copy the extensions into the buffer
	strcpy( global.extensions_buffer, glGetString( GL_EXTENSIONS ) );
	strcat( global.extensions_buffer, glXQueryExtensionsString( global.connection, global.screen ) );
	
	//printf( "Extensions: %s\n", glGetString( GL_EXTENSIONS ) );

	// count the number of words/extensions
	global.extensions_count = _glsk_count_words( global.extensions_buffer );

	if ( global.extensions_count )
	{
		char* string = global.extensions_buffer;
		char last = ' ';
		
		global.extensions = (char**)malloc( sizeof( char* ) * global.extensions_count );

		while ( *string != 0 )
		{
			if ( (last == ' ') && ( *string != ' ' ) )
				global.extensions[ index++ ] = string;
			
			last = *string;
			
			if ( *string == ' ' )
				*string = '\0';
			
			++string;
		}

	}

	// sort the array
	qsort( global.extensions, global.extensions_count, sizeof( char* ), _glsk_string_compare );

	return 1;
}
