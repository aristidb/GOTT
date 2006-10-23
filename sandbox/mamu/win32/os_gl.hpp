#include <windows.h>

namespace gott{namespace gui{namespace win32{

/**
 * Encapsulation of the operating system support library used with opengl
 * on win32, hides the fact that some of the functions are provided as extensions.
 */
struct os_gl
{
  typedef BOOL			(APIENTRY *PFNWGLCHOOSEPIXELFORMATPROC)( HDC, const int*, const FLOAT*, UINT, int*, UINT* );
  typedef const char*		(APIENTRY *PFNWGLGETEXTENSIONSTRINGPROC)( HDC );
  typedef HPBUFFER		(APIENTRY *PFNWGLCREATEPBUFFERPROC)( HDC, int, int, int, const int *);
  typedef HDC			 	(APIENTRY *PFNWGLGETPBUFFERDCPROC)( HPBUFFER );
  typedef int			  (APIENTRY *PFNWGLRELEASEPBUFFERDCPROC)( HPBUFFER, HDC );
  typedef BOOL			(APIENTRY *PFNWGLDESTROYPBUFFERPROC)( HPBUFFER );
  typedef BOOL			(APIENTRY *PFNWGLQUERYPBUFFERPROC)( HPBUFFER, int, int* );

  void init();
  /**
   * @name Basic WGL Functionality 
   */
  //@{
  PFNWGLCHOOSEPIXELFORMATPROC	choose_pixelformat;
  PFNWGLGETEXTENSIONSTRINGPROC get_extension_string;
  //@}


  /**
   * @name Pixelbuffer extensions
   */
  //@{
  PFNWGLCREATEPBUFFERPROC create_pbuffer;
  PFNWGLDESTROYPBUFFERPROC destroy_pbuffer;
  PFNWGLGETPBUFFERDCPROC get_pbuffer_dc;
  PFNWGLRELEASEPBUFFERDCPROC release_pbuffer_dc;
  PFNWGLQUERYPBUFFERPROC query_pbuffer;
  //@}
};

}}}
