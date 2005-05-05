
/*
**
**		filename:		w_pixelformat.c
**
**		description:	pixelformat related functions
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
#include "w_pixelformat.h"




#define set_pf_value( a, b, v, c ) { (v)[ (c)++ ] = (a); (v)[ (c)++ ] = (b); }

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

void _glsk_pixelformat_init( glsk_pixelformat_t* pixelformat, int flags )
{
	pixelformat->flags = flags;
	pixelformat->colorbits = 0;
	pixelformat->depthbits = 0;
	pixelformat->stencilbits = 0;
	pixelformat->samples = 0;
}

static int _glsk_pixelformat_set_on_window_fallback( glsk_window_t* window, glsk_pixelformat_t* pixelformat, PIXELFORMATDESCRIPTOR* pfd )
{
	int		result_index = 0;

	pfd->nSize			= sizeof(PIXELFORMATDESCRIPTOR);
	pfd->nVersion		= 1;
	pfd->iLayerType		= PFD_MAIN_PLANE;
	pfd->dwFlags		= PFD_SUPPORT_OPENGL;
	pfd->iPixelType		= PFD_TYPE_RGBA;

	if ( pixelformat->flags & GLSK_PF_COLOR_BITS )
		pfd->cColorBits	= pixelformat->colorbits;
	else
		pfd->cColorBits = 16;

	if ( pixelformat->flags & GLSK_PF_DEPTH_BITS )
		pfd->cDepthBits = pixelformat->depthbits;
	else
		pfd->cDepthBits = 16;

	if ( pixelformat->flags & GLSK_PF_STENCIL_BITS )
		pfd->cStencilBits = pixelformat->stencilbits;
	else
        pfd->cStencilBits = 0;

	if ( pixelformat->flags & GLSK_PF_DRAW_TO_WINDOW )
		pfd->dwFlags |= PFD_DRAW_TO_WINDOW;

	if ( pixelformat->flags & GLSK_PF_DOUBLEBUFFER )
		pfd->dwFlags |= PFD_DOUBLEBUFFER;

	if ( pixelformat->flags & GLSK_PF_DRAW_TO_PBUFFER ) // this is not supported
		return 0;

	if ( pixelformat->flags & GLSK_PF_MULTISAMPLE )
		return 0;

	result_index = ChoosePixelFormat( window->dc, pfd );

	return ( SetPixelFormat( window->dc, result_index, pfd ) != 0 );
}

int _glsk_pixelformat_get_descriptor( HDC dc, glsk_pixelformat_t* pixelformat, int* descriptor )
{
	PIXELFORMATDESCRIPTOR	pfd;
	unsigned int			result_count = 0;
	int						values[ 24 ];
	int						c = 0;

	*descriptor = 0;

	ZeroMemory( &pfd, sizeof( pfd ) );

	// try the fallback if wglChoosePixelformat isnt loaded
	if ( global.function.wgl_choose_pixelformat == 0 )
		return 0;

	// set minimal requirements
	set_pf_value( WGL_ACCELERATION_ARB,		WGL_FULL_ACCELERATION_ARB, values, c );
	set_pf_value( WGL_PIXEL_TYPE_ARB,		WGL_TYPE_RGBA_ARB, values, c );
	set_pf_value( WGL_SUPPORT_OPENGL_ARB,	GL_TRUE, values, c );

    // set optional requirements

	if ( pixelformat->flags & GLSK_PF_DRAW_TO_WINDOW )
		set_pf_value( WGL_DRAW_TO_WINDOW_ARB,	GL_TRUE, values, c );

	if ( pixelformat->flags & GLSK_PF_DRAW_TO_PBUFFER )
	{
		if ( glsk_extensions_is_supported( "WGL_ARB_pbuffer" ) )
		{
			set_pf_value( WGL_DRAW_TO_PBUFFER_ARB,	GL_TRUE, values, c );
		}
		else
		{
			return FALSE;
		}
	}

	// FIXME: should the flag mean yes/no or yes/don't care?
	if ( pixelformat->flags & GLSK_PF_DOUBLEBUFFER )
		set_pf_value( WGL_DOUBLE_BUFFER_ARB,	GL_TRUE, values, c );

	if ( pixelformat->flags & GLSK_PF_MULTISAMPLE )
	{
		if ( glsk_extensions_is_supported( "GL_ARB_multisample" ) )
		{
			set_pf_value( WGL_SAMPLE_BUFFERS_ARB,	GL_TRUE, values, c );

			if ( pixelformat->samples > 0 )
				set_pf_value( WGL_SAMPLES_ARB, pixelformat->samples, values, c );
		}
		else
		{
			// FIXME: issue a warning?
		}
	}

	if ( pixelformat->flags & GLSK_PF_COLOR_BITS )
		set_pf_value( WGL_COLOR_BITS_ARB, pixelformat->colorbits, values, c );

	if ( pixelformat->flags & GLSK_PF_DEPTH_BITS )
		set_pf_value( WGL_DEPTH_BITS_ARB, pixelformat->depthbits, values, c );

	if ( pixelformat->flags & GLSK_PF_STENCIL_BITS )
		set_pf_value( WGL_STENCIL_BITS_ARB, pixelformat->stencilbits, values, c );

	// set the trailing 0
	values[ c ] = 0;

	return global.function.wgl_choose_pixelformat( dc, values, 0, 1, descriptor, &result_count ) &&
		result_count;	
}

int _glsk_pixelformat_set_on_window( glsk_window_t* window, glsk_pixelformat_t* pixelformat )
{
	int pixelformat_descriptor = 0;
	PIXELFORMATDESCRIPTOR	pfd;

	return ( ( _glsk_pixelformat_get_descriptor( window->dc, pixelformat, &pixelformat_descriptor ) &&
			 ( SetPixelFormat( window->dc, pixelformat_descriptor, &pfd ) != 0 ) )
			|| _glsk_pixelformat_set_on_window_fallback( window, pixelformat, &pfd ) );
}


