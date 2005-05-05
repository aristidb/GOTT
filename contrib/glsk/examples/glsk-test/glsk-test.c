// $Id: glsk-test.c,v 1.2 2004/12/19 00:53:02 ltjax Exp $

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <glsk/glsk.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif gl

float roll = 0.f;

glsk_axial_device_t* mouse = 0;

// function to select the pixelformat/visual/fb_config
void pixelformat_selector( glsk_window_t* window, glsk_pixelformat_t* pf )
{
	glsk_pixelformat_set_flags( pf, GLSK_PF_DOUBLEBUFFER | GLSK_PF_DRAW_TO_WINDOW |
		GLSK_PF_COLOR_BITS | GLSK_PF_DEPTH_BITS | GLSK_PF_STENCIL_BITS | GLSK_PF_MULTISAMPLE );

	glsk_pixelformat_set_colorbits( pf, 32 );
	glsk_pixelformat_set_stencilbits( pf, 8 );
	glsk_pixelformat_set_depthbits( pf, 24 );
	glsk_pixelformat_set_samples( pf, 4 );
}

void update_scene( glsk_window_t* window )
{
	if ( glsk_keyboard_get_keystate( glsk_keyboard_get_keycode( "Backspace" ) ) )
		glsk_window_set_size( window, glsk_window_get_width( window )+5,
									glsk_window_get_height( window ) );
	
	if ( glsk_axial_device_get_button_state( mouse, 1 ) )
		glsk_main_quit();

	roll += 0.01f;
}

void render_scene( glsk_window_t* window )
{
	glsk_window_select_rendercontext( window );
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glLoadIdentity();
	glRotatef( roll, 0.f, 0.f, 1.f );

	glColor3ub( 0, 0, 0 );


	glBegin( GL_LINE_LOOP );

	glVertex3f( -5.f, -2.f, -10.f );
	glVertex3f( 5.f, -2.f, -10.f );
	glVertex3f( 0.f, 5.f, -10.f );

	glEnd();

	glsk_window_swap_buffers( window );
}

// handle mouse events
void mouse_handler( glsk_window_t* window, glsk_mouse_event_t* mouse_event )
{
	switch( glsk_mouse_event_get_type( mouse_event ) )
	{
	case GLSK_ME_TYPE_BUTTONPRESSED:
		if ( glsk_mouse_event_get_button( mouse_event ) == 3 )
			glsk_main_quit();

		break;
	default:
		break;
	};
}

// redraw the window area - just pipe this to render
void redraw_handler( glsk_window_t* window, void* pointer )
{
	render_scene( window );
}

void idle_handler( void* pointer )
{
	update_scene( (glsk_window_t*)pointer );
	render_scene( (glsk_window_t*)pointer );
}

int configure_handler( glsk_window_t* window, int x, int y )
{
	const float ratio = (float)x/y;

	glsk_window_select_rendercontext( window );	
	
	glClearColor( 0.86f, 0.93f, 1.f, 1.f );

	glViewport( 0, 0, x, y );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	gluPerspective( 90.f, ratio, 3.f, 10000.f );

	glMatrixMode( GL_MODELVIEW );

	glPointSize( 3.f );
	glLineWidth( 3.f );

	glEnable( GL_MULTISAMPLE_ARB );

	return 1;
}

int create_handler( glsk_window_t* window, int x, int y )
{
	glsk_window_select_rendercontext( window );

	return 1;
}

void char_handler( glsk_window_t* window, const char* string )
{
	if ( string[ 0 ] == 'a' )
	{
		glsk_window_set_decorated( window, 
			!glsk_window_get_decorated( window ) );
	}

	if ( string[ 0 ] == 'l' )
	{
		glsk_window_set_size( window, glsk_window_get_width( window )+5, glsk_window_get_height( window ) );
	}

	if ( string[ 0 ] == 'f' )
	{
		glsk_window_set_fullscreen( window, !glsk_window_get_fullscreen( window ) );
	}

	if ( string[ 0 ] == 'd' )
	{
		if ( glsk_keyboard_is_aquired() )
		{
			glsk_axial_device_unaquire( mouse );
			glsk_keyboard_unaquire();
		}
		else
		{
			glsk_keyboard_aquire();
			glsk_axial_device_aquire( mouse );
		}
	}

	if ( string[ 0 ] == 'm' )
	{
		int px, py;

		glsk_window_get_position( window, &px, &py );
		glsk_window_set_position( window, px+5, py+5 );
	}

	if ( string[ 0 ] == 'q' )
	{
		glsk_main_quit();
	}

}

int glsk_user_main( int argn, char** args )
{
	glsk_context_t* context = glsk_context_new();
	glsk_window_t* window = glsk_window_new();
	
	glsk_window_set_callback_create_event( window, &create_handler );
	glsk_window_set_callback_char_event( window, &char_handler );
	glsk_window_set_callback_configure_event( window, &configure_handler );
	glsk_window_set_callback_redraw_event( window, &redraw_handler );
	glsk_window_set_callback_pixelformat_select( window, &pixelformat_selector );
	glsk_window_set_callback_mouse_event( window, &mouse_handler );
	glsk_main_append_idle_handler( idle_handler, window );

	mouse = glsk_get_mouse();

	if ( glsk_window_open( window, context ) )
	{
		glsk_window_set_title( window, "Title" );
		glsk_window_set_decorated( window, 1 );
		glsk_window_set_position( window, 200, 150 );
		glsk_window_set_size( window, 400, 300 );

		glsk_window_show( window );

		return glsk_main_run();
	}
	else
	{
		glsk_window_free( window );
		glsk_error_box( 0, "unable to create glsk-window.\n" );

		return 0;
	}
}
