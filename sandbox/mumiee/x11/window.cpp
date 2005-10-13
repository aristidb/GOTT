
#include <iostream>

#pragma GCC visibility push(default)
#include <boost/cstdint.hpp>
#pragma GCC visibility pop
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
  : app( &app ), handle(0), flags(Clear), parent(0)
{
  open(r,title,p,flags);
}

window::window( rect const& r, std::string const& title, pixelformat const& p, std::size_t flags )
  : app( get_default_app() ), flags(Clear), parent(0)
{
  open(r,title,p,flags);
}

bool window::is_open() const
{
  return flags & window_flags::Open;
}

void window::open( application& a, rect const&r, std::string const& title, pixelformat const& p, std::size_t flags)
{
  if( is_open() )
    close();
  app = &a;

  open(r,title,p,flags);
}

void window::open(rect const&r, std::string const& t, pixelformat const& , std::size_t fl )
{
  if( is_open() )
    close();

	::Window root_window = RootWindow( app->get_display(), app->get_screen() );

 	// init the pixelformat and get the associated visual info

	// run the user-handler
  this->prepare_visual( app->get_screen(), app->get_display() );
		
  {
    XSetWindowAttributes	attributes;
    unsigned int attributes_mask = 0;
    // init the colormap
    attributes.colormap = XCreateColormap( app->get_display(), root_window,
        this->get_visual(), AllocNone );

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
        , this->get_depth(), InputOutput
        , this->get_visual(), 
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
	flags |= window_flags::Open;
  
  // Get the current attributes .. lets hope the window manager already reset the window sizes:
  XWindowAttributes attr;
  XGetWindowAttributes( app->get_display(), handle, & attr );

  window_rect.left = attr.x;
  window_rect.top = attr.y;
  window_rect.width = attr.y;
  window_rect.height = attr.y;

  flags |= window_flags::Open;

  this->setup_renderer(app->get_display(), app->get_screen(), handle, window_rect );

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
  if( handle )
  {
    exec_on_close();

    this->close_renderer(app->get_display(), app->get_screen(), handle ); 
    XDestroyWindow( app->get_display(), handle );
    handle = 0;
  }
  flags &= ~window_flags::Open;
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
  this->resize_renderer( app->get_display(), app->get_screen(), handle, window_rect);
}

void window::on_mouse(gott::gui::mouse_event const&)
{
}

void window::on_key(gott::gui::key_event const&)
{
}

void window::swap_buffer() 
{
  this->update_window();
}

void window::set_render_context()
{
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
  return flags&window_flags::Decoration;
}

}}}
