
/*
**
**		filename:		glskmm.cpp
**
**		description:	glsk c++ wrapper
**		target-system:	cross-platform
**		version-info:   $Id$
**
*/

#include <glsk/glsk.h>
#include <glsk/glskmm.h>

/** \fn void glsk::drawable::swap_buffers()
	Swap the rendering buffers.
	If double buffering is selected, this swaps the front and back buffers, making eventual changes visible.
	\see pixelformat::doublebuffer
*/

/**	\fn bool glsk::drawable::select_rendercontext()
	Select this rendercontext.
	Makes this drawable's rendercontext the active one, directing all OpenGL commands to it afterwards
*/

class glsk::pixelformat::internal
{
public:
	glsk_pixelformat_t*	object;
	bool				owner;

	internal() : object( 0 ), owner( false ) {}
};

class glsk::context::internal
{
public:
	glsk_context_t*		object;
	int					refcount;

	internal() : object( 0 ), refcount( 0 ) {}
};

class glsk::window::internal
{
public:
	static void	on_char_event( glsk_window_t* window, const char* string );
	static void	on_redraw_event( glsk_window_t* window, void* pointer );
	static int	on_configure_event( glsk_window_t* window, int x, int y );
	static int	on_create_event( glsk_window_t* window, int x, int y );
	static void	on_close_event( glsk_window_t* window, void* pointer );
	static void	on_destroy_event( glsk_window_t* window, void* pointer );
	static void	on_pixelformat_select( glsk_window_t* window, glsk_pixelformat_t* pixelformat );
	static void on_mouse_event( glsk_window_t* window, glsk_mouse_event_t* mouse_event );
	static void	on_idle( void* context );

#ifndef _glskmm_no_sigc_
	sigc::signal< void, std::string& >			signal_char;
	sigc::signal< void >						signal_redraw;
	sigc::signal< bool, int, int >				signal_configure;
	sigc::signal< bool, int, int >				signal_create;
	sigc::signal< void >						signal_close;
	sigc::signal< void >						signal_destroy;
	sigc::signal< void >						signal_idle;
	sigc::signal< void, glsk::pixelformat& >	signal_pixelformat_select;
	sigc::signal< void, glsk::mouse_event& >	signal_mouse_event;
#endif

	glsk_window_t*								object;
	glsk_idle_handler_t*						idle_handler;
	unsigned int								references;
};



class glsk::internal
{
	public:
	inline static axial_device create( glsk_axial_device_t* object )
	{
		return axial_device( reinterpret_cast< void* >( object ) );
	}
	
	inline static glsk_pixelformat_t* get_object( glsk::pixelformat& pixelformat )
	{
		return pixelformat.data->object;
	}
	
	inline static glsk_window_t* get_object( glsk::window& window )
	{
		return window.data->object;
	}
	
	inline static glsk_context_t* get_object( glsk::context& context )
	{
		return context.data->object;
	}
};

/*

	static message handlers - these route the events to the given object

*/

void
glsk::window::internal::on_char_event( glsk_window_t* object, const char* string )
{
	glsk::window*		window = reinterpret_cast< glsk::window* >( glsk_window_get_userdata( object ) );
	std::string			value( string );

	window->on_char( value );

#ifndef _glskmm_no_sigc_
	window->data->signal_char.emit( value );
#endif
}

void
glsk::window::internal::on_redraw_event( glsk_window_t* object, void* pointer )
{
	glsk::window*		window = reinterpret_cast< glsk::window* >( glsk_window_get_userdata( object ) );

	window->on_redraw();
#ifndef _glskmm_no_sigc_
	window->signal_redraw();
#endif
}

int
glsk::window::internal::on_configure_event( glsk_window_t* object, int x, int y )
{
	glsk::window*		window = reinterpret_cast< glsk::window* >( glsk_window_get_userdata( object ) );

	if ( window->on_configure( x, y ) )
	{
#ifndef _glskmm_no_sigc_
		//TODO: use the return value
		window->data->signal_configure.emit( x, y );
#endif

		return 1;
	}
	else
	{
		return 0;
	}
}

int
glsk::window::internal::on_create_event( glsk_window_t* object, int x, int y )
{
	glsk::window*		window = reinterpret_cast< glsk::window* >( glsk_window_get_userdata( object ) );

	if ( window->on_create( x, y ) )
	{ 
#ifndef _glskmm_no_sigc_
		// TODO: use the return value
		window->data->signal_create.emit( x, y );
#endif
		return 1;
	}
	else
	{
		return 0;
	}
}

void
glsk::window::internal::on_close_event( glsk_window_t* object, void* pointer )
{
	glsk::window*		window = reinterpret_cast< glsk::window* >( glsk_window_get_userdata( object ) );

	window->on_close();
#ifndef _glskmm_no_sigc_
	window->data->signal_close.emit();
#endif
}

void
glsk::window::internal::on_destroy_event( glsk_window_t* object, void* pointer )
{
	glsk::window*		window = reinterpret_cast< glsk::window* >( glsk_window_get_userdata( object ) );

	window->on_destroy();
#ifndef _glskmm_no_sigc_
	window->data->signal_destroy.emit();
#endif
}

void
glsk::window::internal::on_pixelformat_select( glsk_window_t* object, glsk_pixelformat_t* pixelformat )
{
	glsk::window*		window = reinterpret_cast< glsk::window* >( glsk_window_get_userdata( object ) );
	glsk::pixelformat	format( pixelformat );

	window->on_pixelformat_select( format );
#ifndef _glskmm_no_sigc_
	window->data->signal_pixelformat_select.emit( format );
#endif
}

void
glsk::window::internal::on_mouse_event( glsk_window_t* object, glsk_mouse_event_t* c_mouse_event )
{
	glsk::window*		window = reinterpret_cast< glsk::window* >( glsk_window_get_userdata( object ) );
	glsk::mouse_event	mouse_event( c_mouse_event );

	window->on_mouse_event( mouse_event );

#ifndef __glskmm_no_sigc__
	window->data->signal_mouse_event.emit( mouse_event );
#endif
}

void
glsk::window::internal::on_idle( void* pointer )
{
	glsk::window*		window = reinterpret_cast< glsk::window* >( pointer );

	window->on_idle();
#ifndef _glskmm_no_sigc_
	window->data->signal_idle.emit();
#endif
}


/** Create a new rendering context.
	\note The physical device is only created when the first drawable using this is opened, and freed only when the last drawable using this is destroyed.
*/
glsk::context::context()
: data( new context::internal )
{
	data->object = glsk_context_new();
	data->refcount = 1;
}

/** Copy a rendering context reference object.
*/
glsk::context::context( const glsk::context& other )
: data( other.data )
{
	++data->refcount;
}

/** Assign a rendering context reference object.
*/
glsk::context&
glsk::context::operator =( const glsk::context& other )
{
	if ( ( --data->refcount ) == 0 )
	{
		glsk_context_free( data->object );
		delete data;
	}

    data = other.data;
	++data->refcount;

	return *this;
}

glsk::context::~context()
{
	if ( ( --data->refcount ) == 0 )
	{
		glsk_context_free( data->object );
		delete data;
	}
}

/*

	construction & destruction

*/

/** Create a new window reference object.
*/
glsk::window::window()
: data( new glsk::window::internal )
{
	data->object = glsk_window_new();
	data->idle_handler = 0;
	data->references = 1;
	
	glsk_window_set_userdata( data->object, this );

	// bind the message handlers
	glsk_window_set_callback_char_event( data->object, &window::internal::on_char_event );
	glsk_window_set_callback_close_event( data->object, &window::internal::on_close_event );
	glsk_window_set_callback_configure_event( data->object, &window::internal::on_configure_event );
	glsk_window_set_callback_create_event( data->object, &window::internal::on_create_event );
	glsk_window_set_callback_destroy_event( data->object, &window::internal::on_destroy_event );
	glsk_window_set_callback_redraw_event( data->object, &window::internal::on_redraw_event );
	glsk_window_set_callback_pixelformat_select( data->object, &window::internal::on_pixelformat_select );
	glsk_window_set_callback_mouse_event( data->object, &window::internal::on_mouse_event );
}

/** Copy a window reference object.
*/
glsk::window::window( const glsk::window& other )
: data( other.data )
{
	// increase the reference count
    ++data->references;
}

glsk::window::~window()
{
	if ( data->references <= 1 )
	{
		this->set_idle_handler( false );

		if ( data->object )
		{
			glsk_window_free( data->object );
			data->object = 0;
		}

		delete data;
	}
	else
	{
		// just decrease the refrence count, but leave the window intact
		--data->references;
	}
}

/** Assign the physical window referred to.
	\param other The window reference this is supposed to reference too
*/
glsk::window&
glsk::window::operator =( const glsk::window& other )
{
	if ( data->references <= 1 )
	{
		this->set_idle_handler( false );

		if ( data->object )
		{
			glsk_window_free( data->object );
			data->object = 0;
		}

		delete data;
	}
	else
	{
		// just decrease the refrence count, but leave the window intact
		--data->references;
	}

	this->data = other.data;
	this->data->references++;

	return *this;
}

/** Check whether this and another window are referring to the same physical window.
*/
bool
glsk::window::operator ==( const glsk::window& other ) const
{
	return this->data == other.data;
}

/**	Opens the physical window.
*/
bool
glsk::window::open( context& rc )
{
	return glsk_window_open( data->object, reinterpret_cast< glsk_context_t* >( rc.data->object ) ) != 0;
}

/**	Destroys the physical window.
*/
void
glsk::window::destroy()
{
	glsk_window_destroy( data->object );
}

void
glsk::window::swap_buffers()
{
	glsk_window_swap_buffers( data->object );
}

bool
glsk::window::select_rendercontext()
{
	return glsk_window_select_rendercontext( data->object ) != 0;
}

/** Show this window.
*/
bool
glsk::window::show()
{
	return glsk_window_show( data->object ) != 0;
}

/**Hhide this window.
*/
void
glsk::window::hide()
{
	glsk_window_hide( data->object );
}

/** Get the visibilty state of this window.
*/
bool
glsk::window::is_visible() const
{
	return glsk_window_is_visible( data->object ) != 0;
}

/*

	set & get functions

*/

/** Get this window's title.
*/
const char*
glsk::window::get_title() const
{
	return glsk_window_get_title( data->object );
}

/** Get this window's width.
*/
int
glsk::window::get_width() const
{
	return glsk_window_get_width( data->object );
}

/** Get this window's height.
*/
int
glsk::window::get_height() const
{
	return glsk_window_get_height( data->object );
}

/** Get this window's position.
*/
glsk::int2
glsk::window::get_position() const
{
	glsk::int2 result;

	glsk_window_get_position( data->object, &(result[ 0 ]), &(result[ 1 ]) );

	return result;
}

/** Get this window's size.
*/
glsk::int2
glsk::window::get_size() const
{
	glsk::int2 result;

	glsk_window_get_size( data->object, &result[ 0 ], &result[ 1 ] );

	return result;
}

/** Check whether this window wants decoration or not.
*/
bool
glsk::window::get_decorated() const
{
	return glsk_window_get_decorated( data->object ) != 0;
}

/** Check the fullscreen state of this window.
*/
bool
glsk::window::get_fullscreen() const
{
	return glsk_window_get_fullscreen( data->object ) != 0;
}

/** Check whether there is an idle handler bound.
	\return true if an idle handler is bound, false otherwise.
*/
bool
glsk::window::get_idle_handler() const
{
	return data->idle_handler != 0;
}

/** Set the title of this window.
	\param string the new title
*/
void
glsk::window::set_title( const std::string& string )
{
	glsk_window_set_title( data->object, string.c_str() );
}

/** Set this window's size.
*/
void
glsk::window::set_size( const int width, const int height )
{
	glsk_window_set_size( data->object, width, height );
}

/** Set this window's width.
*/
void
glsk::window::set_width( const int width )
{
	glsk_window_set_size( data->object, width, get_height() );
}

/** Set this window's height.
*/
void
glsk::window::set_height( const int height )
{
	glsk_window_set_size( data->object, get_width(), height );
}

/** Set this window's position.
	\note if decoration is turned on, the window manager will often interfere and this function will not work as expected
	\param x the x coordinate of this window, relative to the left side of the screen
	\param y the y coordinate of this window, relative to the lower side of the screen
*/
void
glsk::window::set_position( const int x, const int y )
{
	glsk_window_set_position( data->object, x, y );
}

/** Turn window decorations on or off.
	It tries it's best to convince the window manager to use, or not to use window decorations.
	On Microsoft Windows, this function always works.
	\param value true turns window decorations on, false turns them off
*/
void
glsk::window::set_decorated( const bool value )
{
	glsk_window_set_decorated( data->object, value ? 1 : 0 );
}

/** Turn fullscreen mode of this window on or off.
	\param value true to turn on fullscreen mode, false to turn it off
*/
void
glsk::window::set_fullscreen( const bool value )
{
	glsk_window_set_fullscreen( data->object, value ? 1 : 0 );
}

/** Bind this window's idle handler.
	Binds this window's idle handler to the main loop,
	so it is executed whenever the application has idle time.
	The execution of idle handlers happens in the order they were bound in.
	\param value true to bind the idle handler, false to free it
*/
void
glsk::window::set_idle_handler( const bool value )
{
	if ( value )
	{
		if ( data->idle_handler )
			return;

		glsk_main_append_idle_handler( &window::internal::on_idle, this );
	}
	else
	{
		if ( !data->idle_handler )
			return;

		glsk_main_remove_idle_handler( data->idle_handler );
	}
}

/*

	default message handlers

*/

/** Char handler.
	This is called whenever the window receives string input.
*/
void
glsk::window::on_char( std::string& value )
{
}

/** Close handler.
	This is called when the user requests this window to be closed.
*/
void
glsk::window::on_close()
{
	this->destroy();
}

/**Configure handler.
	This is called whenever this window changes size.
*/
bool
glsk::window::on_configure( int x, int y )
{
	return true;
}

/** Create handle.
	This is called after the window is created.
*/
bool
glsk::window::on_create( int x, int y )
{
	return true;
}

/**Destroy handler.
	This function is called whenever a window is destroyed.
	It's default behaviour is to invoke main::quit() to terminate the mainloop
	and destroy all other windows aswell.
	if that is not the desired behaviour, this function has to be overridden.
*/
void
glsk::window::on_destroy()
{
	glsk::main::quit();
}

/**	Redraw handler.
	This function is called whenever the system requests for this window to be redrawn.
*/
void
glsk::window::on_redraw()
{
}


/** Pixelformat handler.
	This function is called whenever the program needs to select a pixelformat for a drawable.
	To change the default behaviour, this function has to be overridden
*/
void
glsk::window::on_pixelformat_select( glsk::pixelformat& format )
{
	format.set_flags( pf::draw_to_window );
}

/** Idle time function
	This function is called whenever there is idle time for the application.
	For most graphical applications, this rerenders the scene for a smooth animation
*/
void
glsk::window::on_idle()
{
}

/** Mouse event handler.
	This function is called whenever the mouse is modified, that is
	when it is moved or when a button is pressed.
*/
void
glsk::window::on_mouse_event( const glsk::mouse_event& mouse_event )
{
}

/*

	signal access functions

*/

#ifndef _glskmm_no_sigc_

/** Char signal.
	\see on_char()
*/
sigc::signal< void, std::string& >&
glsk::window::signal_char()
{ return data->signal_char; }

/** Redraw signal.
	\see on_redraw()
*/
sigc::signal< void >&
glsk::window::signal_redraw()
{ return data->signal_redraw; }

/** Configure signal.
	\see on_configure()
*/
sigc::signal< bool, int, int >&
glsk::window::signal_configure()
{ return data->signal_configure; }

/** Create signal.
	\see on_create()
*/
sigc::signal< bool, int, int >&
glsk::window::signal_create()
{ return data->signal_create; }

/** Close signal.
	\see on_close()
*/
sigc::signal< void >&
glsk::window::signal_close()
{ return data->signal_close; }

/** Destroy signal.
	\see on_destroy()
*/
sigc::signal< void >&
glsk::window::signal_destroy()
{ return data->signal_destroy; }

/** Idle signal.
	\see on_idle()
*/
sigc::signal< void >&
glsk::window::signal_idle()
{ return data->signal_idle; }

/** Pixelformat select signal.
	\see on_pixelformat_select()
*/
sigc::signal< void,glsk::pixelformat& >&
glsk::window::signal_pixelformat_select()
{ return data->signal_pixelformat_select; }

/** Mouse event signal.
	\see on_mouse_event()
*/
sigc::signal< void, glsk::mouse_event& >&
glsk::window::signal_mouse_event()
{ return data->signal_mouse_event; }

#endif

/*

	misc window functions

*/

/**	Displays an error box that is modal to this window.
*/
void
glsk::window::error_box( const std::string& text ) const
{
	glsk_error_box( this->data->object, text.c_str() );
}

/*

	PBuffer implementation

*/

class glsk::pbuffer::internal
{
	public:
		glsk_pbuffer_t*		object;
		int					refcount;
	
		internal() : object( 0 ), refcount( 0 ) {}
};

/** Create a new pbuffer reference object.
*/
glsk::pbuffer::pbuffer()
: data( new internal )
{
	data->object = glsk_pbuffer_new();
	data->refcount++;
}

/** Copy a pbuffer reference.
*/
glsk::pbuffer::pbuffer( const pbuffer& other )
: data( other.data )
{
	data->refcount++;
}

glsk::pbuffer::~pbuffer()
{
	if ( --data->refcount <= 0 )
	{
		glsk_pbuffer_free( data->object );
		delete data;
	}
}

/** Assign a pbuffer reference.
*/
glsk::pbuffer& glsk::pbuffer::operator=( const pbuffer& other )
{
	if ( --data->refcount <= 0 )
	{
		glsk_pbuffer_free( data->object );
		delete data;
	}
	
	data = other.data;
	data->refcount++;

	return *this;
}

/** Compare two pbuffer references.
	\return true if both objects reference the same pbuffer, false otherwise.
*/
bool glsk::pbuffer::operator==( const pbuffer& other ) const
{
	return this->data == other.data;
}

/** Open the pbuffer.
	This creates the physical pbuffer.
	\param owner The window on which this pbuffer is supposed to be created.
	\param rc The rendering context to be associated with this.
	\param config The pixelformat configuration to be used for this pbuffer.
	\param width Desired width of this pbuffer.
	\param height Desired height of this pbuffer.
	\param get_largest If this is set to true, the system will allocate the largest pbuffer possible if the allocation would otherwise fail.
*/
bool glsk::pbuffer::open( window& owner, context& rc, pixelformat& config,
									int width, int height, bool get_largest )
{
	return glsk_pbuffer_open( data->object, glsk::internal::get_object( owner ), glsk::internal::get_object( rc ),
						glsk::internal::get_object( config ), width, height, get_largest ? 1 : 0 ) != 0;
}

/** Destroy the pbuffer.
	Destroy the physical pbuffer that is referenced by this object.
*/
void glsk::pbuffer::destroy()
{
	glsk_pbuffer_destroy( data->object );
}

void glsk::pbuffer::swap_buffers()
{
	glsk_pbuffer_swap_buffers( data->object );
}

bool glsk::pbuffer::select_rendercontext()
{
	return glsk_pbuffer_select_rendercontext( data->object ) != 0;
}

/*

	pixelformat implementation

*/

glsk::pixelformat::pixelformat( void* object )
: data( new internal )
{
	// the pixelformat object we get is initialized
	data->object = reinterpret_cast< glsk_pixelformat_t* >( object );
	data->owner = false;
}

/** Create a new pixelformat object.
*/
glsk::pixelformat::pixelformat()
: data( new internal )
{
	data->object = glsk_pixelformat_new();
	data->owner = true;
}

glsk::pixelformat::~pixelformat()
{
	if ( data->owner )
		glsk_pixelformat_free( data->object );

	delete data;
}

/** Assign this pixelformat object.
*/
glsk::pixelformat& glsk::pixelformat::operator =( const glsk::pixelformat& other )
{
	this->set_flags( other.get_flags() );
	this->set_colorbits( other.get_colorbits() );
	this->set_depthbits( other.get_depthbits() );
	this->set_stencilbits( other.get_stencilbits() );
	this->set_samples( other.get_samples() );

	return *this;
}

/** Copy this pixelformat object.
*/
glsk::pixelformat::pixelformat( const glsk::pixelformat& other )
: data( new internal )
{
	data->object = glsk_pixelformat_new();
	data->owner = true;

	*this = other;
}


/** Sets the pixelformat flags.
	\param flags can be any of the values from glsk::pixelformat::flags combined via bitwise or.
*/
void glsk::pixelformat::set_flags( const int flags )
{ glsk_pixelformat_set_flags( data->object, flags ); }

/** Sets the colorbuffer size of the pixelformat.
	\note For this to take effect pf::color_bits_set has to be set with set_flags().
	\param colorbits The desired bitsize for the color component of a pixel.
*/
void glsk::pixelformat::set_colorbits( int colorbits )
{ glsk_pixelformat_set_colorbits( data->object, colorbits ); }

/** Sets the depth of the pixelformat.
	\note For this to take effect pf::depth_bits_set has to be set with set_flags().
	\param depthbits The desired depth in bits
*/
void glsk::pixelformat::set_depthbits( int depthbits )
{ glsk_pixelformat_set_depthbits( data->object, depthbits ); }

/** Sets the stencilbuffer size of the pixelformat.
	\note For this to take effect pf::stencil_bits_set has to be set with set_flags().
	\param stencilbits The number of bits to reserve for stencil data	
*/
void glsk::pixelformat::set_stencilbits( int stencilbits )
{ glsk_pixelformat_set_stencilbits( data->object,  stencilbits ); }

/** Sets the number of samples.
	Samples are used for a popular form of anti-aliasing called multi-sampling.
	The more samples used, the less jagged the geometry will be, but it will also be much
	more time consumingto draw. if this number is set to 0, the system will choose a default
	value.
	\note For this to take effect pf::multisample has to be set with set_flags().
	\param samples The number of samples
*/
void glsk::pixelformat::set_samples( int samples )
{ glsk_pixelformat_set_samples( data->object, samples ); }

/** Get the pixelformat flags.
	\return values from pixelformat::flags combined via bitwise or
*/
int glsk::pixelformat::get_flags() const
{ return glsk_pixelformat_get_flags( data->object ); }

/** Get the color bits value.
*/
int glsk::pixelformat::get_colorbits() const
{ return glsk_pixelformat_get_colorbits( data->object ); }

/** Get the depth bits value.
*/
int glsk::pixelformat::get_depthbits() const
{ return glsk_pixelformat_get_depthbits( data->object ); }

/** Get the stencil bits value.
*/
int glsk::pixelformat::get_stencilbits() const
{ return glsk_pixelformat_get_stencilbits( data->object ); }

/** Get the samples.
*/
int glsk::pixelformat::get_samples() const
{ return glsk_pixelformat_get_samples( data->object ); }

/*

	mouse_event implementation

*/

glsk::mouse_event::mouse_event( void* object )
: object( object ) {}

/** Retrieves the current position of the mouse.
	\note Coordinates are relative to the lower left corner of the window which received
	this event. these values are always positive.
	\return The mouse coordinates.
*/
glsk::int2 glsk::mouse_event::get_position() const
{
	int2 result;
	glsk_mouse_event_get_position( reinterpret_cast< glsk_mouse_event_t* >( object ), &result[ 0 ], &result[ 1 ] );
	return result;
}

/** Retrieves the type of this event.
	\return The mouse_event::type of this event.
*/
glsk::mouse_event::type glsk::mouse_event::get_type() const
{
	switch( glsk_mouse_event_get_type( reinterpret_cast< glsk_mouse_event_t* >( object ) ) )
	{
	case GLSK_ME_TYPE_BUTTONPRESSED:
		return button_pressed;
	
	case GLSK_ME_TYPE_BUTTONRELEASED:
		return button_released;
	
	default:
	case GLSK_ME_TYPE_MOVE:
		return move;
	};
}

/** Retrieves the button referred to in this event.
	If the type of this event is button_* event, this function
	returns the number of the mousebutton starting with 1
	\return The mousebutton's number.
*/
int glsk::mouse_event::get_button() const
{ return glsk_mouse_event_get_button( reinterpret_cast< glsk_mouse_event_t* >( object ) ); }

/*
	axial device
*/

class glsk::axial_device::internal
{
	public:
	glsk_axial_device_t* 	object;
	int						refcount;
	
	inline internal( void* object )
	: object( reinterpret_cast< glsk_axial_device_t* >( object ) ), refcount( 0 ) {}
		

};

glsk::axial_device::axial_device( void* object )
: data( new internal( object ) )
{
	++data->refcount;
}

/** Create a NULL reference.
	Creates an axial_device that is not referencing a physical device.
*/
glsk::axial_device::axial_device()
: data( 0 )
{
}

/** Copy an exisiting device reference.
	\param other The object to copy from
*/
glsk::axial_device::axial_device( const axial_device& other )
: data( other.data )
{
	if ( data ) ++data->refcount;
}

glsk::axial_device::~axial_device()
{
	if ( data )
	{
		--data->refcount;
		if ( data->refcount == 0 )
			delete data;
	}
}

/** Assigns axial_device objects to each other.
	\param other The right-handed operand.
*/
glsk::axial_device&
glsk::axial_device::operator=( const axial_device& other )
{
	if ( data )
	{
		--data->refcount;
		
		if ( data->refcount == 0 )
			delete data;
	}
	
	data = other.data;
	
	if ( data ) ++data->refcount;

	return *this;
}

/** Checks whether two axial_device objects reference the same physical device.
	\return true if they reference the same device, false otherwise
*/
bool
glsk::axial_device::operator==( const axial_device& other ) const
{
	return this->data == other.data;
}

/** Retrieve the number of available buttons on this device.
	\return The number of buttons
*/
int
glsk::axial_device::get_button_count() const
{
	return ( data ) ? glsk_axial_device_get_button_count( data->object ) : 0;
}

/** Retrieve the state of a button on the device.
	\return Returns true when the device exists and the button is down, false otherwise.
*/
bool
glsk::axial_device::get_button_state( const int index ) const
{
	return ( data ) ? (glsk_axial_device_get_button_state( data->object, index )!=0) : false;
}

/** Retrieve the number of available axis pairs (vectors) in this device.
	\return The number of available vectors
*/
int
glsk::axial_device::get_vector_count() const
{
	return ( data ) ? glsk_axial_device_get_vector_count( data->object ) : 0;
}

/** Retrieve the state of two axis (a vector) in the device.
	\param index The index of the vector to retrieve.
	\return The vector as a pair of ints.
*/
glsk::int2
glsk::axial_device::get_vector_state( const int index ) const
{
	int2 result( 0, 0 );
	
	if ( data )
	{
		glsk_axial_device_get_vector_state( data->object, index,
				&(result[ 0 ]), &(result[ 1 ]) );
	}
	
	return result;
}

/** Checks whether there is updated information waiting on this device.
	\return true if this object is not a valid device or if there is new information waiting, false otherwise.
*/
bool
glsk::axial_device::is_read() const
{
	return ( data ) ? (glsk_axial_device_is_read( data->object )!=0) : true;
}

/** Marks this device as read.
	If the device is marked read axial_device::is_read() returns false until the device is updated.
	If this object is not a valid device, the function does nothing.
*/
void
glsk::axial_device::mark_read()
{
	if ( data )
		glsk_axial_device_mark_read( data->object );
}

/** Aquire the device.
*/
bool
glsk::axial_device::aquire()
{
	return data ? (glsk_axial_device_aquire( data->object )!=0) : false;
}

/** Release the device.
*/
void
glsk::axial_device::unaquire()
{
	if ( data ) glsk_axial_device_unaquire( data->object );
}

/** Checks whether this device is aquired.
*/
bool
glsk::axial_device::is_aquired() const
{
	return data && glsk_axial_device_is_aquired( data->object );
}

/*

	main loop

*/

/** Starts the main program loop.
	Starts running the main program loop until all windows are destroyed or glsk::main::quit() is called.
*/
int
glsk::main::run()
{
	return glsk_main_run();
}

/** Stops the current main loop.
	This tells the main loop to terminate as soon as possible by calling window::destroy() on all open windows.
*/
void
glsk::main::quit()
{
	glsk_main_quit();
}


/*

	misc functions

*/

/**	Sets the monitor resolution.
	This function is used to set the resolution of the users monitor.
	The old resolution is restored when the program terminates.
	\param width The width of the new resolution in pixels.
	\param height The height of the new resolution in pixels.
	\param bpp Sets the bits per pixel for the new resolution.
*/
void
glsk::set_resolution( const int width, const int height, const int bpp )
{
	glsk_set_resolution( width, height, bpp );
}

/** Displays an error message.
	\param text The error message to show.
*/
void
glsk::error_box( const std::string& text )
{
	glsk_error_box( 0, text.c_str() );
}

/** Print a string to the system console.
	\param text The string to print.
	\note This prints to the debug console in windows.
*/
void
glsk::print( const std::string& text )
{
	glsk_print( text.c_str() );
}


/** Retrieves the system's mouse.
	\return An axial_device object referencing the mouse.
*/
glsk::axial_device
glsk::get_mouse()
{
	static axial_device mouse = glsk::internal::create( glsk_get_mouse() );
	
	return mouse;
}

/** Retrieves the number of supported extensions.
	\return The number of supported extensions.
*/
int
glsk::extensions::get_count()
{
	return glsk_extensions_get_count();
}

/** Retrieves an extension name.
	This function is usefull for enumerating supported extensions
	\param index Specifies the index of the supported extension in a lexically sorted list.
	\return The extension's name.
*/
std::string
glsk::extensions::get( int index )
{
	return glsk_extensions_get( index );
}

/** Checks whether the given extension is supported by the system.
	\return true indicates that the given extension is supported, otherwise false is returned.
*/
bool
glsk::extensions::is_supported( const std::string& extension )
{
	return glsk_extensions_is_supported( extension.c_str() ) != 0;
}

/*

	keyboard access functions

*/

/** Aquire the keyboard.
*/
bool
glsk::keyboard::aquire()
{
	return glsk_keyboard_aquire() != 0;
}

/** Release the keyboard.
*/
void
glsk::keyboard::unaquire()
{
	glsk_keyboard_unaquire();
}

/** Check whether the keyboard is aquired.
	\return true if the keyboard is aquired, false otherwise
*/
bool
glsk::keyboard::is_aquired()
{
	return glsk_keyboard_is_aquired() != 0;
}

/** Get the keycode by the name of a key.
	The algorithm employed needs a single string match to find the right keycode,
	however it is good practice to only get the keycodes once and not everytime when
	checking for a keystate.
	\param name The name of the key as a human readable string.
	\return Corresponding system dependant keycode.
*/
glsk::keyboard::keycode
glsk::keyboard::get_keycode( const std::string& name )
{
	return static_cast< keycode >( glsk_keyboard_get_keycode( name.c_str() ) );
}

/** Get the name of a key by a keycode.
*/
std::string
glsk::keyboard::get_keyname( const glsk::keyboard::keycode code )
{
	return glsk_keyboard_get_keyname( static_cast< int >( code ) );
}

/** Check the keystate of a given key.
	\param code The keycode refering to the key. Use glsk::keyboard::get_keycode() to retrieve that keycode.
	\return The state of the key hit, held or up.
*/

glsk::keyboard::keystate
glsk::keyboard::get_keystate( const keycode code )
{
	return static_cast< keystate >( glsk_keyboard_get_keystate( static_cast< int >( code ) ) );
}

/*

	entry point encapsulation

*/

extern "C" int glsk_user_main( int argc, char** argv )
{
	return glsk::user_main( argc, argv );
}

/** \mainpage glskmm - an openGL Systems toolKit for C++
*
* \section intro_sec About
*
* coming soon..
*/
