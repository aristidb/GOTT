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
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <boost/bind.hpp> 
#include <boost/cstdint.hpp> 
#include <boost/lambda/lambda.hpp> 
#include <boost/lambda/bind.hpp> 
#include <boost/lambda/construct.hpp> 
#include <gott/ui/x11/window.hpp> 
#include <gott/ui/x11/agg_detail.hpp> 

namespace gott{namespace ui{namespace x11{
namespace {
  // TODO define motif flags here
  struct GOTT_LOCAL WMHints
  {
    boost::int32_t flags, functions, decorations, input_mode, status;
    WMHints() : flags(0), functions(1), decorations(1), input_mode(1), status(0) {}
  };
}

using boost::bind;
using boost::lambda::var;
using boost::lambda::constructor;
window::window( uicontext& app, rect const& position, string const& title, std::size_t flags ) 
  : region_( gott::properties::external_storage<rect>( 
        bind(&window::get_region, this) 
        , bind(&window::handle_resize, this, _1 ) )  
      )
  , title_( gott::properties::external_storage<gott::string>(
        var(title_string)
        , bind(&window::set_title, this, _1 ) )
      )
  , visibility_( gott::properties::external_storage<bool>(
        var(mapped_state)
        , bind(&window::map_window, this,_1 ) )
      )
  , flags_( gott::properties::external_storage<flags_type>(
        var(win_flags)
        , bind(&window::set_window_type, this,_1) )
      ) 
  , handle(0)
  , impl(0)
  , invalid_area(0,0,0,0)
  , mapped_state(false)
  , win_flags(0)
  , context(&app)
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
    if( flags & window_flags::MouseEvents ) 
      attributes.event_mask |= 
        PointerMotionMask 
        | FocusChangeMask 
        | ButtonPressMask 
        | ButtonReleaseMask;

    if( flags & window_flags::KeyEvents ) 
      attributes.event_mask |= 
        FocusChangeMask 
        | KeyPressMask 
        | KeyReleaseMask;


    if( ! flags & window_flags::Decoration )     {
      attributes_mask |= CWOverrideRedirect;
      attributes.override_redirect = true;
    }

    if( flags & window_flags::ToolTip || flags & window_flags::Splash  ) {
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

  // not exception safe!
  impl = new detail::agg_buffer( context->get_display(), context->get_screen(), handle, vis_info.first, vis_info.second );
  impl->resize_buffer( position );

  flags_.set( flags );


  size_t num_p = 4;
  protocols[0] = context->get_atom("WM_DELETE_WINDOW");
  protocols[1] = context->get_atom("WM_TAKE_FOCUS");
  protocols[2] = context->get_atom("_NET_WM_PING");
  protocols[3] = context->get_atom("_NET_WM_CONTEXT_HELP");

  XSetWMProtocols( context->get_display(), handle, protocols, num_p);

  // set _NET_WM_PID - to allow the wm to kill the application providing the window
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
  
  title_.set(title);

  // Wait for Map events? 
	
	// flag this window as open
	flags |= window_flags::Open;
  
  region_.set(position);

  context->register_window( this );
 
  if( flags & window_flags::Visible )
    visibility_.set(true);
  XFlush( context->get_display() );

  invalidate_area( rect(0,0,position.width, position.height ) );
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

  rect current( attr.x, attr.y, attr.width, attr.height );
  if( region == current ) {
    impl->resize_buffer( region );
    if( last_region != current ) {
      invalidate_area(rect ( 0,0, current.width, current.height ) );
      last_region = region;
    }
  }
  else if( region.left == current.left
      && region.top == current.top
      && ( region.width != current.width
        || region.height != current.height ) ) {
    XResizeWindow( context->get_display(), handle, region.width, region.height );
    std::cout << "Sending Resize" << std::endl;
  }
  else if(( region.left != current.left
        || region.top != current.top)
      && region.width == current.width
      && region.height == current.height ) {
    std::cout << "region : " << region.left << "," << region.top << " vs current : " << current.left << "," << current.top << std::endl;
    XMoveWindow( context->get_display(), handle, region.left, region.top);
    std::cout << "Sending Move" << std::endl;
  }
  else {
    XMoveResizeWindow(context->get_display(), handle, region.left, region.top, region.width, region.height );
    std::cout << "Sending Move" << std::endl;
  }
}

/**
 * Converts the string to a text property, and sets it as the 
 * X11 window manager title property. 
 */
void window::set_title( gott::string const& str ){
  title_string = str;
  // convert the string we got to a text property
  XTextProperty property;
  char *a = const_cast<char*>(reinterpret_cast<char const*>(str.as_utf8().begin ) );

  if( XmbTextListToTextProperty( context->get_display(), &a, 1, XTextStyle , &property ) != Success )
    return;

  // set the physical window name
  XSetWMName( context->get_display(), handle, &property );

  XFree( property.value );
}

void window::map_window( bool new_state ) {
  if( new_state != mapped_state ) {
    if( new_state ) {
      XMapWindow( context->get_display(), handle ); 
    }
    else 
      XUnmapWindow( context->get_display(), handle );
    mapped_state = new_state;
  }
}


/**
 * \brief update the hints for the window manager. 
 * \bug This code is pretty much untested, there is no evidence
 * whether these flags are really considered by the wm... 
 */
void window::set_window_type( gott::ui::window_base::flags_type const& new_flags ){
  flags_type fl( new_flags );
  fl &= (window_flags::Menu|window_flags::Normal|window_flags::Toolbar|window_flags::Utility|window_flags::Splash|window_flags::Dock|window_flags::ToolTip|window_flags::Decoration);
  
  WMHints hints; 
  if( !( fl[window_flags::Decoration] ) )
    hints.decorations = 0;

  Atom wm_hints = context->get_atom("_MOTIF_WM_HINTS");
  change_property(wm_hints, wm_hints, 32, reinterpret_cast<unsigned char*>(&hints), 5);

  std::vector<Atom> net_wintypes;
  if( fl [window_flags::Normal] )
    net_wintypes.push_back( context->get_atom("_NET_WM_WINDOW_TYPE_NORMAL") );
  if( fl[window_flags::Menu] ) 
    net_wintypes.push_back( context->get_atom("_NET_WM_WINDOW_TYPE_MENU") );
  if( fl[window_flags::Toolbar] ) 
    net_wintypes.push_back( context->get_atom("_NET_WM_WINDOW_TYPE_TOOLBAR") );
  if( fl[window_flags::Utility] ) 
    net_wintypes.push_back( context->get_atom("_NET_WM_WINDOW_TYPE_UTILITY") );
  if( fl[window_flags::Dialog] ) 
    net_wintypes.push_back( context->get_atom("_NET_WM_WINDOW_TYPE_DIALOG") );
  if( fl[window_flags::Splash] ) 
    net_wintypes.push_back( context->get_atom("_NET_WM_WINDOW_TYPE_SPLASH") );
  if( fl[window_flags::Dock] ) 
    net_wintypes.push_back( context->get_atom("_NET_WM_WINDOW_TYPE_DOCK") );


  if( net_wintypes.size() )
    change_property(
        context->get_atom( "_NET_WM_WINDOW_TYPE" ), XA_ATOM, 32
        , reinterpret_cast<unsigned char *>(&(net_wintypes[0])), net_wintypes.size());
  else 
    XDeleteProperty( context->get_display(), handle
        , context->get_atom( "_NET_WM_WINDOW_TYPE"));
 
#if 0
  if( fl[window_flags::Toolbar] || fl[window_flags::ToolTip] || fl[window_flags::Dialog] &&  parent )
    XSetTransientForHint( context->get_display(), handle, parent->handle );
#endif
  win_flags |= fl;

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
 
gott::ui::window_base::string_property_type const& window::title() const
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
  if( region.left == 0 
      && region.top == 0  
      && region.width == impl->buffer.width()
      && region.height == impl->buffer.height() ) {
    impl->update_window();
  }
  else {
    impl->update_rect( region );
  }
}

uicontext* window::get_uicontext(){
  return context;
}

bool window::needs_update() const{
  return invalid_area.width != 0 || invalid_area.height != 0;
}
rect window::get_invalid_area() const {
  return invalid_area;
}
void window::invalidate_area( rect const& region ){
  invalid_area.add_region(region);
}


void window::blit_buffer( coord const& destination, agg::rendering_buffer const& buffer, pixel_format::type buf_format ) {
  impl->blit_buffer(destination,buffer,buf_format);
}
void window::blit_rect( rect const& source, coord const& destination, agg::rendering_buffer const& buffer, pixel_format::type buf_format  ) {
  impl->blit_rect(source, destination, buffer, buf_format );
}

agg::rendering_buffer const& window::screen_buffer() const{
  return impl->buffer;
}

agg::rendering_buffer & window::screen_buffer(){
  return impl->buffer;
}

Window window::get_handle() const{
  return handle;
}

void window::change_property( Atom property, Atom type, int format, unsigned char *data, int nelements ) {
  XChangeProperty(context->get_display(), handle, property, type, format, PropModeReplace, data, nelements);
}

window::~window()
{
  delete impl;
}



}}}

