
/*
**
**		filename:		x_pixelformat.h
**
**		description:	pixelformat/visual/fbconfig related functions
**		target-system:	X-Windows
**		version-info:   $Id$
**
*/


struct glsk_pixelformat_s
{
	int				flags;

	unsigned int	colorbits;
	unsigned int	depthbits;
	unsigned int	stencilbits;

	unsigned int	samples;
};


XVisualInfo*		_glsk_pixelformat_get_visualinfo( glsk_pixelformat_t* pixelformat );
GLXFBConfig			_glsk_pixelformat_get_fbconfig( glsk_pixelformat_t* pixelformat );
void				_glsk_pixelformat_init( glsk_pixelformat_t* pixelformat );

