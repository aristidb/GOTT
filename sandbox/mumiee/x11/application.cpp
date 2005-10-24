#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>

#ifdef HAVE_XSYNC
#include <X11/extensions/sync.h>
#endif

#include "application.hpp"
#include "window.hpp"
#include "input.hpp"

// #define LOG_EVENTS
using namespace std;


namespace gott{ namespace gui{ namespace x11{

application::application( char const* connection )
  : display( XOpenDisplay(connection) ), focus_window(0), xsync(false)
{
  if( display == 0 )
    throw std::runtime_error("Could not open default x11 connection");

  screen = DefaultScreen( display );

  protocols_atom = XInternAtom( display, "WM_PROTOCOLS", false );
  if( protocols_atom == None )
    throw std::runtime_error("Could not create atoms");

#ifdef HAVE_XSYNC
  if(XSyncQueryExtension( display, &xsync_event_base, &xsync_error_base  ) ) {
    int vermin,vermaj;
    if( XSyncInitialize( display, &vermin, &vermaj ) )  {
      xsync = true;
    }
  }
#endif
//  init_cursor();
}

application::status application::handle_pending_messages()
{
  if( process_idle.num_slots() )
  {
    while( XPending( display ) > 0 )
    {
      XEvent					event;
      XNextEvent( display, &event );

      window * win = find_window( event.xany.window );

      if( win== 0 )
      {
        std::cout << "Unable to decode window " << event.xany.window << " RootWindow: " << RootWindow( display, screen ) << std::endl;
        break;
      }

      // handle this message
      process_event( win, event ); // RETURN VALUE?
    }
    XSync( display, 0 );
  }
  else 
  {
    XEvent event;
    XNextEvent( display, &event );

    window * win = find_window( event.xany.window );

    if( win )
      process_event( win, event ); 
    else 
      std::cout << "Unable to decode window " << event.xany.window << " RootWindow: " << RootWindow( display, screen ) << std::endl;
  }

  
  return windows.empty() ? End : Continue; 

}

void application::run()
{
  while( handle_pending_messages() == Continue )
  {
    handle_idle();
  }
}

void application::handle_idle()
{
  process_idle();
}

gott::gui::x11::window* application::find_window( ::Window handle )
{
  for( std::list<window*>::const_iterator it = windows.begin(),
      e= windows.end(); it!=e; ++it )
  {
    if( (*it)->get_handle() == handle )
      return *it;
  }
  return 0;
}

int application::get_screen() const
{ 
  return screen;
}

void application::register_window( window * ref )
{
  if( ! focus_window && windows.empty() )
    focus_window = ref;
  windows.push_back( ref );
}

void application::remove_window( window *ref )
{
  
  for( std::list<window*>::iterator it = windows.begin(),
      e= windows.end(); it!=e; ++it )
    if( *it == ref )
      it=windows.erase(it);
}

Display* application::get_display()
{
  return display;
}

Atom application::get_atom( char const * atom_name )
{
  return XInternAtom( display, atom_name, 0 );
}

void application::init_cursor()
{
	char 		data = 0;
	XColor 		dummy;
	Pixmap 		blank_pixmap = XCreateBitmapFromData( display,
		RootWindow( display, screen ), &data, 1, 1);
  if( blank_pixmap == None )
    throw std::runtime_error("Could not create Bitmap from data");

	blank_cursor = XCreatePixmapCursor( display, blank_pixmap,
								blank_pixmap, &dummy, &dummy, 0, 0);
	
	XFreePixmap( display, blank_pixmap );
}

void application::process_event( gott::gui::x11::window* win, XEvent& event )
{
#ifdef LOG_EVENTS
  std::cout << "Event on: " << win->get_handle() << " ";
  if( focus_window )
    std::cout << "  Focus on: " << win->get_handle() << " ";
  else 
    std::cout << "  No Focus ";
#endif
  switch( event.type )
  {
    case ReparentNotify:
      {
#ifdef LOG_EVENTS
        std::cout << "ReparentNotify" << std::endl;
#endif
        if( win->has_decoration()  == 0 )
        {
          XSetWindowAttributes  attributes;

          // apparently, we just released our decoration - turn on the redirect override_redirect
          attributes.override_redirect = true;

          XChangeWindowAttributes( display, win->get_handle(), CWOverrideRedirect, &attributes );
        }

        break;
      };


    case MotionNotify:
      {
        //        std::cout << "MotionNotify" << std::endl;
        mouse_info.set_primary_position( coord( event.xmotion.x, event.xmotion.y ) );
        mouse_event ev( coord(event.xmotion.x, event.xmotion.y), coord(0,0) );
        win->exec_on_mouse( ev );
        break;
      };

    case ButtonPress:
      {
#ifdef LOG_EVENTS
        std::cout << "ButtonPress" << std::endl;
#endif
        //XSetInputFocus( display, win->get_handle(), RevertToPointerRoot, CurrentTime );

        mouse_info.set_button( event.xbutton.button, true );
        mouse_event ev( mouse_event::Press, event.xbutton.button, mouse_info.get_primary_position(), mouse_info.get_secondary_position());
        win->exec_on_mouse( ev );
        break;
      };

    case ButtonRelease:
      {
#ifdef LOG_EVENTS
        std::cout << "ButtonRelease" << std::endl;
#endif
        mouse_info.set_button( event.xbutton.button, false );
        mouse_event ev( mouse_event::Release, event.xbutton.button, mouse_info.get_primary_position(), mouse_info.get_secondary_position() );
        win->exec_on_mouse( ev );
        break;
      };

    case MapNotify:
      {
#ifdef LOG_EVENTS
        std::cout << "MapNotify" << std::endl;
#endif
        //XSetInputFocus( display, win->get_handle(), RevertToPointerRoot, CurrentTime );
        win->exec_on_redraw();
        break;
      };

    case DestroyNotify:
      {

#ifdef LOG_EVENTS
        std::cout << "DestroyNotify" << std::endl;
#endif
        // FIXME 
        break;
      };

    case KeyPress:
      {
#ifdef LOG_EVENTS
        std::cout << "KeyPress" << std::endl;
#endif
        XKeyEvent sym = event.xkey;
        key_code c = key_table::get_instance().translate_key( XLookupKeysym( &sym, 0) );

        key_info.set(c);
        if ( win && win->flags & window_flags::KeyEvents )
          win->exec_on_key( key_event( c, key_event::Press ) );
        break;
      }

    case KeyRelease:
      {
#ifdef LOG_EVENTS
        std::cout << "KeyRelease" << std::endl;
#endif
        XKeyEvent sym = event.xkey;
        key_code c = key_table::get_instance().translate_key( XLookupKeysym( &sym, 0) );

        key_info.unset(c);
        if ( win && win->flags & window_flags::KeyEvents )
          win->exec_on_key( key_event( c, key_event::Press ) );
        break;
      }
    case ConfigureNotify:
      {
//#ifdef LOG_EVENTS
        std::cout << "ConfigureNotify" << std::endl;
//#endif
        XWindowAttributes root_attribs;
        ::Window root_window = RootWindow( display, screen );

        if(!win)
          return;
        XEvent other = event;
        while (! XCheckTypedWindowEvent( display, win->handle, ConfigureNotify, &other) );

        XGetWindowAttributes( display, root_window, &root_attribs );

        rect new_rect( other.xconfigure.x
            , root_attribs.height - other.xconfigure.height - other.xconfigure.y
            , other.xconfigure.width 
            , other.xconfigure.height );

        win->exec_on_configure( new_rect );

        break;

      }

    case Expose:
      {

//#ifdef LOG_EVENTS
        std::cout << "Expose" << std::endl;
//#endif
        if( win== 0 || event.xexpose.count > 0 )
          return;

        win->exec_on_redraw();
#ifdef HAVE_XSYNC
        if( use_xsync() && win->avail_request ) {
          XSyncSetCounter( display, win->counter, win->last_request );
          win->avail_request = false;
        }
#endif


#ifdef LOG_EVENTS
        std::cout << "Expose done " << std::endl;
#endif
        break;
      }

    case ClientMessage:
      {
#ifdef LOG_EVENTS
        std::cout << "ClientMessage" << std::endl;
#endif
        if( event.xclient.message_type == protocols_atom )
        {
#ifdef LOG_EVENTS
          std::cout << "Protocols " << std::endl;
#endif
          if( ::Atom(event.xclient.data.l[0]) == win->protocols[window::SyncRequest] ) {
            win->last_request.lo = event.xclient.data.l[2];
            win->last_request.hi = event.xclient.data.l[3];
            win->avail_request = true;
            std::cout << "last request : " << win->last_request.lo << " " << win->last_request.hi << std::endl;
          }
          else if( ::Atom(event.xclient.data.l[0]) == win->protocols[window::Ping] )
          {
#ifdef LOG_EVENTS
            std::cout << "Ping " << std::endl;
#endif
            event.xclient.window = RootWindow(display, screen);
            XSendEvent(display, event.xclient.window, false
                , SubstructureNotifyMask|SubstructureRedirectMask, &event );
          }
          else if( ::Atom(event.xclient.data.l[0]) == win->protocols[window::DeleteWindow] )
          {

#ifdef LOG_EVENTS
            std::cout << "Close " << std::endl;
#endif
            win->close();
          }
          std::cout << "unrecognized client-message type:" <<    XGetAtomName( display, event.xclient.data.l[0]) << '\n';
          // the close event

        }
        /*else if( event.xclient.message_type == delete_atom )
          {
          std::cout << "DELETE ATOM" << std::endl;
          }*/
        else
        {
          std::cout << "unrecognized client-message type:" <<    XGetAtomName( display, event.xclient.message_type ) << '\n';
        }
        break;

      };

    case UnmapNotify:
      {
#ifdef LOG_EVENTS
        std::cout << "UnmapNotify" << std::endl;
#endif
        break;
      };

    case FocusIn:
      {
#ifdef LOG_EVENTS
        std::cout << "FocusIn" << std::endl;
        if( (focus_window != 0) && (focus_window != win) )
          std::cout << "(gott-gui) Warning: another window is still focused.\n";
#endif

        focus_window = win;

        //_glsk_input_update_pointer_owner();
        break;

      };

    case FocusOut:
      {
#ifdef LOG_EVENTS
        std::cout << "FocusOut" << std::endl;
        if( focus_window && (focus_window != win) )
          std::cout << "(gott-gui) Warning: losing focus without having focus.\n";
#endif

        focus_window = 0;

        //        _glsk_input_update_pointer_owner();
        break;

      };

    default:
#ifdef HAVE_XDAMAGE
      if( win && win->use_xdamage && event.type == win->xdamage_event_base + XDamageNotify ) {
        XDamageNotifyEvent *notify= reinterpret_cast<XDamageNotifyEvent *>(&event);
        rect r(notify->area.x, notify->area.y, notify->area.width, notify->area.height);
        // call render method with rect, or fusioned rect, if invalid rects are available?
        win->update_rect( r );
        return;
      }
#endif
//#ifdef LOG_EVENTS
      // unrecognized event -> terminate
      std::cout << "(gott-gui) unrecognized event:" << event.type <<  '\n';
//#endif
      break;
  };
}

key_state const& application::get_key_state() const
{
  return key_info;
}

mouse_state const& application::get_mouse_state() const
{
  return mouse_info;
}

bool application::use_xsync() const {
  return xsync;
}

}}}

