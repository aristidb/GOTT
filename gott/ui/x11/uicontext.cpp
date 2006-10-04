// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is a user interface abstraction library.
 *
 * The Initial Developer of the Original Code is
 * Andreas Pokorny (andreas.pokorny@gmail.com)
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Andreas Pokorny (andreas.pokorny@gmail.com)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <boost/bind.hpp>
#include <gott/ui/x11/uicontext.hpp>
#include <gott/ui/x11/input.hpp>
#include <gott/graphics/geometry.hpp>
#include <gott/ui/x11/window.hpp>
#include <gott/events/fd_manager.hpp>


namespace gott{namespace ui{namespace x11{

uicontext::uicontext( events::main_loop & loop,  const char * connection ) 
  : loop_(loop), display_( XOpenDisplay(connection) ) 
{
  if(display_ == 0) 
    throw system_error("Could not open the x11 connection");

  screen_ = DefaultScreen( display_ );

  protocols_atom_ = XInternAtom( display_, "WM_PROTOCOLS", false );
  if( protocols_atom_ == None )
    throw system_error("Could not create atoms");

  try { 
    loop.feature<events::fd_manager>().add_fd( 
        ConnectionNumber( display_)
        , events::fd_manager::read
        , boost::bind( &uicontext::process_read, this ) 
        );
  }
  catch(std::exception &e ) {
    XCloseDisplay(display_);
    throw e;
  }catch(...) {
    XCloseDisplay(display_);
    throw system_error("Unknown exception caught");
  }

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
        win->on_mouse()( ev );
        break;
      }
    case ButtonPress:
      {
        mouse_.set_button( e.xbutton.button, true );
        mouse_event ev( mouse_event::Press, e.xbutton.button, mouse_.get_primary_position(), mouse_.get_secondary_position());
        win->on_mouse()( ev );
        break;
      }
    case ButtonRelease:
      {
        mouse_.set_button( e.xbutton.button, false );
        mouse_event ev( mouse_event::Release, e.xbutton.button, mouse_.get_primary_position(), mouse_.get_secondary_position() );
        win->on_mouse()( ev );
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
          win->on_key()( key_event( c, key_event::Press ) );
        break;
      }

    case KeyRelease:
      {
        XKeyEvent sym = e.xkey;
        key_code c = detail::key_table::get_instance().translate_key( XLookupKeysym( &sym, 0) );

        keys_.unset(c);
        if ( win && win->flags().get()[window_flags::KeyEvents] )
          win->on_key()( key_event( c, key_event::Press ) );
        break;
      }
    case ConfigureNotify:
      {
        rect old( win->last_region )
          , current( e.xconfigure.x
              , e.xconfigure.y
              , e.xconfigure.width 
              , e.xconfigure.height );

        bool move = old.left != current.left && old.top != current.top;
        bool resize = old.width != current.width && old.height != current.height;
        win->handle_sys_resize( current );

        if( move || resize )
          win->on_configure()( current );
        if( move )
          win->on_move()( current );
        if( resize )
          win->on_resize()( current );

        if( win->needs_update() ) {
          rect inv(0,0,0,0);
          std::swap( inv, win->invalid_area );
          //win->on_draw()( win->screen_buffer(), inv );
        }
        break;
      }
    case Expose:
      {
        // This is already in window coordinates
        rect region( e.xexpose.x, e.xexpose.y, e.xexpose.width,  e.xexpose.height );
        win->update_region( region );
        XSync( display_, 0);
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
  {
    window * win=windows_[i];
    if( win->needs_update() ) {
      rect inv(0,0,0,0);
      std::swap( inv, win->invalid_area );
      //win->on_draw().emit( win->screen_buffer(), inv );
      win->update_region( inv );
      XSync( display_, 0 );
    }
  }

}

void uicontext::process_exception(){
}

uicontext::~uicontext() {
  XCloseDisplay(display_);
}

}}}

