
/*
**
**    glsk - openGL Systems toolKit
**    for C
**
**    version-info:  $Id: glsk.h 95 2005-02-27 14:41:06Z jax $
**
*/


#ifdef WIN32
  #ifdef __cplusplus
    #define EXPORT extern "C" __declspec (dllexport)
  #else
    #define EXPORT __declspec (dllexport)
  #endif
#else
  #ifdef __cplusplus
    #define EXPORT extern "C"
  #else
    #define EXPORT
  #endif
#endif


/*

    Pixelformat Flags

*/

#define GLSK_PF_DRAW_TO_WINDOW      1
#define GLSK_PF_DRAW_TO_PBUFFER     2
#define GLSK_PF_DOUBLEBUFFER        4
#define GLSK_PF_MULTISAMPLE         8
#define GLSK_PF_COLOR_BITS          16
#define GLSK_PF_DEPTH_BITS          32
#define GLSK_PF_STENCIL_BITS        64

/*

    Mouseevent Type

*/

#define GLSK_ME_TYPE_MOVE           1
#define GLSK_ME_TYPE_BUTTONPRESSED  2
#define GLSK_ME_TYPE_BUTTONRELEASED 3


/*

    Forward Definitions

*/

typedef struct glsk_pixelformat_s           glsk_pixelformat_t;
typedef struct glsk_idle_handler_s          glsk_idle_handler_t;
typedef struct glsk_window_s                glsk_window_t;
typedef struct glsk_mouse_event_s           glsk_mouse_event_t;
typedef struct glsk_axial_device_s          glsk_axial_device_t;
typedef struct glsk_pbuffer_s				glsk_pbuffer_t;
typedef struct glsk_context_s				glsk_context_t;


/*

    Callback Types

*/

typedef void (*glsk_callback_window_voidp_t)( glsk_window_t*, void* );
typedef void (*glsk_callback_window_string_t)( glsk_window_t*, const char* );
typedef int  (*glsk_callback_window_coord_t)( glsk_window_t*, int, int );
typedef void (*glsk_callback_window_pixelformat_t)( glsk_window_t*, glsk_pixelformat_t* );
typedef void (*glsk_callback_window_mouse_event_t)( glsk_window_t*, glsk_mouse_event_t* );
typedef void (*glsk_callback_voidp_t)( void* );
typedef void (*glsk_callback_pbuffer_t)( glsk_pbuffer_t* );


/*

    General Functions

*/

EXPORT int                      glsk_set_resolution( int width, int height, int bpp );
EXPORT void                     glsk_error_box( glsk_window_t* parent, const char* text );
EXPORT void						glsk_print( const char* string );
EXPORT glsk_axial_device_t*		glsk_get_mouse();


/*

    Extensions Functions

*/

EXPORT int                      glsk_extensions_get_count();
EXPORT const char*              glsk_extensions_get( int index );
EXPORT int                      glsk_extensions_is_supported( const char* extension );

/*

	Render Context Functions

*/

EXPORT glsk_context_t*			glsk_context_new();
EXPORT void						glsk_context_free( glsk_context_t* context );


/*

    Mainloop Functions

    these functions control the main loop

*/

EXPORT int                      glsk_main_run();
EXPORT void                     glsk_main_quit();
EXPORT glsk_idle_handler_t*     glsk_main_append_idle_handler( glsk_callback_voidp_t callback, void* context );
EXPORT void                     glsk_main_remove_idle_handler( glsk_idle_handler_t* node );


// window creation & destruction
EXPORT glsk_window_t*           glsk_window_new();
EXPORT void                     glsk_window_free( glsk_window_t* window );

EXPORT int                      glsk_window_open( glsk_window_t* window, glsk_context_t* context );
EXPORT void                     glsk_window_destroy( glsk_window_t* window );

// OpenGL related
EXPORT void                     glsk_window_swap_buffers( glsk_window_t* window );
EXPORT int                      glsk_window_select_rendercontext( glsk_window_t* window );

EXPORT int                      glsk_window_show( glsk_window_t* window );
EXPORT void                     glsk_window_hide( glsk_window_t* window );
EXPORT int                      glsk_window_is_visible( glsk_window_t* window );

EXPORT const char*              glsk_window_get_title( glsk_window_t* window );
EXPORT void                     glsk_window_get_size( glsk_window_t* window, int* width, int* height );
EXPORT int                      glsk_window_get_width( glsk_window_t* window );
EXPORT int                      glsk_window_get_height( glsk_window_t* window );
EXPORT void                     glsk_window_get_position( glsk_window_t* window, int* px, int* py );
EXPORT int                      glsk_window_get_decorated( glsk_window_t* window );
EXPORT void*                    glsk_window_get_userdata( glsk_window_t* window );
EXPORT int                      glsk_window_get_fullscreen( glsk_window_t* window );

EXPORT void                     glsk_window_set_title( glsk_window_t* window, const char* title );
EXPORT void                     glsk_window_set_size( glsk_window_t* window, int width, int height );
EXPORT void                     glsk_window_set_position( glsk_window_t* window, int x, int y );
EXPORT void                     glsk_window_set_width( glsk_window_t* window, int width );
EXPORT void                     glsk_window_set_height( glsk_window_t* window, int height );
EXPORT void                     glsk_window_set_decorated( glsk_window_t* window, int value );
EXPORT void                     glsk_window_set_userdata( glsk_window_t* window, void* userdata );
EXPORT void                     glsk_window_set_fullscreen( glsk_window_t* window, int value );

EXPORT void                     glsk_window_set_callback_char_event( glsk_window_t* window, glsk_callback_window_string_t callback );
EXPORT void                     glsk_window_set_callback_redraw_event( glsk_window_t* window, glsk_callback_window_voidp_t callback );
EXPORT void                     glsk_window_set_callback_configure_event( glsk_window_t* window, glsk_callback_window_coord_t callback );
EXPORT void                     glsk_window_set_callback_create_event( glsk_window_t* window, glsk_callback_window_coord_t callback );
EXPORT void                     glsk_window_set_callback_close_event( glsk_window_t* window, glsk_callback_window_voidp_t callback );
EXPORT void                     glsk_window_set_callback_destroy_event( glsk_window_t* window, glsk_callback_window_voidp_t callback );
EXPORT void                     glsk_window_set_callback_pixelformat_select( glsk_window_t* window, glsk_callback_window_pixelformat_t callback );
EXPORT void                     glsk_window_set_callback_mouse_event( glsk_window_t* window, glsk_callback_window_mouse_event_t callback );


/*

    Pixelformat Functions

    these functions are used to read and modify the pixelformat structure

*/

EXPORT glsk_pixelformat_t*		glsk_pixelformat_new();
EXPORT void						glsk_pixelformat_free( glsk_pixelformat_t* pixelformat );

EXPORT void                     glsk_pixelformat_set_flags( glsk_pixelformat_t* pixelformat, int flags );
EXPORT void                     glsk_pixelformat_set_colorbits( glsk_pixelformat_t* pixelformat, unsigned int colorbits );
EXPORT void                     glsk_pixelformat_set_depthbits( glsk_pixelformat_t* pixelformat, unsigned int depthbits );
EXPORT void                     glsk_pixelformat_set_stencilbits( glsk_pixelformat_t* pixelformat, unsigned int stencilbits );
EXPORT void                     glsk_pixelformat_set_samples( glsk_pixelformat_t* pixelformat, unsigned int samples );

EXPORT int                      glsk_pixelformat_get_flags( glsk_pixelformat_t* pixelformat );
EXPORT unsigned int             glsk_pixelformat_get_colorbits( glsk_pixelformat_t* pixelformat );
EXPORT unsigned int             glsk_pixelformat_get_depthbits( glsk_pixelformat_t* pixelformat );
EXPORT unsigned int             glsk_pixelformat_get_stencilbits( glsk_pixelformat_t* pixelformat );
EXPORT unsigned int             glsk_pixelformat_get_samples( glsk_pixelformat_t* pixelformat );


/*

    Mouse Event Functions

    these functions are used to read the mouse event structure

*/


EXPORT void                     glsk_mouse_event_get_position( glsk_mouse_event_t* mouse_event, int* px, int* py );
EXPORT int                      glsk_mouse_event_get_type( glsk_mouse_event_t* mouse_event );
EXPORT int                      glsk_mouse_event_get_button( glsk_mouse_event_t* mouse_event );


/*

    Axial-Device Query Functions
    
    unified interface to axial input devices (Mouse/Joystick etc)
    
*/


EXPORT int                      glsk_axial_device_get_vector_count( glsk_axial_device_t* device );
EXPORT void                     glsk_axial_device_get_vector_state( glsk_axial_device_t* device, int index, int* px, int* py );
EXPORT int                      glsk_axial_device_get_button_count( glsk_axial_device_t* device );
EXPORT int                      glsk_axial_device_get_button_state( glsk_axial_device_t* device, int button );
EXPORT void                     glsk_axial_device_mark_read( glsk_axial_device_t* device );
EXPORT int                      glsk_axial_device_is_read( glsk_axial_device_t* device );
EXPORT int						glsk_axial_device_aquire( glsk_axial_device_t* device );
EXPORT void						glsk_axial_device_unaquire( glsk_axial_device_t* device );
EXPORT int						glsk_axial_device_is_aquired( glsk_axial_device_t* device );


/*

	Keyboard State Query Functions

*/

#define	GLSK_KEY_HELD			2
#define GLSK_KEY_HIT			1

EXPORT int						glsk_keyboard_get_keystate( int keycode );
EXPORT int						glsk_keyboard_get_keycode( const char* name );
EXPORT const char*				glsk_keyboard_get_keyname( int keycode );
EXPORT int						glsk_keyboard_aquire();
EXPORT void						glsk_keyboard_unaquire();
EXPORT int						glsk_keyboard_is_aquired();


/*

	Pbuffer Functions

*/

EXPORT glsk_pbuffer_t*			glsk_pbuffer_new();
EXPORT void						glsk_pbuffer_free( glsk_pbuffer_t* pbuffer );

EXPORT int						glsk_pbuffer_open( glsk_pbuffer_t* pbuffer, glsk_window_t* owner, glsk_context_t* context,
												  glsk_pixelformat_t* pixelformat, int width, int height, int get_largest );
EXPORT void						glsk_pbuffer_destroy( glsk_pbuffer_t* pbuffer );

EXPORT void						glsk_pbuffer_swap_buffers( glsk_pbuffer_t* pbuffer );
EXPORT int						glsk_pbuffer_select_rendercontext( glsk_pbuffer_t* pbuffer );
EXPORT void						glsk_pbuffer_set_callback_damaged( glsk_pbuffer_t* pbuffer, glsk_callback_pbuffer_t callback );
