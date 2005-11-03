// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT x11 window implementation
// Authors: Andreas Pokorny
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <gott/ui/x11/window.hpp> 
#include <gott/ui/x11/agg_detail.hpp> 
#include <boost/bind.hpp> 

namespace gott{namespace ui{namespace x11{

using boost::bind;
using boost::_1;
window::window( uicontext& app, rect const& position, string const& title, std::size_t flags ) 
  : region_( gott::propertis::external_storage<rect>( 
        bind(&window::get_region, this) 
        , bind(&window::handle_resize, this, _1 ) )  
      )
  , title_( gott::propertis::external_storage<gott::string>(
        bind(&window::get_title, this)
        , bind(&window::set_title, this, _1 ) )
      )
  , handle(0)
  , impl(0), invalid_area(0,0,0,0), context(&app)
{

  ::Window root_window = RootWindow( context->get_display(), context->get_screen() );
  context->register_window( this );

  std::pair<Visual *, int> vis_info = detail::pick_visual( context->get_display(), context->get_screen() );
  {
    XSetWindowAttributes	attributes;
    unsigned int attributes_mask =  CWBorderPixel | CWColormap | CWEventMask ;
    // init the colormap
    attributes.colormap = XCreateColormap( context->get_display(), root_window,
        vis_info.first, AllocNone );

    attributes.border_pixel = 0;

    attributes.event_mask = ExposureMask | StructureNotifyMask; 
    if( flags & MouseEvents ) 
      attributes.event_mask = 
        PointerMotionMask 
        | FocusChangeMask 
        | ButtonPressMask 
        | ButtonReleaseMask;

    if( flags & KeyEvents ) 
      attributes.event_mask = 
        FocusChangeMask 
        | KeyPressMask 
        | KeyReleaseMask;


    if( ! flags & Decoration )     {
      attributes_mask |= CWOverrideRedirect;
      attributes.override_redirect = true;
    }

    if( flags & ToolTip || fl & Splash  ) {
      attributes.save_under = true ;
      attributes_mask |= CWSaveUnder;
    }


    // create the physical window
    handle = XCreateWindow( 
        context->get_display()
        , root_window
        , position.left
        , position.top 
        , position.width 
        , position.height 
        , 0 // border?
        , vis_info.second
        , InputOutput
        , vis_info.first
        , attributes_mask
        , &attributes );
  }

  if( handle == None )
    throw std::runtime_error("no window handle");
  ///////////////

  set_window_type( fl );


  size_t num_p = 4;
  protocols[0] = context->get_atom("WM_DELETE_WINDOW");
  protocols[1] = context->get_atom("WM_TAKE_FOCUS");
  protocols[2] = context->get_atom("_NET_WM_PING");
  protocols[3] = context->get_atom("_NET_WM_CONTEXT_HELP");

  XSetWMProtocols( context->get_display(), handle, protocols, num_p);

  // set _NET_WM_PID
  long curr_pid = getpid();
  change_property(
      context->get_atom("_NET_WM_PID")
      , XA_CARDINAL
      , 32
      , reinterpret_cast<unsigned char *>(&curr_pid)
      , 1
      );


 /* if( parent == 0 )
    change_property(
        context->get_atom("WM_CLIENT_LEADER")
        , XA_WINDOW, 32
        , reinterpret_cast<unsigned char *>(&X11->wm_client_leader), 1);*/
//////////////////
  
  //if( fl & Decoration )
  //  set_decoration();

  set_title( t );
	XSync( context->get_display(), 0 );

  // Wait for Map events? 
	
	// flag this window as open
	flags |= window_flags::Open;
  
  // Get the current attributes .. lets hope the window manager already reset the window sizes:
  XWindowAttributes attr;
  XGetWindowAttributes( context->get_display(), handle, & attr );

  window_rect.left = attr.x;
  window_rect.top = attr.y;
  window_rect.width = attr.width;
  window_rect.height = attr.height;

  flags |= window_flags::Open;

  this->setup_renderer(context->get_display(), context->get_screen(), handle, window_rect );

  context->register_window( this );
  
  if( fl & Visible )
    show();

  exec_on_configure( window_rect );

}

/**
 * \brief handles the "read" part of the region property
 * The area data of the window is read directly from xlib
 * \returns the area covered by the window 
 */
rect window::get_region() const{
  XWindowAttributes attr;
  XGetWindowAttributes( context->get_display(), handle, &attr );
  return rect(attr.x, attr.y, attr.width,attr.height );
}

/**
 * \brief forwards resize events to the system
 * There are two different resize events, which makes the 
 * whole thing problematic. If the user wants to resize the 
 * window he picks the propery and sets the new size, then 
 * xlib will send the resize event to the server/window manager. 
 * This resize request has to be processed there and returns
 * back as a ConfigureNotify. The uicontext will tell the 
 * window that the size actually changed. So we have to differ
 * between these two resize ``events''.
 * This sounds ugly, but it is simple to solve. We can check 
 * this by requesting the ``real'' current window size from xlib, 
 * and compare it with the passed one. If they do not differ, 
 * we just forward the resize to our agg_buffer which will 
 * then update its buffer, otherwise we send the resize request 
 * to xlib. 
 */
void window::handle_resize( rect const& region ){
  XWindowAttributes attr;
  XGetWindowAttributes( context->get_display(), handle, &attr );
          
  if( region.left == attr.x
      && region.top == attr.y 
      && region.width == attr.width
      && region.height == attr.height ) {
    agg_buffer->resize_buffer( region )
  }
  else 
    if( region.left == attr.x
        && region.top == attr.y 
        && ( region.width != attr.width
          || region.height != attr.height ) )
      XResizeWindow( context->get_display(), handle, region.width, region.height );
    else if(( region.left == attr.x
          || region.top == attr.y )
        && region.width == attr.width
        && region.height == attr.height )
      XMoveWindow( context->get_display(), handle, region.left, region.top);
    else
      XMoveResizeWindow(context->get_display(), handle, region.left, region.top, region.width, region.height );
}

gott::string window::get_title() const{
}

/**
 * Converts the string to a text property, and sets it as the 
 * X11 window manager title property. 
 */
void window::set_title( gott::string const& str ){
  // convert the string we got to a text property
  XTextProperty property;
  char *a = const_cast<char*>(str.as_utf8().begin());

  if( XmbTextListToTextProperty( context->get_display(), &a, 1, XTextStyle , &property ) != Success )
    return;

  // set the physical window name
  XSetWMName( context->get_display(), handle, &property );

  XFree( property.value );
}

gott::ui::window_base::rect_property_type& window::region()
{
  return region_;
}

gott::ui::window_base::rect_property_type const& window::region() const
{
  return region_;
}

gott::ui::window_base::string_property_type & window::title()
{
  return title_;
}
 
gott::ui::window_base::string_property_type const& window::title()
{
  return title_;
}


gott::ui::window_base::toggle_property_type& window::visible()
{
  return visibility_;
}
 
gott::ui::window_base::toggle_property_type const& window::visible() const
{
  return visibility_;
}
 


gott::ui::window_base::flags_property_type& window::flags()
{
  return flags_;
}
 
gott::ui::window_base::flags_property_type const& window::flags() const
{
  return flags_;
}
 


void window::set_size_hints(){
}

void window::update_region( rect const& region ){
  blit_buffer( buffer, region );
}

uicontext* window::get_uicontext(){
  return context;
}

bool window::needs_update() const{
  return invalid_area.width == 0 || invalid_area.height == 0;
}
rect window::get_invalidation_area() const{
  return invalid_area;
}
void window::invalidate_area( rect const& region ){
  invalid_area.add_region(region);
}


void window::blit_buffer( agg::rendering_buffer const& buffer, recont const& target_region ){
}

agg::rendering_buffer const& window::screen_buffer() const{
  return buffer;
}

agg::rendering_buffer & window::screen_buffer(){
  return buffer;
}

Window window::get_handle() const{
  return handle;
}

void window::change_property( Atom property, Atom type, int format, unsigned char *data, int nlements ) {
  XChangeProperty(context->get_display(), handle, property, type, format, PropModeReplace, data, nelements);
}

window::~window()
{
}



}}}

