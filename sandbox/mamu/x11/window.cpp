#include <iostream>
#include <boost/cstdint.hpp>
#include "window.hpp"
#include "application.hpp"

namespace gott{ namespace gui{ namespace x11{

application *get_default_app(){ static application instance; return &instance;}

namespace {
  // TODO define motif flags here
  struct WMHints
  {
    boost::int32_t flags, functions, decorations, input_mode, status;
    WMHints() : flags(0), functions(1), decorations(1), input_mode(1), status(0) {}
  };

}

window::~window()
{
   if( is_open() )
    close();
}

window::window( application& app, rect const& r, std::string const& title, pixelformat const& p, std::size_t flags )
  : app( &app ), handle(0), context(0), drawable(0), flags(Clear), parent(0)
{
  open(r,title,p,flags);
}

window::window( rect const& r, std::string const& title, pixelformat const& p, std::size_t flags )
  : app( get_default_app() ), handle(0), drawable(0), flags(Clear), parent(0)
{
  open(r,title,p,flags);
}

bool window::is_open() const
{
  return flags & WindowFlags::Open;
}

void window::open( application& a, rect const&r, std::string const& title, pixelformat const& p, std::size_t flags)
{
  if( is_open() )
    close();
  app = &a;

  open(r,title,p,flags);
}

void window::open(rect const&r, std::string const& t, pixelformat const& p, std::size_t fl )
{
  if( is_open() )
    close();

	::Window root_window = RootWindow( app->get_display(), app->get_screen() );

 	// init the pixelformat and get the associated visual info

	// run the user-handler

	GLXFBConfig fb_config;
	XVisualInfo* visual_info = 0;
	if( ! app->use_fallback() )
	{
		// select the appropriate fbconfig
		fb_config = get_fbconfig( p );
		
		// did we find a suitable fb_config?
		if ( fb_config == None )
      throw std::runtime_error("no suitable fb config");
		
		visual_info = glXGetVisualFromFBConfig( app->get_display(),	fb_config );
	}
	else
	{
		visual_info = get_visualinfo( p );
	}
		
	// did we find a matching visual info too?
	if ( visual_info == 0 )
		throw std::runtime_error( "unable to find requested visual." );

  {
    XSetWindowAttributes	attributes;
    unsigned int attributes_mask = 0;
    // init the colormap
    attributes.colormap = XCreateColormap( app->get_display(), root_window,
        visual_info->visual, AllocNone );

    attributes.border_pixel = 0;

    attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
      StructureNotifyMask | PointerMotionMask | FocusChangeMask;

    if( ! fl & Decoration )     {
      attributes_mask |= CWOverrideRedirect;
      attributes.override_redirect = true;
    }

    if( fl & ToolTip || fl & Splash  ) {
      attributes.save_under = true ;
      attributes_mask |= CWSaveUnder;
    }

    attributes_mask = CWBorderPixel | CWColormap | CWEventMask ;

    // create the physical window
    handle = XCreateWindow( app->get_display(), root_window
        , r.left
        , r.top 
        , r.width 
        , r.height 
        , 0 // border?
        , visual_info->depth, InputOutput, visual_info->visual, 
        attributes_mask, &attributes );
  }

	if( handle == None )
    throw std::runtime_error("no window handle");
///////////////

  set_window_type( fl );

 
  protocols[0] = app->get_atom("WM_DELETE_WINDOW");
protocols[1] = app->get_atom("WM_TAKE_FOCUS");
protocols[2] = app->get_atom("_NET_WM_PING");
protocols[3] = app->get_atom("_NET_WM_CONTEXT_HELP");
  XSetWMProtocols( app->get_display(), handle, protocols, 4);

  // set _NET_WM_PID
  long curr_pid = getpid();
  XChangeProperty(app->get_display(), handle
      , app->get_atom("_NET_WM_PID")
      , XA_CARDINAL, 32, PropModeReplace,
      reinterpret_cast<unsigned char *>(&curr_pid), 1
      );

 /* if( parent == 0 )
    XChangeProperty( app->get_display(), handle
        , app->get_atom("WM_CLIENT_LEADER")
        , XA_WINDOW, 32, PropModeReplace
        , reinterpret_cast<unsigned char *>(&X11->wm_client_leader), 1);*/
//////////////////
  
  //if( fl & Decoration )
  //  set_decoration();

  set_title( t );
	XSync( app->get_display(), 0 );

  // Wait for Map events? 
	
	// flag this window as open
	flags |= WindowFlags::Open;
	
	// fallback if we are below GLX version 1.3 (damn ATI drivers)
  if( app->use_fallback() == 0 ) // GLX >= 1.3
  {
    // create the GL context
    context = glXCreateNewContext( app->get_display(), fb_config, GLX_RGBA_TYPE, 0, true);

    if( context == None )
      throw std::runtime_error("glXCreateNewContext did not yield a context");

    drawable = glXCreateWindow( app->get_display(), fb_config, handle, 0 );

    if( drawable == None )
      throw std::runtime_error("no drawable" );
  }
  else // GLX <= 1.2
  {
    std::cout << "Using glX Fallback mode for glX-1.2" << std::endl;

    context = glXCreateContext( app->get_display(), visual_info, 0, true );

    if ( context == None )
      throw std::runtime_error("glXCreateContext did not yield a context");
  }
	
  // Get the current attributes .. lets hope the window manager already reset the window sizes:
  XWindowAttributes attr;
  XGetWindowAttributes( app->get_display(), handle, & attr );

  window_rect.left = attr.x;
  window_rect.top = attr.y;
  window_rect.width = attr.y;
  window_rect.height = attr.y;

  flags |= WindowFlags::Open;


  app->register_window( this );
  
  //XSync( app->get_display(), 1 );

  if( fl & Visible )
    show();

  exec_on_configure( window_rect );
}

void window::set_window_type( std::size_t fl )
{

  fl &= (Menu|Normal|Toolbar|Utility|Splash|Dock|ToolTip|Decoration);
  
  WMHints hints; 
  if( !( fl & Decoration ) )
    hints.decorations = 0;

  Atom wm_hints = app->get_atom("_MOTIF_WM_HINTS");
  XChangeProperty(app->get_display(), handle, wm_hints, wm_hints, 32, PropModeReplace, 
      reinterpret_cast<unsigned char*>(&hints), 5);

  std::vector<Atom> net_wintypes;
  if( fl & Normal )
    net_wintypes.push_back( app->get_atom("_NET_WM_WINDOW_TYPE_NORMAL") );
  if( fl & Menu ) 
    net_wintypes.push_back( app->get_atom("_NET_WM_WINDOW_TYPE_MENU") );
  if( fl & Toolbar ) 
    net_wintypes.push_back( app->get_atom("_NET_WM_WINDOW_TYPE_TOOLBAR") );
  if( fl & Utility ) 
    net_wintypes.push_back( app->get_atom("_NET_WM_WINDOW_TYPE_UTILITY") );
  if( fl & Dialog ) 
    net_wintypes.push_back( app->get_atom("_NET_WM_WINDOW_TYPE_DIALOG") );
  if( fl & Splash ) 
    net_wintypes.push_back( app->get_atom("_NET_WM_WINDOW_TYPE_SPLASH") );
  if( fl & Dock ) 
    net_wintypes.push_back( app->get_atom("_NET_WM_WINDOW_TYPE_DOCK") );


  if( net_wintypes.size() )
    XChangeProperty(app->get_display(), handle
        , app->get_atom( "_NET_WM_WINDOW_TYPE" ), XA_ATOM, 32
        , PropModeReplace, reinterpret_cast<unsigned char *>(&(net_wintypes[0])), net_wintypes.size());
  else 
    XDeleteProperty( app->get_display(), handle
        , app->get_atom( "_NET_WM_WINDOW_TYPE"));

  if( fl & ( Toolbar || ToolTip || Dialog  ) &&  parent )
    XSetTransientForHint( app->get_display(), handle, parent->handle );

  flags |= fl;
}

void window::show()
{
  if( is_open() )
  XMapWindow( app->get_display(), handle );
}

void window::hide()
{
  if( is_open() )
  XUnmapWindow( app->get_display(), handle );
}

XVisualInfo* window::get_visualinfo( pixelformat const& format ) const
{
  std::vector<boost::int32_t> values(24);
  values.push_back( GLX_RGBA );
	
	if( format.flags & pixelformat::DoubleBuffer )
		values.push_back( GLX_DOUBLEBUFFER );
		
	if( format.color.first )
	{
    switch( format.color.second )
    {
      case 32:
      case 24:
        values.push_back(GLX_RED_SIZE);
        values.push_back( 8 );
        values.push_back(GLX_GREEN_SIZE);
        values.push_back( 8 );
        values.push_back(GLX_BLUE_SIZE);
        values.push_back( 8 );
        break;

      case 16:
        values.push_back(GLX_RED_SIZE);
        values.push_back( 5 );
        values.push_back(GLX_GREEN_SIZE);
        values.push_back( 6 );
        values.push_back(GLX_BLUE_SIZE);
        values.push_back( 5 );
        break;
      default:
                break;
    }
  }
  else{
    values.push_back(GLX_RED_SIZE);
    values.push_back( 1 );
    values.push_back(GLX_GREEN_SIZE);
    values.push_back( 1 );
    values.push_back(GLX_BLUE_SIZE);
    values.push_back( 1 );
  }
  
  if( format.depth.first )
	{
    values.push_back(GLX_DEPTH_SIZE);
    values.push_back(format.depth.second);
  }
  else
  {
    values.push_back(GLX_DEPTH_SIZE);
    values.push_back(1);
  }

	
  values.push_back( None );
	
	return glXChooseVisual( app->get_display(), app->get_screen(), &(values[0]) );
}

GLXFBConfig	window::get_fbconfig( pixelformat const& format ) const
{
  std::vector<boost::int32_t> values(24);
	GLXFBConfig*			Array = 0;
	GLXFBConfig				Result = 0;
	int						bits = 0;
	int						elements = 0;

	if( format.flags & pixelformat::DrawToWindow )
		bits |= GLX_WINDOW_BIT_SGIX;

	if(format.flags & pixelformat::DrawToPBuffer)
		bits |= GLX_PBUFFER_BIT_SGIX;

  values.push_back( GLX_DRAWABLE_TYPE_SGIX);
  values.push_back( bits);

	if( format.flags & pixelformat::DoubleBuffer )
  {
    values.push_back( GLX_DOUBLEBUFFER );
    values.push_back( 1 );
  }

	if( format.color.first )
	{
    switch( format.color.second )
    {
      case 32:
      case 24:
        values.push_back(GLX_RED_SIZE);
        values.push_back( 8 );
        values.push_back(GLX_GREEN_SIZE);
        values.push_back( 8 );
        values.push_back(GLX_BLUE_SIZE);
        values.push_back( 8 );
        break;

      case 16:
        values.push_back(GLX_RED_SIZE);
        values.push_back( 5 );
        values.push_back(GLX_GREEN_SIZE);
        values.push_back( 6 );
        values.push_back(GLX_BLUE_SIZE);
        values.push_back( 5 );
        break;
      default:
        break;
    }
  }

	if( format.depth.first )
	{
    values.push_back(GLX_DEPTH_SIZE);
    values.push_back(format.depth.second);
	}

	if( format.stencil.first )
	{
    values.push_back(GLX_STENCIL_SIZE);
    values.push_back(format.stencil.second);
	}

	if( format.samples.first ) 
	{
		if( app->is_extension_supported( "GL_ARB_multisample" ) )
		{
			values.push_back( GLX_SAMPLE_BUFFERS_ARB );
      values.push_back(  1 );
	
			if( format.samples.second )
      {
        values.push_back( GLX_SAMPLES_ARB );
        values.push_back( format.samples.second );
      }
		}
		else
		{
		}
	}

  values.push_back( None );


	Array = glXChooseFBConfig( app->get_display(), app->get_screen(), &values[0], &elements );

	if ( elements )
	{
		Result = Array[ 0 ];
		XFree( Array );
		
		return Result;
	}

	return None;
}

void window::set_title( std::string const& t )
{
  if( handle )
  {
    // convert the string we got to a text property
    XTextProperty property;
    char *a = const_cast<char*>(t.c_str());

    if( XmbTextListToTextProperty( app->get_display(), &a, 1, XTextStyle , &property ) != Success )
      return;

    // set the physical window name
    XSetWMName( app->get_display(), handle, &property );

    XFree( property.value );
    // _NET_WM_NAME
  }
}



void window::close()
{
  if( ! is_open() )
    return;
  if( drawable && handle )
    exec_on_close();
  if( context )
  {
    glXDestroyContext( app->get_display(), context );
    context = 0;
  }
  if( drawable )
  {
    glXDestroyWindow( app->get_display(), drawable );
    drawable = 0;
  }
  if( handle )
  {
    XDestroyWindow( app->get_display(), handle );
    handle = 0;
  }
  flags &= ~WindowFlags::Open;
  app->remove_window( this );
}


void window::on_redraw()
{
}

void window::on_focus_enter()
{
}

void window::on_focus_leave()
{
}

void window::on_close()
{
}

void window::on_configure( gott::gui::rect const& r)
{
  window_rect = r;
}

void window::on_mouse(gott::gui::mouse_event const&)
{
}

void window::on_key(gott::gui::key_event const&)
{
}

void window::swap_buffer() 
{
  if( is_open() )
    if( !app->use_fallback() )
      glXSwapBuffers( app->get_display(), drawable);
    else 
      glXSwapBuffers( app->get_display(), handle );
}

void window::set_render_context()
{
  if( !is_open() )
    return;
  if( !app->use_fallback() )
  {
    if( glXMakeContextCurrent( app->get_display(), drawable, drawable, context ) ) 
      return;
  }
  else 
  {
    if( glXMakeCurrent( app->get_display(), handle, context ) ) 
      return;
  }
  throw std::runtime_error("Error while setting context");
}

::Window window::get_handle() const
{
  return handle;
}

gott::gui::rect const& window::get_rect() const
{
  return window_rect;
}


bool window::has_decoration() const
{
  return flags&WindowFlags::Decoration;
}

}}}
