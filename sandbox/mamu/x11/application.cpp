#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <X11/extensions/xf86vmode.h>

#include "application.hpp"
#include "window.hpp"
#include "input.hpp"

using namespace std;


namespace gott{ namespace gui{ namespace x11{

application::application( char const* connection )
  : display( XOpenDisplay(connection) ), focus_window(0)
{
  std::cout << "TEST" << std::endl;
  if( display == 0 )
    throw std::runtime_error("Could not open default x11 connection");

  screen = DefaultScreen( display );

  protocols_atom = XInternAtom( display, "WM_PROTOCOLS", false );
  if( protocols_atom == None )
    throw std::runtime_error("Could not create atoms");


  int major, minor;
  if( false == glXQueryVersion( display, &major, &minor ) )
    throw std::runtime_error("Could not query glXVersion");
	
	old_glx = ( major <= 1 ) && ( minor < 3 );

  init_cursor();

  try
  {
    std::cout << "TEST" << std::endl;
    pixelformat format;
    rect region(0,0,1,1);
    window win( *this, region, "glx-test-window", format, std::size_t(window_flags::Defaults) );
    std::cout << "TEST " <<  win.is_open() << std::endl;
    
    XSync( display, 0 );
    std::size_t num_events;
    int break_counter = 10;
    while( (num_events = XPending( display )) != 0 && --break_counter )
      XSync( display, 0 );

    for( std::size_t i = 0; i < num_events; ++i )
      handle_pending_messages();
    std::cout << "set_render_context:" << std::endl;
    win.set_render_context();
    std::cout << "init_extensions:" << std::endl;
    init_extensions();

    copy(extensions.begin(), extensions.end(), ostream_iterator<std::string>(cout) ); 

    std::cout << "closeing:" << std::endl;
    win.close();
    std::cout << "Synching again ... window has been closed!"  << std::endl;
    XSync( display, 0 );
    num_events = XPending( display );
    std::cout << "Handling " <<  num_events << " Events." << std::endl;
    for( std::size_t i = 0; i < num_events; ++i )
      handle_pending_messages();

  }
  catch( std::runtime_error & e)
  {
    std::cout << e.what() << std::endl;
  }

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
    std::cout << "find_window " << (*it)->get_handle()<< " == " <<  handle << std::endl;
    if( (*it)->get_handle() == handle )
      return *it;
  }
  return 0;
}

int application::get_screen() const
{ 
  return screen;
}

void application::init_extensions() 
{
  if( glXQueryExtensionsString( display, screen ) != 0 )
  {
    istringstream in(glXQueryExtensionsString(display,screen) );
    copy( istream_iterator<string>(in), istream_iterator<string>(), 
        insert_iterator<set<string> >( extensions, extensions.begin() ) ); 
  }
  else 
    throw runtime_error("glx bad");

  {

    if( glGetString( GL_EXTENSIONS ) == 0 )
    {
      throw runtime_error( reinterpret_cast<const char* >(gluErrorString ( glGetError())));
    }
    istringstream in( reinterpret_cast<const char*>(glGetString( GL_EXTENSIONS ) ) );
    copy( istream_iterator<string>(in), istream_iterator<string>(), 
        insert_iterator<set<string> >( extensions, extensions.begin() ) ); 
  }
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

bool application::use_fallback()const
{
  return old_glx;
}

Atom application::get_atom( char const * atom_name )
{
  return XInternAtom( display, atom_name, 0 );
}

bool application::is_extension_supported( std::string const& str ) const
{
  return extensions.find(str) != extensions.end();
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
  std::cout << "Event on: " << win->get_handle() << " ";
  if( focus_window )
    std::cout << "  Focus on: " << win->get_handle() << " ";
  else 
    std::cout << "  No Focus ";
  switch( event.type )
  {
    case ReparentNotify:
      {
        std::cout << "ReparentNotify" << std::endl;
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
        std::cout << "MotionNotify" << std::endl;
        /*if ( _glsk_input_update_pointer_move( event.xmotion.x, event.xmotion.y, win) == 0 )
          _glsk_send_mouse_event( window, event.xmotion.x, window->height-event.xmotion.y,
          GLSK_ME_TYPE_MOVE, 0 );*/
        break;
      };

    case ButtonPress:
      {
        std::cout << "ButtonPress" << std::endl;
        //XSetInputFocus( display, win->get_handle(), RevertToPointerRoot, CurrentTime );

        /*if ( _glsk_input_update_pointer_button( event.xbutton.button, 1 ) == 0 )
          _glsk_send_mouse_event( win, event.xbutton.x, win->height-event.xbutton.y,
          GLSK_ME_TYPE_BUTTONPRESSED, event.xbutton.button );*/

        break;
      };

    case ButtonRelease:
      {
        std::cout << "ButtonRelease" << std::endl;
        /*if ( _glsk_input_update_pointer_button( event.xbutton.button, 0 ) == 0 )
          _glsk_send_mouse_event( win, event.xbutton.x, win->height-event.xbutton.y,
          GLSK_ME_TYPE_BUTTONRELEASED, event.xbutton.button );
          */
        break;
      };

    case MapNotify:
      {
        std::cout << "MapNotify" << std::endl;
        //XSetInputFocus( display, win->get_handle(), RevertToPointerRoot, CurrentTime );
        win->exec_on_redraw();
        break;
      };

    case DestroyNotify:
      {
        std::cout << "DestroyNotify" << std::endl;
        // FIXME 
        break;
      };

    case KeyPress:
      {
        std::cout << "KeyPress" << std::endl;
        XKeyEvent sym = event.xkey;
        key_code c = key_table::get_instance().translate_key( XLookupKeysym( &sym, 0) );

        key_info.set(c);
        if ( win && win->flags & window_flags::KeyEvents )
          win->exec_on_key( key_event( c, key_event::Press ) );
        break;
      }

    case KeyRelease:
      {
        std::cout << "KeyRelease" << std::endl;
        XKeyEvent sym = event.xkey;
        key_code c = key_table::get_instance().translate_key( XLookupKeysym( &sym, 0) );

        key_info.unset(c);
        if ( win && win->flags & window_flags::KeyEvents )
          win->exec_on_key( key_event( c, key_event::Press ) );
        break;
      }
    case ConfigureNotify:
      {
        std::cout << "ConfigureNotify" << std::endl;
        XWindowAttributes root_attribs;
        ::Window root_window = RootWindow( display, screen );

        if(!win)
          return;

        XGetWindowAttributes( display, root_window, &root_attribs );

        rect new_rect( event.xconfigure.x
            , root_attribs.height - event.xconfigure.height - event.xconfigure.y
            , event.xconfigure.width 
            , event.xconfigure.height );

        win->exec_on_configure( new_rect );
        break;

      }

    case Expose:
      {

        std::cout << "Expose" << std::endl;
        if( win== 0 )
          return;

        win->exec_on_redraw();
        std::cout << "Expose done " << std::endl;
        break;
      }

    case ClientMessage:
      {
        std::cout << "ClientMessage" << std::endl;
        if( event.xclient.message_type == protocols_atom )
        {
          std::cout << "Protocols " << std::endl;
          if( event.xclient.data.l[0] == win->protocols[window::Ping] )
          {
            std::cout << "Ping " << std::endl;
            event.xclient.window = RootWindow(display, screen);
            XSendEvent(display, event.xclient.window, false
                , SubstructureNotifyMask|SubstructureRedirectMask, &event );
          }
          else if( event.xclient.data.l[0] == win->protocols[window::DeleteWindow] )
          {

            std::cout << "Close " << std::endl;
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
        std::cout << "UnmapNotify" << std::endl;
        break;
      };

    case FocusIn:
      {
        std::cout << "FocusIn" << std::endl;
        if( (focus_window != 0) && (focus_window != win) )
          std::cout << "(gott-gui) Warning: another window is still focused.\n";

        focus_window = win;

        //_glsk_input_update_pointer_owner();
        break;

      };

    case FocusOut:
      {
        std::cout << "FocusOut" << std::endl;
        if( focus_window && (focus_window != win) )
          std::cout << "(gott-gui) Warning: losing focus without having focus.\n";

        focus_window = 0;

        //        _glsk_input_update_pointer_owner();
        break;

      };

    default:
      // unrecognized event -> terminate
      std::cout << "(gott-gui) unrecognized event:" << event.type <<  '\n';
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

}}}

