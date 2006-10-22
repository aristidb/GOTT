#include <windows.h>
#include <GL/wgl.h>
#include "os_gl.hpp"

namespace gott{namespace gui{namespace win32{

os_gl::os_gl()
  : choose_pixelformat(0), get_extension_string(0), create_pbuffer(0), destroy_pbuffer(0), get_pbuffer_dc(0), release_pbuffer_dc(0), query_pbuffer(0)
{
}

void os_gl::init()
{
  get_extension_string = (PFNWGLGETEXTENSIONSTRINGPROC)wglGetProcAddress( "wglGetExtensionsStringARB" );
	choose_pixelformat = (PFNWGLCHOOSEPIXELFORMATPROC)wglGetProcAddress( "wglChoosePixelFormatARB" );

	// those functions are essential
	if ( !ext->get_extension_string || !ext->choose_pixelformat )
    throw runtime_error("could not load wgl arb pixelformat extension");
    
	// pbuffer functions
	create_pbuffer = (PFNWGLCREATEPBUFFERPROC)wglGetProcAddress( "wglCreatePbufferARB" );
	destroy_pbuffer = (PFNWGLDESTROYPBUFFERPROC)wglGetProcAddress( "wglDestroyPbufferARB" );
	get_pbuffer_dc = (PFNWGLGETPBUFFERDCPROC)wglGetProcAddress( "wglGetPbufferDCARB" );
	release_pbuffer_dc = (PFNWGLRELEASEPBUFFERDCPROC)wglGetProcAddress( "wglReleasePbufferDCARB" );
	query_pbuffer = (PFNWGLQUERYPBUFFERPROC)wglGetProcAddress( "wglQueryPbufferARB" );
}


}}}

