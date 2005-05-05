
/*
**
**		filename:		w_extensions.h
**
**		description:	functions to load the extension functions
**		target-system:	microsoft windows
**		version-info:   $Id$
**
*/

int						_glsk_init_extensions();
void					_glsk_free_extensions();

typedef char	GLcharARB;
typedef int		GLhandleARB;

DECLARE_HANDLE(HPBUFFER);

typedef BOOL			(APIENTRY *PFNWGLCHOOSEPIXELFORMATPROC)( HDC, const int*, const FLOAT*, UINT, int*, UINT* );

typedef const char*		(APIENTRY *PFNWGLGETEXTENSIONSTRINGPROC)( HDC );

typedef HPBUFFER		(APIENTRY *PFNWGLCREATEPBUFFERPROC)( HDC, int, int, int, const int *);
typedef HDC				(APIENTRY *PFNWGLGETPBUFFERDCPROC)( HPBUFFER );
typedef int				(APIENTRY *PFNWGLRELEASEPBUFFERDCPROC)( HPBUFFER, HDC );
typedef BOOL			(APIENTRY *PFNWGLDESTROYPBUFFERPROC)( HPBUFFER );
typedef BOOL			(APIENTRY *PFNWGLQUERYPBUFFERPROC)( HPBUFFER, int, int* );


typedef void			(APIENTRY* PFNGLGENQUERIESPROC)( GLsizei, GLuint*);
typedef void			(APIENTRY* PFNGLDELETEQUERIESPROC)( GLsizei, const GLuint* );
typedef GLboolean		(APIENTRY* PFNGLISQUERYPROC)( GLuint );
typedef void			(APIENTRY* PFNGLBEGINQUERYPROC)( GLenum, GLuint );
typedef void			(APIENTRY* PFNGLENDQUERYPROC)( GLenum );
typedef void			(APIENTRY* PFNGLGETQUERYIV)( GLenum, GLenum, GLint* );
typedef void			(APIENTRY* PFNGLGETQUERYOBJECTIV)( GLuint, GLenum, GLint* );
typedef void			(APIENTRY* PFNGLGETQUERYOBJECTUIV)( GLuint, GLenum, GLuint* ); 

typedef void			(APIENTRY* PFNGLHANDLEFUNCS) (GLhandleARB);
typedef GLhandleARB		(APIENTRY* PFNGLSOENUM) (GLenum); //covers gethandle and createshaderobject
typedef void			(APIENTRY* PFNGLDETATCHORATTACHOBJ) (GLhandleARB, GLhandleARB);
typedef void			(APIENTRY* PFNGLSHADERSOURCE) (GLhandleARB, GLsizei, const GLcharARB**, const GLint*);
typedef GLhandleARB		(APIENTRY* PFNGLCREATEPROGRAMOBJ) (void);
typedef void			(APIENTRY* PFNGLUNIFORM1F) (GLint, GLfloat);
typedef void			(APIENTRY* PFNGLUNIFORM2F) (GLint, GLfloat, GLfloat);
typedef void			(APIENTRY* PFNGLUNIFORM3F) (GLint, GLfloat, GLfloat, GLfloat);
typedef void			(APIENTRY* PFNGLUNIFORM4F) (GLint, GLfloat, GLfloat, GLfloat, GLfloat);
typedef void			(APIENTRY* PFNGLUNIFORM1I) (GLint, GLint);
typedef void			(APIENTRY* PFNGLUNIFORM2I) (GLint, GLint, GLint);
typedef void			(APIENTRY* PFNGLUNIFORM3I) (GLint, GLint, GLint, GLint);
typedef void			(APIENTRY* PFNGLUNIFORM4I) (GLint, GLint, GLint, GLint, GLint);
typedef void			(APIENTRY* PFNGLUNIFORMF) (GLint, GLsizei, const GLfloat*);
typedef void			(APIENTRY* PFNGLUNIFORMI) (GLint, GLsizei, const GLint*);
typedef void			(APIENTRY* PFNGLUNIFORMMATRIX) (GLint, GLsizei, GLboolean, const GLfloat*);
typedef void			(APIENTRY* PFNGLGETOBJECTPARAMETERF) (GLhandleARB, GLenum, GLfloat*);
typedef void			(APIENTRY* PFNGLGETOBJECTPARAMETERI) (GLhandleARB, GLenum, GLint*);
typedef void			(APIENTRY* PFNGLGETINFOLOGSHADERSOURCE) (GLhandleARB, GLsizei, GLsizei*, GLcharARB*);//covers getshadersource and getinfolog
typedef void			(APIENTRY* PFNGLGETATTACHEDOBJS) (GLhandleARB, GLsizei, GLsizei*, GLhandleARB*);
typedef GLint			(APIENTRY* PFNGLGETUNIFORMLOC) (GLhandleARB, const GLcharARB*);
typedef void			(APIENTRY* PFNGLGETACTIVEUNIFORM) (GLhandleARB, GLuint, GLsizei, GLsizei*, GLint*, GLenum*, GLcharARB*);
typedef void			(APIENTRY* PFNGLGETUNIFORMF) (GLhandleARB, GLint, GLfloat*);
typedef void			(APIENTRY* PFNGLGETUNIFORMI) (GLhandleARB, GLint, GLint*);

// WGL_PIXELFORMAT defines
#define WGL_DRAW_TO_WINDOW_ARB			0x2001
//#define WGL_DRAW_TO_BITMAP_ARB		0x2002
#define WGL_DRAW_TO_PBUFFER_ARB			0x202D
#define WGL_ACCELERATION_ARB			0x2003
#define	WGL_SUPPORT_OPENGL_ARB			0x2010
#define WGL_DOUBLE_BUFFER_ARB			0x2011
#define	WGL_COLOR_BITS_ARB				0x2014
#define	WGL_PIXEL_TYPE_ARB				0x2013
#define WGL_DEPTH_BITS_ARB				0x2022
#define WGL_STENCIL_BITS_ARB			0x2023
#define WGL_SAMPLE_BUFFERS_ARB			0x2041
#define WGL_SAMPLES_ARB					0x2042

//#define WGL_NO_ACCELERATION_ARB		0x2025
//#define WGL_GENERIC_ACCELERATION_ARB	0x2026
#define WGL_FULL_ACCELERATION_ARB		0x2027
#define WGL_TYPE_RGBA_ARB				0x202B

#define WGL_PBUFFER_LARGEST_ARB         0x2033

typedef struct _glsk_extensions_s
{
	PFNWGLCHOOSEPIXELFORMATPROC		wgl_choose_pixelformat;
	PFNWGLGETEXTENSIONSTRINGPROC	wgl_get_extension_string;

	PFNWGLCREATEPBUFFERPROC			wgl_create_pbuffer;
	PFNWGLDESTROYPBUFFERPROC		wgl_destroy_pbuffer;
	PFNWGLGETPBUFFERDCPROC			wgl_get_pbuffer_dc;
	PFNWGLRELEASEPBUFFERDCPROC		wgl_release_pbuffer_dc;
	PFNWGLQUERYPBUFFERPROC			wgl_query_pbuffer;

	PFNGLGENQUERIESPROC				gl_gen_queries;
	PFNGLDELETEQUERIESPROC			gl_delete_queries;
	PFNGLISQUERYPROC				gl_is_query;
	PFNGLBEGINQUERYPROC				gl_begin_query;
	PFNGLENDQUERYPROC				gl_end_query;
	PFNGLGETQUERYIV					gl_get_query_iv;
	PFNGLGETQUERYOBJECTIV			gl_get_query_object_iv;
	PFNGLGETQUERYOBJECTUIV			gl_get_query_object_uiv;

	PFNGLACTIVETEXTUREPROC			gl_active_texture;
	PFNGLCLIENTACTIVETEXTUREPROC	gl_client_active_texture;
	PFNGLMULTITEXCOORD2DARBPROC		gl_multi_texcoord_2d;
	PFNGLMULTITEXCOORD2DVARBPROC	gl_multi_texcoord_2dv;
	PFNGLMULTITEXCOORD2FARBPROC		gl_multi_texcoord_2f;
	PFNGLMULTITEXCOORD2FVARBPROC	gl_multi_texcoord_2fv;
	PFNGLMULTITEXCOORD2IARBPROC		gl_multi_texcoord_2i;
	PFNGLMULTITEXCOORD2IVARBPROC	gl_multi_texcoord_2iv;
	PFNGLMULTITEXCOORD2SARBPROC		gl_multi_texcoord_2s;
	PFNGLMULTITEXCOORD2SVARBPROC	gl_multi_texcoord_2sv;
	PFNGLMULTITEXCOORD3DARBPROC		gl_multi_texcoord_3d;
	PFNGLMULTITEXCOORD3DVARBPROC	gl_multi_texcoord_3dv;
	PFNGLMULTITEXCOORD3FARBPROC		gl_multi_texcoord_3f;
	PFNGLMULTITEXCOORD3FVARBPROC	gl_multi_texcoord_3fv;
	PFNGLMULTITEXCOORD3IARBPROC		gl_multi_texcoord_3i;
	PFNGLMULTITEXCOORD3IVARBPROC	gl_multi_texcoord_3iv;
	PFNGLMULTITEXCOORD3SARBPROC		gl_multi_texcoord_3s;
	PFNGLMULTITEXCOORD3SVARBPROC	gl_multi_texcoord_3sv;
	PFNGLMULTITEXCOORD4DARBPROC		gl_multi_texcoord_4d;
	PFNGLMULTITEXCOORD4DVARBPROC	gl_multi_texcoord_4dv;
	PFNGLMULTITEXCOORD4FARBPROC		gl_multi_texcoord_4f;
	PFNGLMULTITEXCOORD4FVARBPROC	gl_multi_texcoord_4fv;
	PFNGLMULTITEXCOORD4IARBPROC		gl_multi_texcoord_4i;
	PFNGLMULTITEXCOORD4IVARBPROC	gl_multi_texcoord_4iv;
	PFNGLMULTITEXCOORD4SARBPROC		gl_multi_texcoord_4s;
	PFNGLMULTITEXCOORD4SVARBPROC	gl_multi_texcoord_4sv;

	PFNGLHANDLEFUNCS				gl_delete_object;
	PFNGLHANDLEFUNCS				gl_compile_shader;
	PFNGLHANDLEFUNCS				gl_link_program;
	PFNGLHANDLEFUNCS				gl_use_program_object;
	PFNGLHANDLEFUNCS				gl_validate_program;
	PFNGLSOENUM						gl_get_handle;
	PFNGLSOENUM						gl_create_shader_object;
	PFNGLDETATCHORATTACHOBJ			gl_detatch_object;
	PFNGLDETATCHORATTACHOBJ			gl_attach_object;
	PFNGLSHADERSOURCE				gl_shader_source;
	PFNGLCREATEPROGRAMOBJ			gl_create_program_object;
	PFNGLUNIFORM1F					gl_uniform_1f;
	PFNGLUNIFORM2F					gl_uniform_2f;
	PFNGLUNIFORM3F					gl_uniform_3f;
	PFNGLUNIFORM4F					gl_uniform_4f;
	PFNGLUNIFORM1I					gl_uniform_1i;
	PFNGLUNIFORM2I					gl_uniform_2i;
	PFNGLUNIFORM3I					gl_uniform_3i;
	PFNGLUNIFORM4I					gl_uniform_4i;
	PFNGLUNIFORMF					gl_uniform_1fv;
	PFNGLUNIFORMF					gl_uniform_2fv;
	PFNGLUNIFORMF					gl_uniform_3fv;
	PFNGLUNIFORMF					gl_uniform_4fv;
	PFNGLUNIFORMI					gl_uniform_1iv;
	PFNGLUNIFORMI					gl_uniform_2iv;
	PFNGLUNIFORMI					gl_uniform_3iv;
	PFNGLUNIFORMI					gl_uniform_4iv;
	PFNGLUNIFORMMATRIX				gl_uniform_matrix_2fv;
	PFNGLUNIFORMMATRIX				gl_uniform_matrix_3fv;
	PFNGLUNIFORMMATRIX				gl_uniform_matrix_4fv;
	PFNGLGETOBJECTPARAMETERF		gl_get_object_parameter_1f;
	PFNGLGETOBJECTPARAMETERI		gl_get_object_parameter_1i;
	PFNGLGETINFOLOGSHADERSOURCE		gl_get_info_log;
	PFNGLGETINFOLOGSHADERSOURCE		gl_get_shader_source;
	PFNGLGETATTACHEDOBJS			gl_get_attached_objects;
	PFNGLGETUNIFORMLOC				gl_get_uniform_location;
	PFNGLGETACTIVEUNIFORM			gl_get_active_uniform;
	PFNGLGETUNIFORMF				gl_get_uniform_1f;
	PFNGLGETUNIFORMI				gl_get_uniform_1i;
}
_glsk_extensions_t;
