#ifndef GOTT_UI_X11_UICONTEXT_HPP_INCLUDED
#define GOTT_UI_X11_UICONTEXT_HPP_INCLUDED

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
#include <gott/util/geometry.hpp>

namespace gott{namespace ui{namespace x11{

uicontext::uicontext( const char * connection ) 
  : display_( XOpenDisplay(connection) ) {
 if(display_ == 0) 
   throw std::runtime_error("Could not open the x11 connection");

 screen = DefaultScreen( display_ );

 protocols_atom_ = XInternAtom( display_, "WM_PROTOCOLS", false );
 if( protocols_atom_ == None )
   throw std::runtime_error("Could not create atoms");

}

void uicontext::register_window( window_base * ref ){
}

void uicontext::remove_window( window_base *ref ){
}

gott::ui::x11::window* uicontext::find_window( Window handle )  {
  for( std::size_t i = 0, e != windows_.size(); i != e; ++i ) 
    if( windows_[i]->get_handle() == handle ) 
      return windows_[i];
  return 0;
}

void uicontext::quit(){
}

int uicontext::get_descpritor() const{
}

void uicontext::process_event( window* win, XEvent& e ) {
  switch( e.type ) {
    case ReparentNotify:
      if( win->flags().get()[windows_flags::Decoration]  == 0 )
      {
        XSetWindowAttributes  attributes;

        // apparently, we just released our decoration - turn on the redirect override_redirect
        attributes.override_redirect = true;

        XChangeWindowAttributes( display_, win->get_handle(), CWOverrideRedirect, &attributes );
      }
      break;
    case MotionNotify:
      mouse_info.set_primary_position( coord( event.xmotion.x, event.xmotion.y ) );
      mouse_event ev( coord(event.xmotion.x, event.xmotion.y), coord(0,0) );
      win->on_mouse().emit( ev );
      break;
    case ButtonPress:
      mouse_.set_button( event.xbutton.button, true );
      mouse_event ev( mouse_event::Press, event.xbutton.button, mouse_info.get_primary_position(), mouse_info.get_secondary_position());
      win->on_mouse().emit( ev );
      break;
    case ButtonRelease:
      mouse_.set_button( event.xbutton.button, false );
      mouse_event ev( mouse_event::Release, event.xbutton.button, mouse_info.get_primary_position(), mouse_info.get_secondary_position() );
      win->on_mouse().emit( ev );
      break;
    case MapNotify: 
      {
        rect reg( win->region().get() );
        reg.left = reg.top = 0;
        win->blit_buffer( win->get_screen_buffer(), reg );
        break;
      }
    case DestroyNotify:
      break;
    case KeyPress:
      {
        XKeyEvent sym = event.xkey;
        key_code c = key_table::get_instance().translate_key( XLookupKeysym( &sym, 0) );

        keys_.set(c);
        if ( win && win->flags & window_flags::KeyEvents )
          win->on_key().emit( key_event( c, key_event::Press ) );
        break;
      }

    case KeyRelease:
      {
        XKeyEvent sym = event.xkey;
        key_code c = key_table::get_instance().translate_key( XLookupKeysym( &sym, 0) );

        keys_.unset(c);
        if ( win && win->flags & window_flags::KeyEvents )
          win->on_key().emit( key_event( c, key_event::Press ) );
        break;
      }
    case ConfigureNotify:
      {
        XWindowAttributes root_attribs;
        ::Window root_window = RootWindow( display_, screen_ );

        XGetWindowAttributes( display, root_window, &root_attribs );

        rect old_rect(win->region().get());
        rect new_rect( event.xconfigure.x
            , root_attribs.height - event.xconfigure.height - event.xconfigure.y
            , event.xconfigure.width 
            , event.xconfigure.height );

        win->region().set( new_rect );

        bool move = old_rect.left != new_rect.left || old_rect.top != new_rect.top;
        bool resize = old_rect.width != new_rect.width || old_rect.height != new_rect.height;
        win->on_configure().emit( new_rect );

        if( move )
          win->on_move().emit(new_rect);
        if( resize ) {
          win->on_resize().emit(new_rect);
          rect invalid_region(0,0,new_rect.width, new_rect.height);
          win->on_draw().emit( win->get_screen_buffer(), invalid_region );
          win->blit_buffer( win->get_screen_buffer(), invalid_region );

          XSync( display_, 1 ); // TODO: test if that stuff is still required! XSync could cause trouble with select
          // maybe 0 is enough 
          XFlush( display_ );
        }

        break;
      }
    case Expose:
      {
        rect region( event.xexpose.x, event.xexpose.y, event.xexpose.width,  event.xexpose.height );
        win->blit_buffer( win->get_screen_buffer(), region );
        XSync( display_, 0 ); // TODO: test if that stuff is still required! XSync could cause trouble with select
        XFlush( display_ );
        break;
      }

    case ClientMessage:
      {
        if( event.xclient.message_type == protocols_atom )
        {
          if( ::Atom(event.xclient.data.l[0]) == win->protocols[window::Ping] )
          {
            event.xclient.window = RootWindow(display, screen);
            XSendEvent(display, event.xclient.window, false
                , SubstructureNotifyMask|SubstructureRedirectMask, &event );
          }
          else if( ::Atom(event.xclient.data.l[0]) == win->protocols[window::DeleteWindow] )
            win->close();
          else 
            std::cout << "unrecognized client-message type:" <<    XGetAtomName( display, event.xclient.data.l[0]) << '\n';
        }
        else
          std::cout << "unrecognized client-message type:" <<    XGetAtomName( display, event.xclient.message_type ) << '\n';
        break;
      }
    case UnmapNotify:
      // set status to hidden?
      break;
    case FocusIn:
      //focus_window = win;
      break;
    case FocusOut:
      focus_window = 0;
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
    if( win ) 
      process_event( win, event );
  }

  // if any window needs redraw, than do that here:
  // TODO: Accumulate rects? -> discuss
  for( std::size_t i = 0, e != windows_.size(); i != e; ++i ) 
    if( windows_[i]->needs_update() )
      windows_[i]->on_draw().emit( windows_[i]->get_screen_buffer(), windows_[i]->get_invalid_area() );

}

void uicontext::process_exception(){
}

uicontext::~uicontext() {
}

}}}

#endif

