
/*
**
**		filename:		w_window.c
**
**		description:	window functions
**		target-system:	microsoft windows
**		version-info:   $Id$
**
*/

// system headers
#include <windows.h>
#include <gl/gl.h>

// global glsk header
#include <glsk/glsk.h>

// internal glsk headers
#include "w_main.h"
#include "w_window.h"
#include "w_pixelformat.h"
#include "w_input.h"

// a zero length string for return values
static const char* null_string = "";




glsk_window_t*
glsk_window_new()
{
	glsk_window_t*	window = (glsk_window_t*)malloc( sizeof( glsk_window_t ) );

	window->handle = 0;
	window->dc = 0;
	window->context = 0;

	window->callback.char_event = 0;
	window->callback.redraw_event = 0;
	window->callback.configure_event = 0;
	window->callback.create_event = 0;
	window->callback.close_event = 0;
	window->callback.destroy_event = 0;
	window->callback.pixelformat_select = 0;
	window->callback.mouse_event = 0;

	window->userdata = 0;
	window->flags = 0;

	window->width = window->uwidth = 0;
	window->height = window->uheight = 0;

	window->px = CW_USEDEFAULT;
	window->py = CW_USEDEFAULT;

	window->title = 0;

	return window;
}


void
glsk_window_free( glsk_window_t* window )
{
	free( window->title );
	free( window );
}


int
glsk_window_open( glsk_window_t* window, glsk_context_t* context )
{
	return _glsk_open_window( window, 
		global.hinstance, global.appname, context );
}


void
glsk_window_destroy( glsk_window_t* window )
{
	DestroyWindow( window->handle );
}

void
_glsk_send_mouse_event( glsk_window_t* window, int px, int py, int type, int button )
{
	if ( window->callback.mouse_event )
	{
		glsk_mouse_event_t	event;
		_glsk_init_mouse_event( &event, px, py, type, button );
		window->callback.mouse_event( window, &event );
	}
}

LRESULT CALLBACK
_glsk_wndproc( HWND hwindow, UINT message, WPARAM wparam, LPARAM lparam )
{
	// special part to init the userdata
	if ( message == WM_NCCREATE )
	{
		CREATESTRUCT*	Data = (CREATESTRUCT*)lparam;
		glsk_window_t*	window = (glsk_window_t*)Data->lpCreateParams;

		SetWindowLongPtr( hwindow, GWLP_USERDATA, (LONG_PTR)window );

		window->handle = hwindow;
		window->dc = GetDC( hwindow );

		return DefWindowProc( hwindow, message, wparam, lparam );
	}
	else
	{
		glsk_window_t*	window = (glsk_window_t*)GetWindowLongPtr( hwindow, GWLP_USERDATA );

		switch( message )
		{
			case WM_CREATE:
			{
				CREATESTRUCT*	Data = (CREATESTRUCT*)lparam;

				glsk_pixelformat_t pixelformat;
				_glsk_pixelformat_init( &pixelformat, GLSK_PF_DRAW_TO_WINDOW );

				if ( window->callback.pixelformat_select )
					window->callback.pixelformat_select( window, &pixelformat );

				_glsk_pixelformat_set_on_window( window, &pixelformat );

				if ( window->context->handle == NULL ) // create a physical context?
                    window->context->handle = wglCreateContext( window->dc );

				if ( window->context->handle == NULL )
					return -1;

				window->context->refcount++;				

				if ( window->callback.create_event )
				{
					if ( window->callback.create_event( window, Data->x, Data->y ) == 0 )
						return -1;
				}
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
				/*if ( (GetActiveWindow() == Message.hWindow) || (!IsIconic( Message.hWindow )) )
					OnActivate( bActive = true );
				else
					OnActivate( bActive = false );*/
			}
            return 0;

			case WM_PAINT:
			{
				PAINTSTRUCT ps;

				BeginPaint( window->handle, &ps );

				if ( window->callback.redraw_event )
					window->callback.redraw_event( window, 0 );

				EndPaint( window->handle, &ps );	
			}
			return 0;

			case WM_MOVE:
			{
				if ( (window->flags & GLSK_WINDOW_FULLSCREEN ) == 0 )
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
				if ( window->callback.close_event )
					window->callback.close_event( window, 0 );
				else
					glsk_window_destroy( window );
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

		return DefWindowProc( hwindow, message, wparam, lparam );
	}
}


int
glsk_window_select_rendercontext( glsk_window_t* window )
{
	return wglMakeCurrent( window->dc, window->context->handle );
}


void
glsk_window_swap_buffers( glsk_window_t* window )
{
	SwapBuffers( window->dc );
}


void
_glsk_register_class( HINSTANCE hinstance, TCHAR* appname )
{
	WNDCLASS c;

	if ( !hinstance )
		hinstance = GetModuleHandle( NULL );

	// init the wnd class
	c.style =			/*CS_HREDRAW | CS_VREDRAW |*/ CS_OWNDC;
	c.lpfnWndProc =		&_glsk_wndproc;
	c.cbClsExtra =		0;
	c.cbWndExtra =		0;
	c.hInstance =		hinstance;
	c.hIcon =			LoadIcon( NULL, IDI_APPLICATION );
	c.hCursor =			LoadCursor( NULL, IDC_ARROW );
	c.hbrBackground =	(HBRUSH)GetStockObject( GRAY_BRUSH );
	c.lpszMenuName =	0;
	c.lpszClassName =	appname;

	// register the class
	if ( !RegisterClass( &c ) )
	{
		MessageBox( NULL, TEXT( "this program requires Unicode support." ),
			appname, MB_ICONERROR );
	}
}

static void _glsk_window_update_visibility( glsk_window_t* window )
{
	if ( window->handle != 0 )
	{
		if ( window->flags & GLSK_WINDOW_IS_VISIBLE )
		{
			ShowWindow( window->handle, SW_SHOW );
			UpdateWindow( window->handle );
		}
		else
		{
			ShowWindow( window->handle, SW_HIDE );
		}
	}
}

int _glsk_open_window( glsk_window_t* window, HINSTANCE hinstance, TCHAR* appname, glsk_context_t* context )
{
	int	  x, y, w, h;
	int px, py;

	x = y = w = h = CW_USEDEFAULT;

	px = window->px;
	py = window->py;

	if ( context == 0 )
	{
		// TODO: create an own context
		return 0;
	}
	else
	{
		window->context = context;
	}

	window->handle = CreateWindowEx( WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, appname, "",
		WS_OVERLAPPEDWINDOW, x, y, w, h,	NULL, NULL,	hinstance, window );

	if ( window->handle == NULL )
	{
		return 0;
	}
	else
	{
		glsk_window_set_decorated( window, window->flags & GLSK_WINDOW_HAS_DECORATION );

		if ( ( window->uwidth > 0 ) && ( window->uheight > 0 ) )
			glsk_window_set_size( window, window->uwidth, window->uheight );

		if ( (px != CW_USEDEFAULT) && (py != CW_USEDEFAULT) )
			glsk_window_set_position( window, px, py );

		if ( window->title )
			glsk_window_set_title( window, window->title );

		_glsk_window_update_visibility( window );
	}

	return 1;
}


void
_glsk_window_translate_client_to_screen( glsk_window_t* window, RECT* rectangle )
{
	POINT delta = { 0, 0 };

	ClientToScreen( window->handle, &delta );

	// we can pretty much assume linear movement
	rectangle->left += delta.x;
	rectangle->right += delta.x;

	rectangle->top += delta.y;
	rectangle->bottom += delta.y;
}


void
_glsk_window_move_relative( glsk_window_t* window, int dx, int dy )
{
	RECT area;
	GetWindowRect( window->handle, &area );

	SetWindowPos( window->handle, 0, area.left+dx, area.top+dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER );	
}


int glsk_window_show( glsk_window_t* window )
{
	window->flags |= GLSK_WINDOW_IS_VISIBLE;

	_glsk_window_update_visibility( window );

	return 1;
}

void glsk_window_hide( glsk_window_t* window )
{
	window->flags &= ~GLSK_WINDOW_IS_VISIBLE;

	_glsk_window_update_visibility( window );
}

int glsk_window_is_visible( glsk_window_t* window )
{
	return window->handle && IsWindowVisible( window->handle );
}

void glsk_window_set_fullscreen( glsk_window_t* window, int value )
{
	if ( value )
		window->flags |= GLSK_WINDOW_FULLSCREEN;
	else
		window->flags &= ~GLSK_WINDOW_FULLSCREEN;

	if ( window->handle != NULL )
	{
		if ( window->flags & GLSK_WINDOW_FULLSCREEN )
		{
			DEVMODE mode;

			EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &mode );

			SetWindowPos( window->handle, HWND_TOPMOST, 0, 0,
				mode.dmPelsWidth, mode.dmPelsHeight, 0 );
			
		}
		else
		{
			glsk_window_set_position( window, window->px, window->py );
			glsk_window_set_size( window, window->uwidth, window->uheight );
		}
	}
}


void glsk_window_set_decorated( glsk_window_t* window, int value )
{
	// we don't have a win32 window yet, just store this for creation

	if ( value )
		window->flags |= GLSK_WINDOW_HAS_DECORATION;
	else
		window->flags &= ~GLSK_WINDOW_HAS_DECORATION;

	if ( window->handle != NULL )
	{
		DWORD style, exstyle;
		RECT area;
		POINT p0, p1;
		int is_visible = glsk_window_is_visible( window );

		if ( value )
		{
			style = WS_OVERLAPPEDWINDOW;
			exstyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		}
		else
		{
			style = WS_POPUP;
			exstyle = WS_EX_APPWINDOW;
		}

		// get client coordinates before the resize
		p0.x = p0.y = 0;
		ClientToScreen( window->handle, &p0 );

		GetClientRect( window->handle, &area );
		AdjustWindowRectEx( &area, style, FALSE, exstyle );

		SetWindowLong( window->handle, GWL_STYLE, style );
		SetWindowLong( window->handle, GWL_EXSTYLE, exstyle );

		//window->has_decoration = value;
        
		SetWindowPos( window->handle, 0, 0, 0,
			area.right-area.left, area.bottom-area.top,
			SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOMOVE );

		// we moved the client, undo that
		// get the client coords after the resize
		p1.x = p1.y = 0;
		ClientToScreen( window->handle, &p1 );	

		// calculate the movement delta
		p0.x = p1.x - p0.x;
		p0.y = p1.y - p0.y;

		if ( p0.x || p0.y )
		{
			_glsk_window_move_relative( window, -p0.x, -p0.y );
		}

		if ( is_visible )
		{
			glsk_window_show( window );
		}

		// Invalidate the whole desktop area to force a redraw
		InvalidateRect( NULL, NULL, TRUE );
	}
}


int glsk_window_get_decorated( glsk_window_t* window )
{
	return (window->flags & GLSK_WINDOW_HAS_DECORATION) != 0;
}


int glsk_window_get_fullscreen( glsk_window_t* window )
{
	return (window->flags & GLSK_WINDOW_FULLSCREEN) != 0;
}


void glsk_window_set_title( glsk_window_t* window, const char* title )
{
	if ( !title ) // accept NULL
	{
		title = null_string;
	}

	if ( title != window->title )
	{
		size_t length = strlen( title );

		free( window->title );

        if ( length )
		{
			window->title = (char*)malloc( length + 1 );
			strcpy( window->title, title );
		}
		else
		{
			window->title = 0;
		}
	}

	if ( window->handle )
	{
		if ( window->title )
            SetWindowText( window->handle, window->title );
		else
			SetWindowText( window->handle, "" );
	}
}


const char* glsk_window_get_title( glsk_window_t* window )
{
	if ( window->title )
		return window->title;
	else
		return null_string;
}


void glsk_window_set_size( glsk_window_t* window, int width, int height )
{
	window->uwidth = width;
	window->uheight = height;

	if ( window->handle )
	{
		RECT	a,b,n;

		GetClientRect( window->handle, &a );
		_glsk_window_translate_client_to_screen( window, &a );

		SetRect( &n, 0, 0, width, height );
		AdjustWindowRectEx( &n, GetWindowLong( window->handle, GWL_STYLE ), FALSE,
			GetWindowLong( window->handle, GWL_EXSTYLE ) );

		SetWindowPos( window->handle, HWND_TOP,
			0, 0, n.right-n.left, n.bottom-n.top, SWP_NOMOVE );
		
		GetClientRect( window->handle, &b );
		_glsk_window_translate_client_to_screen( window, &b );

		_glsk_window_move_relative( window, a.left-b.left, a.bottom-b.bottom );
	}
}


int glsk_window_get_width( glsk_window_t* window )
{
	return window->width;
}


int glsk_window_get_height( glsk_window_t* window )
{
	return window->height;
}

void glsk_window_get_size( glsk_window_t* window, int* width, int* height )
{
	*width = window->width;
	*height = window->height;
}

void glsk_window_get_position( glsk_window_t* window, int* px, int* py )
{
	*px = window->px;
	*py = window->py;
}


void glsk_window_set_position( glsk_window_t* window, int x, int y )
{
	// this is a bit more tricky since we need to translate into windows' shitty coordinate system

    window->px = x;
	window->py = y;

	if ( window->handle )
	{
		RECT		rectangle;
		RECT		workarea;
		int			dx, dy;

		SystemParametersInfo( SPI_GETWORKAREA, 0, (LPVOID)( &workarea ), 0);

		// calculate the desired position for the lower-left corner of the client rect in windows' coords
		dx = x;
		dy = (workarea.bottom - workarea.top)-y;

		GetClientRect( window->handle, &rectangle );
		_glsk_window_translate_client_to_screen( window, &rectangle );

		_glsk_window_move_relative( window, dx-rectangle.left, dy-rectangle.bottom );
	}
}


void glsk_window_set_userdata( glsk_window_t* window, void* userdata )
{
	window->userdata = userdata;
}


void* glsk_window_get_userdata( glsk_window_t* window )
{
	return window->userdata;
}


void glsk_window_set_callback_char_event( glsk_window_t* window, glsk_callback_window_string_t callback )
{
	window->callback.char_event = callback;
}


void glsk_window_set_callback_redraw_event( glsk_window_t* window, glsk_callback_window_voidp_t callback )
{
	window->callback.redraw_event = callback;
}


void glsk_window_set_callback_configure_event( glsk_window_t* window, glsk_callback_window_coord_t callback )
{
	window->callback.configure_event = callback;
}


void glsk_window_set_callback_create_event( glsk_window_t* window, glsk_callback_window_coord_t callback )
{
	window->callback.create_event = callback;
}


void glsk_window_set_callback_close_event( glsk_window_t* window, glsk_callback_window_voidp_t callback )
{
	window->callback.close_event = callback;
}


void glsk_window_set_callback_destroy_event( glsk_window_t* window, glsk_callback_window_voidp_t callback )
{
	window->callback.destroy_event = callback;
}


void glsk_window_set_callback_pixelformat_select( glsk_window_t* window, glsk_callback_window_pixelformat_t callback )
{
	window->callback.pixelformat_select = callback;
}

void glsk_window_set_callback_mouse_event( glsk_window_t* window, glsk_callback_window_mouse_event_t callback )
{
	window->callback.mouse_event = callback;
}
