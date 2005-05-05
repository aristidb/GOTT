
/*
**
**		filename:		x_pixelformat.c
**
**		description:	pixelformat/visual/fbconfig related functions
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

struct glsk_pbuffer_s
{
	GLXPbuffer		handle;
	glsk_context_t*	context;
	glsk_window_t*	owner;
	
	int				width;
	int				height;
	
};

glsk_pbuffer_t*	glsk_pbuffer_new()
{
	glsk_pbuffer_t* result = (glsk_pbuffer_t*)malloc( sizeof( glsk_pbuffer_t ) );
	
	result->handle = 0;
	result->context = 0;
	result->owner = 0;
	
	result->width = 0;
	result->height = 0;
	
	return result;
}


void glsk_pbuffer_free( glsk_pbuffer_t* pbuffer )
{
	free( pbuffer );
}

int glsk_pbuffer_open( glsk_pbuffer_t* pbuffer, glsk_window_t* owner, glsk_context_t* context,
					glsk_pixelformat_t* pixelformat, int width, int height, int get_largest )
{
	int attribs[] = { GLX_PBUFFER_WIDTH, width,
					  GLX_PBUFFER_HEIGHT, height,
					  GLX_LARGEST_PBUFFER, get_largest,
					  GLX_PRESERVED_CONTENTS, 1, None };
					  
	GLXFBConfig fb_config = None;
					  
	if ( global.glx_fallback_mode || !context || !owner || !pbuffer )
		return FALSE;
	
	fb_config = _glsk_pixelformat_get_fbconfig( pixelformat );
	
	if ( fb_config == None )
		return FALSE;
	
	pbuffer->handle = glXCreatePbuffer( global.connection, fb_config, attribs );

	if ( pbuffer->handle == None )
		return FALSE;
	
	if ( context->handle == None )
	{
		// create the GL context
		context->handle = glXCreateNewContext( global.connection, fb_config, GLX_RGBA_TYPE, 0, TRUE );
	
		if ( context->handle == None )
			return 0;
		
		if ( !glXIsDirect( global.connection, context->handle ) )
			return 0;
	}
	
	context->refcount++;
	pbuffer->context = context;
	pbuffer->owner = owner;
	
	if ( get_largest )
	{
		unsigned int value = 0;

		glXQueryDrawable( global.connection, pbuffer->handle, GLX_PBUFFER_WIDTH, &value );
		pbuffer->width = value;
		
		glXQueryDrawable( global.connection, pbuffer->handle, GLX_PBUFFER_HEIGHT, &value );
		pbuffer->height = value;
	}
	else
	{
		pbuffer->width = width;
		pbuffer->height = height;
	}
	
	return TRUE;
}

void glsk_pbuffer_destroy( glsk_pbuffer_t* pbuffer )
{
	if ( glXGetCurrentDrawable() == pbuffer->handle ) 
	{
		glXMakeContextCurrent( global.connection, None, None, None );
	}

	pbuffer->context->refcount--;
	
	if ( pbuffer->context->refcount == 0 )
	{
		if ( glXGetCurrentContext() == pbuffer->context->handle )
			glXMakeContextCurrent( global.connection, None, None, None );
		
		// destroy our GL context
		glXDestroyContext( global.connection, pbuffer->context->handle );
		pbuffer->context->handle = None;
	}
	
	pbuffer->context = 0;
	pbuffer->owner = 0;
	
	pbuffer->width = pbuffer->height = 0;
	
	if ( pbuffer->handle )
		glXDestroyPbuffer( global.connection, pbuffer->handle );
	
	pbuffer->handle = None;
}

void glsk_pbuffer_swap_buffers( glsk_pbuffer_t* pbuffer )
{
	glXSwapBuffers( global.connection, pbuffer->handle );
}

int glsk_pbuffer_select_rendercontext( glsk_pbuffer_t* pbuffer )
{
	return glXMakeContextCurrent( global.connection, pbuffer->handle,
					pbuffer->handle, pbuffer->context->handle );
}

/*void						glsk_pbuffer_set_callback_damaged( glsk_pbuffer_t* pbuffer, glsk_callback_pbuffer_t callback );*/
