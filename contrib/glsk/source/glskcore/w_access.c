
/*
**
**		filename:		w_access.c
**
**		description:	get and set functions for the window
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









glsk_idle_handler_t* glsk_main_append_idle_handler( glsk_callback_voidp_t callback, void* context )
{
	glsk_idle_handler_t*	newnode = (glsk_idle_handler_t*)malloc( sizeof( glsk_idle_handler_t ) );

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
