
/*
**
**		filename:		w_pbuffer.c
**
**		description:	pbuffer related functions
**		target-system:	microsoft windows
**		version-info:   $Id: w_extensions.c 76 2005-02-23 09:48:21Z jax $
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
#include "w_pixelformat.h"

struct glsk_pbuffer_s
{
	HPBUFFER				handle;
	HDC						dc;

	glsk_window_t*			owner;
	glsk_context_t*			context;
	int						width;
	int						height;
};

glsk_pbuffer_t* glsk_pbuffer_new()
{
	glsk_pbuffer_t*	result = (glsk_pbuffer_t*)malloc( sizeof( glsk_pbuffer_t ) );

	result->handle = 0;
	result->dc = 0;

	result->owner = 0;
	result->context = 0;

	result->width = result->height = 0;

	return result;
}


void glsk_pbuffer_free( glsk_pbuffer_t* pbuffer )
{
	free( pbuffer );
}

int glsk_pbuffer_open( glsk_pbuffer_t* pbuffer, glsk_window_t* parent, glsk_context_t* context,
					  glsk_pixelformat_t* pixelformat, int width, int height, int get_largest )
{
	int descriptor = 0;

	if ( !parent || !context )
		return FALSE;

	// get a pixelformat
	if ( !_glsk_pixelformat_get_descriptor( parent->dc, pixelformat, &descriptor ) )
		return FALSE;


	// get the pbuffer handle
	if ( get_largest )
	{
		const int attribs[] = { WGL_PBUFFER_LARGEST_ARB, 1, 0 };

		pbuffer->handle = global.function.wgl_create_pbuffer( parent->dc, descriptor, width, height, attribs );
	}
	else
	{
		pbuffer->handle = global.function.wgl_create_pbuffer( parent->dc, descriptor, width, height, 0 );
	}

	if ( !pbuffer->handle )
		return FALSE;

	// get the device context
	pbuffer->dc = global.function.wgl_get_pbuffer_dc( pbuffer->handle );

	// bind/create a rendering context
	if ( context->handle == NULL ) // create a physical context?
        context->handle = wglCreateContext( pbuffer->dc );

	if ( context->handle == NULL )
	{
		glsk_pbuffer_destroy( pbuffer );
		return FALSE;
	}

	context->refcount++;

	// init the structure
	pbuffer->context = context;
	pbuffer->width = width;
	pbuffer->height = height;

	return TRUE;
}

void glsk_pbuffer_destroy( glsk_pbuffer_t* pbuffer )
{
	if ( wglGetCurrentDC() == pbuffer->dc )
		wglMakeCurrent( pbuffer->dc, 0 );

	if ( pbuffer->context && ( pbuffer->context->handle != NULL ) )
	{
		--pbuffer->context->refcount;

		if ( pbuffer->context->refcount == 0 )
		{
			wglDeleteContext( pbuffer->context->handle );
			pbuffer->context->handle = NULL;
		}
	}

	if ( pbuffer->dc )
	{
		global.function.wgl_release_pbuffer_dc( pbuffer->handle, pbuffer->dc );
		pbuffer->dc = 0;
	}

	if ( pbuffer->handle )
	{
		global.function.wgl_destroy_pbuffer( pbuffer->handle );
		pbuffer->handle = 0;
	}

	pbuffer->width = 0;
	pbuffer->height = 0;
	pbuffer->context = 0;
}

void glsk_pbuffer_swap_buffers( glsk_pbuffer_t* pbuffer )
{
	SwapBuffers( pbuffer->dc );
}

int glsk_pbuffer_select_rendercontext( glsk_pbuffer_t* pbuffer )
{
	return wglMakeCurrent( pbuffer->dc, pbuffer->context->handle );
}

