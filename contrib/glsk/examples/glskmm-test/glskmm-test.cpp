// $Id: glskmm-test.cpp,v 1.2 2004/12/19 00:53:02 ltjax Exp $
#include <GL/gl.h>
#include <GL/glu.h>
#include <glsk/glskmm.h>
#include <cmath>

class testwindow : public glsk::window
{
private:
	float				roll;
	bool				do_roll;
	
	float				radius;

	glsk::int2			center;
	glsk::axial_device	mouse;

public:
	testwindow( float radius );

	void		on_char( std::string& value );
	void		on_redraw();
	bool		on_configure( int x, int y );
	bool		on_create( int x, int y );
	void		on_pixelformat_select( glsk::pixelformat& format );
	void		on_mouse_event( const glsk::mouse_event& mouse_event );
	void		on_idle();

	void		update();
	void		render();
};

#include <iostream>

testwindow::testwindow( float radius )
: roll( 0 ), do_roll( false ), radius( radius )
{
	// get the system mouse
	mouse = glsk::get_mouse();

	// activate the idle handler
	set_idle_handler( true );
}

void testwindow::on_mouse_event( const glsk::mouse_event& mouse_event )
{
	switch( mouse_event.get_type() )
	{
	case glsk::mouse_event::move:
		{
			float dx = float( mouse_event.get_position()[ 0 ] - center[ 0 ] );
			float dy = float( mouse_event.get_position()[ 1 ] - center[ 1 ] );
			float length = dx*dx+dy*dy;

			if ( length > 0.5 )
			{
				length = sqrtf( length );

				roll = acosf( dx / length ) * 180.f / (acosf( 0.f )*2.f);

				if ( dy < 0.f )
					roll = -roll;
			}

		};
		break;

	case glsk::mouse_event::button_pressed:
	
		if ( mouse_event.get_button() == 3 )
			this->destroy();
		
		break;

	default:
		break;
	}
}

void testwindow::update()
{
	if ( glsk::keyboard::get_keystate( glsk::keyboard::get_keycode( "left" ) ) )
	{
		roll += 0.3f;
	}

	if ( glsk::keyboard::get_keystate( glsk::keyboard::get_keycode( "right" ) ) )
	{
		roll -= 0.3f;
	}
	
	if ( do_roll ) roll += .3f;
}

void testwindow::render()
{
	const float angle = 210.f/180.f * std::acos( 0.f ) * 2.f;
	
	const float h = std::sin( angle ) * radius;
	const float w = std::cos( angle ) * radius;
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glLoadIdentity();
	glRotatef( roll-90.f, 0.f, 0.f, 1.f );

	glColor3ub( 0, 0, 0 );

	glBegin( GL_LINE_LOOP );
		glVertex3f( w, h, -10.f );
		glVertex3f( -w, h, -10.f );
		glVertex3f( 0.f, radius, -10.f );
	glEnd();

	this->swap_buffers();
}

void testwindow::on_redraw()
{
	render();
}

void testwindow::on_idle()
{
	update();
	render();
}

bool testwindow::on_configure( int x, int y )
{
	glClearColor( 0.86f, 0.93f, 1.f, 1.f );

	// compute the center
	center.set( x/2, y/2 );

	glViewport( 0, 0, x, y );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	gluPerspective( 90.f, float(x)/y, 3.f, 10000.f );

	glMatrixMode( GL_MODELVIEW );

	glPointSize( 3.f );
	glLineWidth( 3.f );

	return true;
}

bool testwindow::on_create( int x, int y )
{
	return select_rendercontext();
}

void testwindow::on_pixelformat_select( glsk::pixelformat& format )
{
	format.set_flags( glsk::pf::draw_to_window | glsk::pf::doublebuffer |
		glsk::pf::color_bits_set | glsk::pf::depth_bits_set | glsk::pf::multisample );

	format.set_colorbits( 32 );
	format.set_depthbits( 16 );
	format.set_samples( 4 );
}

void testwindow::on_char( std::string& string )
{
	if ( string == "x" )
	{
		for ( int i = 0; i < glsk::extensions::get_count(); ++i )
			glsk::print( glsk::extensions::get( i ) + "\n" );
		
		return;
	}
	
	if ( string == "a" )
	{
		this->set_decorated( !this->get_decorated() );
		
		return;
	}

	if ( string == "d" )
	{
		if ( glsk::keyboard::is_aquired() || mouse.is_aquired() )
		{
			glsk::keyboard::unaquire();
			mouse.unaquire();
		}
		else
		{
			glsk::keyboard::aquire();
			mouse.aquire();
		}
		
		return;
	}
	
	if ( string == "y" )
	{
		this->set_decorated( true );
	}

	if ( string == "f" )
	{
		this->set_fullscreen( !this->get_fullscreen() );
		
		return;
	}

	if ( string == "q" )
	{
		this->destroy();
	}

	if ( string == "s" )
	{
		this->do_roll = !this->do_roll;
	}

	if ( string == "z" )
	{
		glsk::set_resolution( 800, 600, 32 );
	}

	if ( string == "u" )
	{
		glsk::set_resolution( 1024, 768, 32 );
	}
}

int glsk::user_main( int argc, char** argv )
{
	glsk::context		context;
	testwindow			wnd( 5.f );

	wnd.set_title( "Title" );
	wnd.set_decorated( false );
	wnd.set_size( 400, 300 );
	wnd.set_position( 200, 150 );

	if ( wnd.open( context ) && wnd.show() )
	{
		return glsk::main::run();
	}
	else
	{
		glsk::error_box( "unable to show window" );
	}
	
	return 0;
}
