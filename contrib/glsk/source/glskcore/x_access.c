
/*
**
**		filename:		x_access.c
**
**		description:	window access functions
**		target-system:	X-Windows
**		version-info:   $Id$
**
*/


#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#include <X11/extensions/xf86vmode.h>
#include <glsk/glsk.h>
#include "x_main.h"
#include "x_access.h"

const char* null_string = "";

void glsk_window_set_callback_char_event( glsk_window_t* window,
				glsk_callback_window_string_t callback )
{
	window->callback.char_event = callback;
}


void glsk_window_set_callback_pixelformat_select( glsk_window_t* window,
				glsk_callback_window_pixelformat_t callback )
{
	window->callback.pixelformat_select = callback;
}


void glsk_window_set_callback_configure_event( glsk_window_t* window,
				glsk_callback_window_coord_t callback )
{
	window->callback.configure_event = callback;
}


void glsk_window_set_callback_redraw_event( glsk_window_t* window,
				glsk_callback_window_voidp_t callback )
{
	window->callback.redraw_event = callback;
}


void glsk_window_set_callback_create_event( glsk_window_t* window,
				glsk_callback_window_coord_t callback )
{
	window->callback.create_event = callback;
}

void glsk_window_set_callback_close_event( glsk_window_t* window,
				glsk_callback_window_voidp_t callback )
{
	window->callback.close_event = callback;
}

void glsk_window_set_callback_destroy_event( glsk_window_t* window,
				glsk_callback_window_voidp_t callback )
{
	window->callback.destroy_event = callback;
}

void glsk_window_set_callback_mouse_event( glsk_window_t* window, glsk_callback_window_mouse_event_t callback )
{
	window->callback.mouse_event = callback;
}


glsk_idle_handler_t* glsk_main_append_idle_handler( glsk_callback_voidp_t callback, void* context )
{
	glsk_idle_handler_t* newnode = (glsk_idle_handler_t*)malloc( sizeof( glsk_idle_handler_t ) );

	newnode->function = callback;
	newnode->context = context;

	if ( global.idle_head )
	{
		newnode->prev = 0;
		newnode->next = global.idle_head;
		global.idle_head->prev = newnode;

		global.idle_head = newnode;
	}
	else
	{
		newnode->prev = 0;
		newnode->next = 0;
		global.idle_head = newnode;
	}

	return newnode;
}

void glsk_main_remove_idle_handler( glsk_idle_handler_t* node )
{
	if ( node == global.idle_head )
		global.idle_head = node->next;

	if ( node->next )
		node->next->prev = node->prev;

	if ( node->prev )
		node->prev->next = node->next;

	free( node );
}

void glsk_window_set_userdata( glsk_window_t* window, void* userdata )
{
	window->userdata = userdata;
}

void* glsk_window_get_userdata( glsk_window_t* window )
{
	return window->userdata;
}

int glsk_window_show( glsk_window_t* window )
{
	window->flags |= GLSK_WINDOW_VISIBLE;
	
	if ( window->handle )
	{
		XMapWindow( global.connection, window->handle );
	}
	return 1;
}

void glsk_window_hide( glsk_window_t* window )
{
	XUnmapWindow( global.connection, window->handle );
	
	window->flags &= ~GLSK_WINDOW_VISIBLE;
}

int glsk_window_is_visible( glsk_window_t* window )
{
	return (window->flags & GLSK_WINDOW_VISIBLE) ? 1 : 0;
}

void glsk_window_get_size( glsk_window_t* window, int* width, int* height )
{
	*width = window->width;
	*height = window->height;
}

int glsk_window_get_width( glsk_window_t* window )
{
	return window->width;
}

int glsk_window_get_height( glsk_window_t* window )
{
	return window->height;
}

void glsk_window_get_position( glsk_window_t* window, int* px, int* py )
{
	*px = window->px;
	*py = window->py;
}

const char* glsk_window_get_title( glsk_window_t* window )
{
	// if we set the title, return it, otherwise return a null string
	if ( window && window->title )
		return window->title;
	else
		return null_string;
}

void glsk_window_set_title( glsk_window_t* window, const char* title )
{	
	int length;
	
	if ( !title ) // accept NULL
	{
		title = null_string;
	}

	if ( window->title != title )
	{
		// free the format window title
		free( window->title );
		
		// copy the string to the window structure
		length = strlen( title );	
		window->title = (char*)malloc( length + 1 );
		strcpy( window->title, title );
	}
	
	if ( window->handle )
	{
		// convert the string we got to a text property
		XTextProperty property;
		
		if ( XmbTextListToTextProperty( global.connection, &window->title, 1, XTextStyle, &property ) != Success )
			return;
		
		// set the physical window name
		XSetWMName( global.connection, window->handle, &property );

		XFree( property.value );		
	}
}

int glsk_window_get_decorated( glsk_window_t* window )
{
	return window && ( window->flags & GLSK_WINDOW_HAS_DECORATION );
}

void glsk_window_set_decorated( glsk_window_t* window, int has_decoration )
{
	if ( has_decoration != 0 )
		has_decoration = 1;
	
	if ( has_decoration )
		window->flags |= GLSK_WINDOW_HAS_DECORATION;
	else
		window->flags &= ~GLSK_WINDOW_HAS_DECORATION;
	
	if ( window->handle )
	{
		glsk_motif_wm_hints_t 	hints_object;
		glsk_motif_wm_hints_t*	hints_pointer  = 0;
		Atom					hints_atom = None;
		int						return_format = 0;
		Atom					return_type = None;
		unsigned long			return_count = 0;
		unsigned long			return_rest = 0;
		XSetWindowAttributes	attributes;
		
		attributes.override_redirect = FALSE;
		
		XChangeWindowAttributes( global.connection, window->handle, CWOverrideRedirect, &attributes );
		
		if ( has_decoration )
		{
			
			//XMapRaised( global.connection, window->handle );
			XUnmapWindow( global.connection, window->handle );
			XMapWindow( global.connection, window->handle );
		}
		XFlush( global.connection );
		
		
		//printf( "changing decoration %i\n", has_decoration );
		
		hints_object.flags = MWM_HINTS_DECORATIONS;
		hints_object.decorations = has_decoration;
	
		// try getting the window-decoration(hints) extension atom		
		hints_atom = XInternAtom( global.connection, _XA_MOTIF_WM_HINTS, FALSE );

		
		XGetWindowProperty ( global.connection, window->handle,
			hints_atom, 0, sizeof (glsk_motif_wm_hints_t)/sizeof (int),
			FALSE, AnyPropertyType, &return_type, &return_format, &return_count,
			&return_rest, (unsigned char **)&hints_pointer);
		
		if ( return_type != None )
		{
			hints_pointer->flags |= MWM_HINTS_DECORATIONS;
			hints_pointer->decorations = hints_object.decorations;
		}
		else
		{
			hints_pointer = &hints_object;
		}
		
		XChangeProperty ( global.connection, window->handle, hints_atom,
			hints_atom, 32, PropModeReplace, (unsigned char *)hints_pointer,
			sizeof (glsk_motif_wm_hints_t)/sizeof (int));
  
		if ( hints_pointer != &hints_object )
			XFree ( hints_pointer );
		
		XFlush( global.connection );
		//;
	}
}

void glsk_window_set_size( glsk_window_t* window, int width, int height )
{
	window->uwidth = window->width = width;
	window->uheight = window->height = height;
	
	if ( window->handle )
	{
		XWindowChanges changes;
		Window root_window = RootWindow( global.connection, global.screen );
		XWindowAttributes root_attribs;
		
		XGetWindowAttributes( global.connection, root_window, &root_attribs );
		
		changes.x = window->px;
		changes.y = root_attribs.height-height-window->py;
		changes.width = window->width = width;
		changes.height = window->height = height;
		
		XConfigureWindow( global.connection, window->handle,
					CWWidth | CWHeight | CWX | CWY, &changes );
	}
}

void glsk_window_set_width( glsk_window_t* window, int width )
{
	glsk_window_set_size( window, width, window->height );
}

void glsk_window_set_height( glsk_window_t* window, int height )
{
	glsk_window_set_size( window, window->width, height );
}

void glsk_window_set_position( glsk_window_t* window, int x, int y )
{
	window->px = x;
	window->py = y;
	
	if ( window->handle )
	{
		Window root_window = RootWindow( global.connection, global.screen );
		XWindowAttributes root_attribs;
		XGetWindowAttributes( global.connection, root_window, &root_attribs );
		
		y = root_attribs.height-window->height-y;

		XMoveWindow( global.connection, window->handle, x, y );		
	}	
}

int	glsk_extensions_get_count()
{
	return global.extensions_count;
}

const char* glsk_extensions_get( int index )
{
	if ( (0 <= index) && (index < global.extensions_count) )
		return global.extensions[ index ];
	else
		return null_string;
}

int	glsk_extensions_is_supported( const char* extension )
{
	int l = 0;
	int r = global.extensions_count-1;

	while ( l <= r )
	{
		int m = (l + r)/2;
		int result = strcasecmp( extension, global.extensions[ m ] );

		if ( result < 0 )
			r = m - 1;
		else if ( result > 0 )
			l = m + 1;
		else
			return 1;
	}

	return 0;
}

int glsk_window_get_fullscreen( glsk_window_t* window )
{
	return window->flags & GLSK_WINDOW_FULLSCREEN;
}

void glsk_window_set_fullscreen( glsk_window_t* window, int value )
{
	if ( value )
	{
		window->flags |= GLSK_WINDOW_FULLSCREEN;
	}
	else
	{
		window->flags &= ~GLSK_WINDOW_FULLSCREEN;
	}	
	
	if ( !window->handle )
		return;
	
	if ( window->flags & GLSK_WINDOW_FULLSCREEN )
	{
		XWindowChanges changes;
		Window root_window = RootWindow( global.connection, global.screen );		
		XWindowAttributes root_attribs;
		
		XGetWindowAttributes( global.connection, root_window, &root_attribs );
		
		changes.x = 0;
		changes.y = 0;
		changes.width = root_attribs.width;
		changes.height = root_attribs.height;

		
		XConfigureWindow( global.connection, window->handle,
					CWWidth | CWHeight | CWX | CWY, &changes );
		XRaiseWindow( global.connection, window->handle );
	}
	else
	{
		glsk_window_set_size( window, window->uwidth, window->uheight );
	}
}

void glsk_mouse_event_get_position( glsk_mouse_event_t* mouse_event, int* px, int* py )
{
	*px = mouse_event->px;
	*py = mouse_event->py;
}

int glsk_mouse_event_get_type( glsk_mouse_event_t* mouse_event )
{
	return mouse_event->type;
}

int glsk_mouse_event_get_button( glsk_mouse_event_t* mouse_event )
{
	return mouse_event->button;
}
