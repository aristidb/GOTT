
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






#define set_pf_value( a, b, v, c ) { (v)[ (c)++ ] = (a); (v)[ (c)++ ] = (b); }

XVisualInfo* _glsk_pixelformat_get_visualinfo( glsk_pixelformat_t* pixelformat )
{
	int						values[ 24 ];
	int						c = 0;
	
	values[ c++ ] = GLX_RGBA;
	
	if ( pixelformat->flags & GLSK_PF_DOUBLEBUFFER )
		values[ c++ ] = GLX_DOUBLEBUFFER;
		
	if ( pixelformat->flags & GLSK_PF_COLOR_BITS )
	{
		if ( pixelformat->colorbits == 32 )
		{
			set_pf_value( GLX_RED_SIZE, 8 , values, c);
			set_pf_value( GLX_GREEN_SIZE, 8, values, c );
			set_pf_value( GLX_BLUE_SIZE, 8, values, c );
		}
		else if ( pixelformat->colorbits == 16 )
		{
			set_pf_value( GLX_RED_SIZE, 5, values, c );
			set_pf_value( GLX_GREEN_SIZE, 6, values, c );
			set_pf_value( GLX_BLUE_SIZE, 5, values, c );
		}
		else
		{
			return 0;
		}
	}
	
	values[ c++ ] = None;
	
	return glXChooseVisual( global.connection, global.screen, values );
}

GLXFBConfig	_glsk_pixelformat_get_fbconfig( glsk_pixelformat_t* pixelformat )
{
	GLXFBConfig*			Array = 0;
	GLXFBConfig				Result = 0;
	int						values[ 24 ];
	int						c = 0;
	int						bits = 0;
	int						elements = 0;

	if ( pixelformat->flags & GLSK_PF_DRAW_TO_WINDOW )
		bits |= GLX_WINDOW_BIT_SGIX;

	if ( pixelformat->flags & GLSK_PF_DRAW_TO_PBUFFER )
		bits |= GLX_PBUFFER_BIT_SGIX;

	set_pf_value( GLX_DRAWABLE_TYPE_SGIX, bits, values, c );

	if ( pixelformat->flags & GLSK_PF_DOUBLEBUFFER )
		set_pf_value( GLX_DOUBLEBUFFER, 1, values, c );

	if ( pixelformat->flags & GLSK_PF_COLOR_BITS )
	{
		if ( pixelformat->colorbits == 32 )
		{
			set_pf_value( GLX_RED_SIZE, 8 , values, c);
			set_pf_value( GLX_GREEN_SIZE, 8, values, c );
			set_pf_value( GLX_BLUE_SIZE, 8, values, c );
		}
		else if ( pixelformat->colorbits == 16 )
		{
			set_pf_value( GLX_RED_SIZE, 5, values, c );
			set_pf_value( GLX_GREEN_SIZE, 6, values, c );
			set_pf_value( GLX_BLUE_SIZE, 5, values, c );
		}
		else
		{
			return 0;
		}
	}

	if ( pixelformat->flags & GLSK_PF_DEPTH_BITS )
	{
		set_pf_value( GLX_DEPTH_SIZE, pixelformat->depthbits, values, c );
	}

	if ( pixelformat->flags & GLSK_PF_STENCIL_BITS )
	{
		set_pf_value( GLX_STENCIL_SIZE, pixelformat->stencilbits, values, c );
	}

	if ( pixelformat->flags & GLSK_PF_MULTISAMPLE )
	{
		if ( glsk_extensions_is_supported( "GL_ARB_multisample" ) )
		{
			set_pf_value( GLX_SAMPLE_BUFFERS_ARB, 1, values, c );
	
			if ( pixelformat->samples )
				set_pf_value( GLX_SAMPLES_ARB, pixelformat->samples, values, c );
		}
		else
		{
		}
	}

	values[ c++ ] = None;


	Array = glXChooseFBConfig( global.connection, global.screen, values, &elements );

	if ( elements )
	{
		Result = Array[ 0 ];
		XFree( Array );
		
		return Result;
	}

	return None;
}

glsk_pixelformat_t* glsk_pixelformat_new()
{
	glsk_pixelformat_t* pixelformat = (glsk_pixelformat_t*)malloc( sizeof( glsk_pixelformat_t ) );
	
	pixelformat->flags = 0;
	pixelformat->colorbits = 0;
	pixelformat->depthbits = 0;
	pixelformat->stencilbits = 0;
	pixelformat->samples = 0;
	
	return pixelformat;
}

void glsk_pixelformat_free( glsk_pixelformat_t* pixelformat )
{
	free( pixelformat );
}

void _glsk_pixelformat_init( glsk_pixelformat_t* pixelformat )
{
	pixelformat->flags = GLSK_PF_DRAW_TO_WINDOW;
	pixelformat->colorbits = 0;
	pixelformat->depthbits = 0;
	pixelformat->stencilbits = 0;
	pixelformat->samples = 0;
}


void glsk_pixelformat_set_flags( glsk_pixelformat_t* pixelformat, int flags )
{
	pixelformat->flags = flags;
}

void glsk_pixelformat_set_colorbits( glsk_pixelformat_t* pixelformat, unsigned int colorbits )
{
	pixelformat->colorbits = colorbits;
}

void glsk_pixelformat_set_depthbits( glsk_pixelformat_t* pixelformat, unsigned int depthbits )
{
	pixelformat->depthbits = depthbits;
}

void glsk_pixelformat_set_stencilbits( glsk_pixelformat_t* pixelformat, unsigned int stencilbits )
{
	pixelformat->stencilbits = stencilbits;
}

void glsk_pixelformat_set_samples( glsk_pixelformat_t* pixelformat, unsigned int samples )
{
	pixelformat->samples = samples;
}

int glsk_pixelformat_get_flags( glsk_pixelformat_t* pixelformat )
{
	return pixelformat->flags;
}

unsigned int glsk_pixelformat_get_colorbits( glsk_pixelformat_t* pixelformat )
{
	return pixelformat->colorbits;
}

unsigned int glsk_pixelformat_get_depthbits( glsk_pixelformat_t* pixelformat )
{
	return pixelformat->depthbits;
}

unsigned int glsk_pixelformat_get_stencilbits( glsk_pixelformat_t* pixelformat )
{
	return pixelformat->stencilbits;
}

unsigned int glsk_pixelformat_get_samples( glsk_pixelformat_t* pixelformat )
{
	return pixelformat->samples;
}
