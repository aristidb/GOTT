
/*
**
**		filename:		w_pixelformat.h
**
**		description:	pixelformat functions
**		target-system:	microsoft windows
**		version-info:   $Id$
**
*/


#include <gl/glext.h>

/*

	Internal Pixelformat Structure

*/

struct glsk_pixelformat_s
{
	int				flags;

	unsigned int	colorbits;
	unsigned int	depthbits;
	unsigned int	stencilbits;

	unsigned int	samples;
};

/*

	Internal Pixelformat Functions

*/


int					_glsk_pixelformat_get_descriptor( HDC dc, glsk_pixelformat_t* pixelformat, int* descriptor );
void				_glsk_pixelformat_init( glsk_pixelformat_t* pixelformat, int flags );
int					_glsk_pixelformat_set_on_window( glsk_window_t* window, glsk_pixelformat_t* pixelformat );
