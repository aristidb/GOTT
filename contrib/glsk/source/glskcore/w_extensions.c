
/*
**
**		filename:		w_extensions.c
**
**		description:	functions to load the extension functions
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

// a zero length string for return values
static const char* null_string = "";


int glsk_extensions_get_count()
{
	return global.extensions_count;
}

const char* glsk_extensions_get( int index )
{
	if ( (0 <= index) && (index < global.extensions_count) )
		return global.extensions[ index ];
	else
		return null_string;
}

int glsk_extensions_is_supported( const char* extension )
{
	int l = 0;
	int r = global.extensions_count-1;

	while ( l <= r )
	{
		int m = (l + r)/2;
		int result = _stricmp( extension, global.extensions[ m ] );

		if ( result < 0 )
			r = m - 1;
		else if ( result > 0 )
			l = m + 1;
		else
			return 1;
	}

	return 0;
}

int _glsk_count_words( const char* string )
{
	int result = 0;
	char last = ' ';

	if ( string == 0 )
		return 0;

	while ( *string != 0 )
	{
        if ( ( *string != ' ' ) && ( last == ' ' ) )
			++result;

		last = *string;
		++string;
	}

	return result;
}

int _glsk_string_compare( const void* a, const void* b )
{
	return strcmp( *(char**)a, *(char**)b );
}

int _glsk_init_extensions_string( glsk_window_t* window )
{
	size_t buffer_length = strlen( global.function.wgl_get_extension_string( window->dc ) ) +
		strlen( glGetString( GL_EXTENSIONS ) ) + 2;
	int index = 0;

	// reserve memory for the string
	global.extensions_buffer = (char*)malloc( buffer_length );

	// copy the extensions into the buffer
	strcpy( global.extensions_buffer, glGetString( GL_EXTENSIONS ) );
	strcat( global.extensions_buffer, " " );
	strcat( global.extensions_buffer, global.function.wgl_get_extension_string( window->dc ) );

	// count the number of words/extensions
	global.extensions_count = _glsk_count_words( global.extensions_buffer );

	if ( global.extensions_count )
	{
		char* string = global.extensions_buffer;
		char last = ' ';
		
		global.extensions = (char**)malloc( sizeof( char* ) * global.extensions_count );

		while ( *string != 0 )
		{
			if ( (last == ' ') && ( *string != ' ' ) )
				global.extensions[ index++ ] = string;
			
			last = *string;
			
			if ( *string == ' ' )
				*string = '\0';
			
			++string;
		}
	}

	// sort the array
	qsort( global.extensions, global.extensions_count, sizeof( char* ), _glsk_string_compare );

	return 1;
}




int _glsk_init_gl_extensions( glsk_window_t* window )
{
	global.function.gl_gen_queries = (PFNGLGENQUERIESPROC)wglGetProcAddress( "glGenQueriesARB" );
	global.function.gl_delete_queries = (PFNGLDELETEQUERIESPROC)wglGetProcAddress( "glDeleteQueriesARB" );
	global.function.gl_is_query = (PFNGLISQUERYPROC)wglGetProcAddress( "glIsQueryARB" );
	global.function.gl_begin_query = (PFNGLBEGINQUERYPROC)wglGetProcAddress( "glBeginQueryARB" );
	global.function.gl_end_query = (PFNGLENDQUERYPROC)wglGetProcAddress( "glEndQueryARB" );
	global.function.gl_get_query_iv = (PFNGLGETQUERYIV)wglGetProcAddress( "glGetQueryivARB" );
	global.function.gl_get_query_object_iv = (PFNGLGETQUERYOBJECTIV)wglGetProcAddress( "glGetQueryObjectivARB" );
	global.function.gl_get_query_object_uiv = (PFNGLGETQUERYOBJECTUIV)wglGetProcAddress( "glGetQueryObjectuivARB" );

	global.function.gl_active_texture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress( "glActiveTextureARB" );
	global.function.gl_client_active_texture = (PFNGLCLIENTACTIVETEXTUREPROC)wglGetProcAddress( "glClientActiveTextureARB" );
	global.function.gl_multi_texcoord_2d = (PFNGLMULTITEXCOORD2DARBPROC)wglGetProcAddress( "glMultiTexCoord2dARB" );
	global.function.gl_multi_texcoord_2dv = (PFNGLMULTITEXCOORD2DVARBPROC)wglGetProcAddress( "glMultiTexCoord2dvARB" );
	global.function.gl_multi_texcoord_2f = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress( "glMultiTexCoord2fARB" );
	global.function.gl_multi_texcoord_2fv = (PFNGLMULTITEXCOORD2FVARBPROC)wglGetProcAddress( "glMultiTexCoord2fvARB" );
	global.function.gl_multi_texcoord_2i = (PFNGLMULTITEXCOORD2IARBPROC)wglGetProcAddress( "glMultiTexCoord2iARB" );
	global.function.gl_multi_texcoord_2iv = (PFNGLMULTITEXCOORD2IVARBPROC)wglGetProcAddress( "glMultiTexCoord2ivARB" );
	global.function.gl_multi_texcoord_2s = (PFNGLMULTITEXCOORD2SARBPROC)wglGetProcAddress( "glMultiTexCoord2sARB" );
	global.function.gl_multi_texcoord_2sv = (PFNGLMULTITEXCOORD2SVARBPROC)wglGetProcAddress( "glMultiTexCoord2svARB" );
	global.function.gl_multi_texcoord_3d = (PFNGLMULTITEXCOORD3DARBPROC)wglGetProcAddress( "glMultiTexCoord3dARB" );
	global.function.gl_multi_texcoord_3dv = (PFNGLMULTITEXCOORD3DVARBPROC)wglGetProcAddress( "glMultiTexCoord3dvARB" );
	global.function.gl_multi_texcoord_3f = (PFNGLMULTITEXCOORD3FARBPROC)wglGetProcAddress( "glMultiTexCoord3fARB" );
	global.function.gl_multi_texcoord_3fv = (PFNGLMULTITEXCOORD3FVARBPROC)wglGetProcAddress( "glMultiTexCoord3fvARB" );
	global.function.gl_multi_texcoord_3i = (PFNGLMULTITEXCOORD3IARBPROC)wglGetProcAddress( "glMultiTexCoord3iARB" );
	global.function.gl_multi_texcoord_3iv = (PFNGLMULTITEXCOORD3IVARBPROC)wglGetProcAddress( "glMultiTexCoord3ivARB" );
	global.function.gl_multi_texcoord_3s = (PFNGLMULTITEXCOORD3SARBPROC)wglGetProcAddress( "glMultiTexCoord3sARB" );
	global.function.gl_multi_texcoord_3sv = (PFNGLMULTITEXCOORD3SVARBPROC)wglGetProcAddress( "glMultiTexCoord3svARB" );
	global.function.gl_multi_texcoord_4d = (PFNGLMULTITEXCOORD4DARBPROC)wglGetProcAddress( "glMultiTexCoord4dARB" );
	global.function.gl_multi_texcoord_4dv = (PFNGLMULTITEXCOORD4DVARBPROC)wglGetProcAddress( "glMultiTexCoord4dvARB" );
	global.function.gl_multi_texcoord_4f = (PFNGLMULTITEXCOORD4FARBPROC)wglGetProcAddress( "glMultiTexCoord4fARB" );
	global.function.gl_multi_texcoord_4fv = (PFNGLMULTITEXCOORD4FVARBPROC)wglGetProcAddress( "glMultiTexCoord4fvARB" );
	global.function.gl_multi_texcoord_4i = (PFNGLMULTITEXCOORD4IARBPROC)wglGetProcAddress( "glMultiTexCoord4iARB" );
	global.function.gl_multi_texcoord_4iv = (PFNGLMULTITEXCOORD4IVARBPROC)wglGetProcAddress( "glMultiTexCoord4ivARB" );
	global.function.gl_multi_texcoord_4s = (PFNGLMULTITEXCOORD4SARBPROC)wglGetProcAddress( "glMultiTexCoord4sARB" );
	global.function.gl_multi_texcoord_4sv = (PFNGLMULTITEXCOORD4SVARBPROC)wglGetProcAddress( "glMultiTexCoord4svARB" );

	global.function.gl_delete_object = (PFNGLHANDLEFUNCS)wglGetProcAddress( "glDeleteObjectARB" );
	global.function.gl_compile_shader = (PFNGLHANDLEFUNCS)wglGetProcAddress( "glCompileShaderARB" );
	global.function.gl_link_program = (PFNGLHANDLEFUNCS)wglGetProcAddress( "glLinkProgramARB" );
	global.function.gl_use_program_object = (PFNGLHANDLEFUNCS)wglGetProcAddress( "glUseProgramObjectARB" );
	global.function.gl_validate_program = (PFNGLHANDLEFUNCS)wglGetProcAddress( "glValidateProgramARB" );
	global.function.gl_get_handle = (PFNGLSOENUM)wglGetProcAddress( "glGetHandleARB" );
	global.function.gl_create_shader_object = (PFNGLSOENUM)wglGetProcAddress( "glCreateShaderObjectARB" );
	global.function.gl_detatch_object = (PFNGLDETATCHORATTACHOBJ)wglGetProcAddress( "glDetatchObjectARB" );
	global.function.gl_attach_object = (PFNGLDETATCHORATTACHOBJ)wglGetProcAddress( "glAttachObjectARB" );
	global.function.gl_shader_source = (PFNGLSHADERSOURCE)wglGetProcAddress( "glShaderSourceARB" );
	global.function.gl_create_program_object = (PFNGLCREATEPROGRAMOBJ)wglGetProcAddress( "glCreateProgramObjectARB" );
	global.function.gl_uniform_1f = (PFNGLUNIFORM1F)wglGetProcAddress( "glUniform1fARB" );
	global.function.gl_uniform_2f = (PFNGLUNIFORM2F)wglGetProcAddress( "glUniform2fARB" );
	global.function.gl_uniform_3f = (PFNGLUNIFORM3F)wglGetProcAddress( "glUniform3fARB" );
	global.function.gl_uniform_4f = (PFNGLUNIFORM4F)wglGetProcAddress( "glUniform4fARB" );
	global.function.gl_uniform_1i = (PFNGLUNIFORM1I)wglGetProcAddress( "glUniform1iARB" );
	global.function.gl_uniform_2i = (PFNGLUNIFORM2I)wglGetProcAddress( "glUniform2iARB" );
	global.function.gl_uniform_3i = (PFNGLUNIFORM3I)wglGetProcAddress( "glUniform3iARB" );
	global.function.gl_uniform_4i = (PFNGLUNIFORM4I)wglGetProcAddress( "glUniform4iARB" );
	global.function.gl_uniform_1fv = (PFNGLUNIFORMF)wglGetProcAddress( "glUniform1fvARB" );
	global.function.gl_uniform_2fv = (PFNGLUNIFORMF)wglGetProcAddress( "glUniform2fvARB" );
	global.function.gl_uniform_3fv = (PFNGLUNIFORMF)wglGetProcAddress( "glUniform3fvARB" );
	global.function.gl_uniform_4fv = (PFNGLUNIFORMF)wglGetProcAddress( "glUniform4fvARB" );
	global.function.gl_uniform_1iv = (PFNGLUNIFORMI)wglGetProcAddress( "glUniform1ivARB" );
	global.function.gl_uniform_2iv = (PFNGLUNIFORMI)wglGetProcAddress( "glUniform2ivARB" );
	global.function.gl_uniform_3iv = (PFNGLUNIFORMI)wglGetProcAddress( "glUniform3ivARB" );
	global.function.gl_uniform_4iv = (PFNGLUNIFORMI)wglGetProcAddress( "glUniform4ivARB" );
	global.function.gl_uniform_matrix_2fv = (PFNGLUNIFORMMATRIX)wglGetProcAddress( "glUniformMatrix2fvARB" );
	global.function.gl_uniform_matrix_3fv = (PFNGLUNIFORMMATRIX)wglGetProcAddress( "glUniformMatrix3fvARB" );
	global.function.gl_uniform_matrix_4fv = (PFNGLUNIFORMMATRIX)wglGetProcAddress( "glUniformMatrix4fvARB" );
	global.function.gl_get_object_parameter_1f = (PFNGLGETOBJECTPARAMETERF)wglGetProcAddress( "glGetObjectParameterfvARB" );
	global.function.gl_get_object_parameter_1i = (PFNGLGETOBJECTPARAMETERI)wglGetProcAddress( "glGetObjectParameterivARB" );
	global.function.gl_get_info_log = (PFNGLGETINFOLOGSHADERSOURCE)wglGetProcAddress( "glGetInfoLogARB" );
	global.function.gl_get_shader_source = (PFNGLGETINFOLOGSHADERSOURCE)wglGetProcAddress( "glGetShaderSourceARB" );
	global.function.gl_get_attached_objects = (PFNGLGETATTACHEDOBJS)wglGetProcAddress( "glGetAttachedObjectsARB" );
	global.function.gl_get_uniform_location = (PFNGLGETUNIFORMLOC)wglGetProcAddress( "glGetUniformLocationARB" );
	global.function.gl_get_active_uniform = (PFNGLGETACTIVEUNIFORM)wglGetProcAddress( "glGetActiveUniformARB" );
	global.function.gl_get_uniform_1f= (PFNGLGETUNIFORMF)wglGetProcAddress( "glGetUniformfvARB" );
	global.function.gl_get_uniform_1i= (PFNGLGETUNIFORMI)wglGetProcAddress( "glGetUniformivARB" );

	return 1;
}


int _glsk_init_wgl_extensions( glsk_window_t* window )
{
	glsk_window_select_rendercontext( window );
		
	// get the function address
	global.function.wgl_get_extension_string = (PFNWGLGETEXTENSIONSTRINGPROC)wglGetProcAddress( "wglGetExtensionsStringARB" );
	global.function.wgl_choose_pixelformat = (PFNWGLCHOOSEPIXELFORMATPROC)wglGetProcAddress( "wglChoosePixelFormatARB" );

	global.function.wgl_create_pbuffer = (PFNWGLCREATEPBUFFERPROC)wglGetProcAddress( "wglCreatePbufferARB" );
	global.function.wgl_destroy_pbuffer = (PFNWGLDESTROYPBUFFERPROC)wglGetProcAddress( "wglDestroyPbufferARB" );
	global.function.wgl_get_pbuffer_dc = (PFNWGLGETPBUFFERDCPROC)wglGetProcAddress( "wglGetPbufferDCARB" );
	global.function.wgl_release_pbuffer_dc = (PFNWGLRELEASEPBUFFERDCPROC)wglGetProcAddress( "wglReleasePbufferDCARB" );
	global.function.wgl_query_pbuffer = (PFNWGLQUERYPBUFFERPROC)wglGetProcAddress( "wglQueryPbufferARB" );



	return 1;
}


int _glsk_init_extensions()
{
	// create a dummy window to get the GL extensions
	glsk_context_t* context = glsk_context_new();
	glsk_window_t* window = glsk_window_new();

	// set sane default values before we can bump out
	global.extensions_buffer = 0;
	global.extensions_count = 0;
	global.extensions = 0;

	if ( window == 0 ) // error: window init failed
		return 0;

	if ( !glsk_window_open( window, context ) )
	{
		glsk_window_free( window );
		return 0;
	}


	// init the opengl extensions
	if ( !_glsk_init_wgl_extensions( window ) ||
		 !_glsk_init_gl_extensions( window ) )
		return 0;

	// init the extensions string
	if ( !_glsk_init_extensions_string( window ) )
		return 0;

	// cleanup that window
	glsk_window_destroy( window );
	_glsk_handle_events();

	glsk_window_free( window );
	glsk_context_free( context );

	return 1;
}

void _glsk_free_extensions()
{
	free( global.extensions );
	global.extensions = 0;

	free( global.extensions_buffer );
	global.extensions_buffer = 0;
}
