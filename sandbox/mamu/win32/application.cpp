#include <windows.h>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <GL/wgl.h>
#include <GL/glu.h>

#include "application.hpp"
#include "window.hpp"
#include "input.hpp"

using namespace std;


namespace gott{ namespace gui{ namespace win32{

application::application( HINSTANCE hi )
  : hinstance( hi ), focus_window( 0 )
{
	cls_name = TEXT( "gott-wnd-cls" );
	EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &desktop_mode );

  register_class( hi, cls_name );

  init_input();
  init_extensions();
}

void application::init_input()
{
  // acquire dinput8 interfaces 
}

void application::init_extensions()
{

  //create window 
  // get extensions
  // wgl_functions.init();
  // end 
}

os_gl const& application::get_os_support() const
{
  return wgl_functions;
}

LRESULT CALLBACK
application::window_proc( HWND hwindow, UINT message, WPARAM wparam, LPARAM lparam )
{
  if( message == WM_NCCREATE )
  {
   // CREATESTRUCT*	Data = reinterpret_cast<CREATESTRUCT*>(lparam);
   // window*	win = reinterpret_cast<window*>(Data->lpCreateParams);

   // ?? 
    return DefwindowProc( hwindow, message, wparam, lparam );
  }
  else
  {
    window*	win = reinterpret_cast<window*>(GetwindowLongPtr( hwindow, GWLP_USERDATA ));

    switch( message )
    {
      case WM_CREATE:
        {
          CREATESTRUCT*	Data = (CREATESTRUCT*)lparam;

          win->set_pixel_format();
          win->create_context();

          win->exec_on_configure( rect( Data->x, Data->y, Data->cx, Data->cy ) );          
        }
        return 0;

      case WM_DESTROY:
        {
          if ( window->callback.destroy_event )
            window->callback.destroy_event( window, 0 );
          else
            glsk_main_quit(); // default behaviour

          if ( wglGetCurrentDC() == window->dc )
            wglMakeCurrent( window->dc, 0 );

          if ( window->context->handle != NULL )
          {
            --window->context->refcount;

            if ( window->context->refcount == 0 )
            {
              wglDeleteContext( window->context->handle );
              window->context->handle = NULL;
            }
          }

          window->context = 0;

          ReleaseDC( window->handle, window->dc );

          window->context = 0;
          window->dc = 0;
          window->handle = 0;
          window->flags &= ~GLSK_WINDOW_OPEN;
        }
        return 0;

        // antiflicker on resize
      case WM_ERASEBKGND:
        return 0;


        // Intersect screensaver and monitor power-down
      case WM_SYSCOMMAND:
        {
          if ( (wparam == SC_SCREENSAVE) || (wparam == SC_MONITORPOWER) )
            return 0;

          break;
        }

      case WM_ACTIVATE:
        {
          if ( LOWORD( wparam ) != WA_INACTIVE )
          {
            // we're activated
            _glsk_input_set_owner( window );
            _glsk_input_enable( 1 );
          }
          else
          {
            // deactivate inputs here
            _glsk_input_enable( 0 );
          }
        }
        return 0;

      case WM_ENTERMENULOOP:
      case WM_ENTERSIZEMOVE:
        {
          //on_activate( active = false );
          // deactivate inputs here
          _glsk_input_enable( 0 );
        }
        return 0;

      case WM_EXITMENULOOP:
      case WM_EXITSIZEMOVE:
        {
          /*if ( (GetActivewindow() == Message.hwindow) || (!IsIconic( Message.hwindow )) )
            OnActivate( bActive = true );
            else
            OnActivate( bActive = false );*/
        }
        return 0;

      case WM_PAINT:
        {
          PAINTSTRUCT ps;

          BeginPaint( win->get_handle(), &ps );
          win->exec_on_redraw();

          EndPaint( win->get_handle(), &ps );	
        }
        return 0;

      case WM_MOVE:
        {
          if( (window->flags & GLSK_WINDOW_FULLSCREEN ) == 0 )
          {
            RECT rectangle, workarea;

            SystemParametersInfo( SPI_GETWORKAREA, 0, (LPVOID)( &workarea ), 0);
            GetClientRect( window->handle, &rectangle );
            _glsk_window_translate_client_to_screen( window, &rectangle );

            window->px = rectangle.left;
            window->py = (workarea.bottom-workarea.top) - rectangle.bottom;
          }
        }
        return 0;

      case WM_SIZE:
        {
          window->width = LOWORD( lparam );
          window->height = HIWORD( lparam );

          if ( (window->flags & GLSK_WINDOW_FULLSCREEN ) == 0 )
          {
            window->uwidth = window->width;
            window->uheight = window->height;
          }

          if ( window->callback.configure_event )
            window->callback.configure_event( window, window->width, window->height );
          else
            break;
        }
        return 0;

      case WM_CLOSE:
        {
          win->close();
        }
        return 0;


      case WM_CHAR:
        {
          char	string[ 2 ] = { '\0', '\0' };

          string[ 0 ] = (char)wparam;

          if ( window->callback.char_event )
            window->callback.char_event( window, string );
          else
            break;
        }
        return 0;

      case WM_MOUSEMOVE:
        _glsk_send_mouse_event( window, LOWORD( lparam ),
            window->height-HIWORD( lparam ), GLSK_ME_TYPE_MOVE, 0 );
        return 0;

      case WM_LBUTTONUP:
        _glsk_send_mouse_event( window, LOWORD( lparam ),
            window->height-HIWORD( lparam ), GLSK_ME_TYPE_BUTTONRELEASED, 1 );
        return 0;

      case WM_MBUTTONUP:
        _glsk_send_mouse_event( window, LOWORD( lparam ),
            window->height-HIWORD( lparam ), GLSK_ME_TYPE_BUTTONRELEASED, 2 );
        return 0;

      case WM_RBUTTONUP:
        _glsk_send_mouse_event( window, LOWORD( lparam ),
            window->height-HIWORD( lparam ), GLSK_ME_TYPE_BUTTONRELEASED, 3 );
        return 0;

      case WM_LBUTTONDOWN:
        _glsk_send_mouse_event( window, LOWORD( lparam ),
            window->height-HIWORD( lparam ), GLSK_ME_TYPE_BUTTONPRESSED, 1 );
        return 0;

      case WM_MBUTTONDOWN:
        _glsk_send_mouse_event( window, LOWORD( lparam ),
            window->height-HIWORD( lparam ), GLSK_ME_TYPE_BUTTONPRESSED, 2 );
        return 0;

      case WM_RBUTTONDOWN:
        _glsk_send_mouse_event( window, LOWORD( lparam ),
            window->height-HIWORD( lparam ), GLSK_ME_TYPE_BUTTONPRESSED, 3 );
        return 0;

      default:
        break;
    };

    return DefwindowProc( hwindow, message, wparam, lparam );
  }

}

}

void application::register_class( HINSTANCE hi, TCHAR* appname )
{
	WNDCLASS c;

	if ( !hi )
		hi = GetModuleHandle( NULL );

	// init the wnd class
	c.style =			/*CS_HREDRAW | CS_VREDRAW |*/ CS_OWNDC;
	c.lpfnWndProc =		&application::window_proc;
	c.cbClsExtra =		0;
	c.cbWndExtra =		0;
	c.hInstance =		hi;
	c.hIcon =			LoadIcon( NULL, IDI_APPLICATION );
	c.hCursor =			LoadCursor( NULL, IDC_ARROW );
	c.hbrBackground =	static_cast<HBRUSH>(GetStockObject( GRAY_BRUSH ) );
	c.lpszMenuName =	0;
	c.lpszClassName =	appname;

	// register the class
	if ( !RegisterClass( &c ) )
	{
		MessageBox( NULL, TEXT( "this program requires Unicode support." ),
			appname, MB_ICONERROR );
	}
}


application::status application::handle_pending_messages()
{
  MSG message;
  if( process_idle.num_slots() )
  {
    while( PeekMessage( &message, NULL, 0,0,PM_REMOVE ) )
    {
      if ( message.message == WM_QUIT )
        return End;

      TranslateMessage( &message );
      DispatchMessage( &message );
    }
  }
  else 
  {
    if( GetMessage( &message, NULL, 0,0,PM_REMOVE ) )
    {
      if ( message.message == WM_QUIT )
        return End;

      TranslateMessage( &message );
      DispatchMessage( &message );
    }
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

gott::gui::x11::window* application::find_window( ::window handle )
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


bool application::use_fallback()const
{
  return old_glx;
}

bool application::is_extension_supported( std::string const& str ) const
{
  return extensions.find(str) != extensions.end();
}

void application::process_event( gott::gui::win32::window* win, XEvent& event )
{
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

