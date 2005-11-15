// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT User interface context base class
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

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <gott/ui/x11/uicontext.hpp>
#include <gott/ui/x11/input.hpp>
#include <gott/graphics/geometry.hpp>
#include <gott/ui/x11/window.hpp>

namespace gott{namespace ui{namespace x11{

uicontext::uicontext( const char * connection ) 
  : display_( XOpenDisplay(connection) ) {
 if(display_ == 0) 
   throw std::runtime_error("Could not open the x11 connection");

 screen_ = DefaultScreen( display_ );

 protocols_atom_ = XInternAtom( display_, "WM_PROTOCOLS", false );
 if( protocols_atom_ == None )
   throw std::runtime_error("Could not create atoms");

}

Display* uicontext::get_display() {
  return display_;
}
int uicontext::get_screen() const {
  return screen_;
}
Atom uicontext::get_atom( const char* atom ) const {
  return XInternAtom( display_, atom, 0 );
}

void uicontext::register_window( window_base * ref ){
  window *xwin = dynamic_cast<window*>(ref);
  if(xwin)
    windows_.push_back( xwin );
  else {
    // do what?
  }
}

void uicontext::remove_window( window_base *ref ){
  window *xwin = dynamic_cast<window*>(ref);
  if(xwin) {
    std::vector<gott::ui::x11::window*>::iterator it = find( windows_.begin(), windows_.end(), xwin);
    if( it != windows_.end()) 
      windows_.erase( it );
  }
  else {
    // do what?
  }

}

gott::ui::x11::window* uicontext::find_window( Window handle )  {
  for( std::size_t i = 0, e = windows_.size(); i != e; ++i ) 
    if( windows_[i]->get_handle() == handle ) 
      return windows_[i];
  return 0;
}

void uicontext::quit(){
}

int uicontext::get_descriptor() const{
  return ConnectionNumber( display_ );
}

void uicontext::process_event( window* win, XEvent& e ) {
  switch( e.type ) {
    case ReparentNotify:
      if( win->flags().get()[window_flags::Decoration]  == 0 )
      {
        XSetWindowAttributes  attributes;

        // apparently, we just released our decoration - turn on the redirect override_redirect
        attributes.override_redirect = true;

        XChangeWindowAttributes( display_, win->get_handle(), CWOverrideRedirect, &attributes );
      }
      break;
    case MotionNotify:
      {
        mouse_.set_primary_position( coord( e.xmotion.x, e.xmotion.y ) );
        mouse_event ev( coord(e.xmotion.x, e.xmotion.y), coord(0,0) );
        win->on_mouse().emit( ev );
        break;
      }
    case ButtonPress:
      {
      mouse_.set_button( e.xbutton.button, true );
      mouse_event ev( mouse_event::Press, e.xbutton.button, mouse_.get_primary_position(), mouse_.get_secondary_position());
      win->on_mouse().emit( ev );
      break;
      }
    case ButtonRelease:
      {
        mouse_.set_button( e.xbutton.button, false );
        mouse_event ev( mouse_event::Release, e.xbutton.button, mouse_.get_primary_position(), mouse_.get_secondary_position() );
        win->on_mouse().emit( ev );
        break;
      }
    case MapNotify: 
      {
        rect reg( win->region().get() );
        reg.left = reg.top = 0;
        win->update_region( reg ); //< required?
        break;
      }
    case DestroyNotify:
      break;
    case KeyPress:
      {
        XKeyEvent sym = e.xkey;
        key_code c = detail::key_table::get_instance().translate_key( XLookupKeysym( &sym, 0) );

        keys_.set(c);
        if ( win && win->flags().get()[window_flags::KeyEvents] )
          win->on_key().emit( key_event( c, key_event::Press ) );
        break;
      }

    case KeyRelease:
      {
        XKeyEvent sym = e.xkey;
        key_code c = detail::key_table::get_instance().translate_key( XLookupKeysym( &sym, 0) );

        keys_.unset(c);
        if ( win && win->flags().get()[window_flags::KeyEvents] )
          win->on_key().emit( key_event( c, key_event::Press ) );
        break;
      }
    case ConfigureNotify:
      {
        /*XWindowAttributes root_attribs;
        ::Window root_window = RootWindow( display_, screen_ );
        XGetWindowAttributes( display_, root_window, &root_attribs );*/

        win->region().set(rect( 
              e.xconfigure.x,
              , e.xconfigure.y
              , e.xconfigure.width 
              , e.xconfigure.height 
              ) );


        if( win->needs_update() ) {
          win->on_resize().emit(new_rect);
          rect invalid_region(win->get_invalid_area());
          win->on_draw().emit( win->screen_buffer(), invalid_region );
          win->update_region( invalid_region );
          XSync( display_, 1 ); // TODO: test if that stuff is still required! XSync could cause trouble with select
          XFlush( display_ );
        }


        break;
      }
    case Expose:
      {
        // This is already in window coordinates
        rect region( e.xexpose.x, e.xexpose.y, e.xexpose.width,  e.xexpose.height );
        win->update_region( region );
        XSync( display_, 0 ); // TODO: test if that stuff is still required! XSync could cause trouble with select
        XFlush( display_ );
        break;
      }

    case ClientMessage:
      {
        if( e.xclient.message_type == protocols_atom_ )
        {
          if( ::Atom(e.xclient.data.l[0]) == win->protocols[window::Ping] )
          {
            e.xclient.window = RootWindow(display_, screen_);
            XSendEvent(display_, e.xclient.window, false
                , SubstructureNotifyMask|SubstructureRedirectMask, &e);
          }
          else if( ::Atom(e.xclient.data.l[0]) == win->protocols[window::DeleteWindow] ) {
            // How should we close a window?!?!
            // win->close()
          }
          else 
            std::cout << "unrecognized client-message type:" <<    XGetAtomName( display_, e.xclient.data.l[0]) << '\n';
        }
        else
          std::cout << "unrecognized client-message type:" <<    XGetAtomName( display_, e.xclient.message_type ) << '\n';
        break;
      }
    case UnmapNotify:
      // set status to hidden?
      break;
    case FocusIn:
      //focus_window = win;
      break;
    case FocusOut:
      // focus_window = 0;
      break;
    default:
      break;
  }
}

void uicontext::process_read(){
  while(XPending( display_ ) > 0 ) {
    XEvent event;
    XNextEvent(display_, &event );
    window * win  = find_window( event.xany.window );
    if( win )  {
      process_event( win, event );
    }
  }

  // if any window needs redraw, than do that here:
  // TODO: Accumulate rects? -> discuss
  for( std::size_t i = 0, e = windows_.size(); i != e; ++i ) 
    if( windows_[i]->needs_update() )
      windows_[i]->on_draw().emit( windows_[i]->screen_buffer(), windows_[i]->get_invalid_area() );

}

void uicontext::process_exception(){
}

uicontext::~uicontext() {
}

}}}

