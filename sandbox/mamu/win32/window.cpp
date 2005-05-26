#include <iostream>
#include <boost/cstdint.hpp>
#include "window.hpp"
#include "application.hpp"

namespace gott{ namespace gui{ namespace win32{

Application *get_default_app(){ static Application instance; return &instance;}


window::~window()
{
   if( is_open() )
    close();
}


window::window( Application& app, rect const& r, std::string const& title, pixelformat const& p, std::size_t fl )
  : app( &app ), handle(0), dc(0), context(0), flags( Clear ), parent(0)
{
  open(r,title,p,flags);
}

window::window( rect const& r, std::string const& title, pixelformat const& p, std::size_t flags )
  : app( get_default_app() ), handle(0), dc(0), context(0), flags( Clear ), parent(0)
{
  open(r,title,p,flags);
}

bool window::is_open() const
{
  return flags & window_flags::Open;
}

void window::open( Application& a, rect const&r, std::string const& title, pixelformat const& p, std::size_t flags)
{
  if( is_open() )
    close();
  app = &a;

  open(r,title,p,flags);
}

void window::open(rect const&r, std::string const& t, pixelformat const& p, std::size_t fl )
{
  if( is_open() )
    close();

  format = p;

  handle = CreateWindowEx( WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, app->get_appname(), "",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,	NULL, NULL,	get_instance(), this);

  if( ! handle )
    throw runtime_error( "failed to create window"  );

  flags |= window_flags::Open;
 
  set_region( r );
  set_window_type( fl );
  set_title( t );

  app->register_window( this );

  if( fl & Visible )
    show();

  SetwindowLongPtr( handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this) );

  dc = GetDC( handle );
  if( dc == 0 )
    throw runtime_error("context could not be aquired");

}

void window::create_context()
{
  context = wglCreateContext( dc );
  if( context == 0 ) 
    throw runtime_error("Error creating context");
}

void window::set_pixel_format()
{
  std::vector<boost::int32_t> values(24);

  values.push_back( WGL_ACCELERATION_ARB );     values.push_back( WGL_FULL_ACCELERATION_ARB );
  values.push_back( WGL_PIXEL_TYPE_ARB );       values.push_back( WGL_TYPE_RGBA_ARB );       
  values.push_back( WGL_SUPPORT_OPENGL_ARB );   values.push_back( GL_TRUE );
  values.push_back( WGL_DRAW_TO_WINDOW_ARB );   values.push_back( GL_TRUE );

  if( format.flags & pixelformat::DoubleBuffer )  {
    values.push_back( WGL_DOUBLE_BUFFER_ARB );  values.push_back( GL_TRUE );
  }

  if( format.samples.first && app->is_extension_supported( "GL_ARB_multisample" ) )
  {
    values.push_back( WGL_SAMPLES_BUFFERS_ARB );  values.push_back( GL_TRUE );
    if( format.samples.second )  {
      values.push_back( WGL_SAMPLES_ARB );  values.push_back( format.samples.second );
    }
  }
  
  if( format.color.first  ) {
    values.push_back( WGL_COLOR_BITS_ARB ); values.push_back( format.color.second ); 
  }
  if( format.depth.first  ) {
    values.push_back( WGL_DEPTH_BITS_ARB ); values.push_back( format.depth.second ); 
  }
  if( format.stencil.first  ) {
    values.push_back( WGL_STENCIL_BITS_ARB ); values.push_back( format.stencil.second ); 
  }

  values.push_back( 0 );

  PIXELFORMATDESCRIPTOR	pfd;
  ZeroMemory( &pfd, sizeof( pfd ) );
  unsigned int			result_count = 0;
  int * descriptor;

  app->get_os_support().choose_pixelformat( dc, &(values[0]), 0,  1, descriptor, &result_count );
  if( result_count == 0 )
    throw runtime_error("Failure finding pixelformat");

  if( SetPixelFormat( dc, descriptor, &pfd  ) == 0 )
    throw runtime_error("Failure setting pixelformat");
  // try fallback?
}


void window::set_window_type( std::size_t fl )
{
  fl &= (Menu|Normal|Toolbar|Utility|Splash|Dock|ToolTip|Decoration);

  if( ToolTip || Splash || Dock )
    fl &= ~Decoration;
  else if( Menu || Normal || Utility || Toolbar )
    fl |= Decoration;
  
	if ( window->flags & GLSK_WINDOW_OPEN )
	{
		DWORD style, exstyle;
		RECT area;
		POINT p0, p1;
		int is_visible = IsWindowVisible( handle );

		if ( fl & Decoration )
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
		ClientToScreen( handle, &p0 );

		GetClientRect( handle, &area );
		AdjustWindowRectEx( &area, style, FALSE, exstyle );

		SetWindowLong( handle, GWL_STYLE, style );
		SetWindowLong( handle, GWL_EXSTYLE, exstyle );

		SetWindowPos( handle, 0, 0, 0,
			area.right-area.left, area.bottom-area.top,
			SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOMOVE );

		// we moved the client, undo that
		// get the client coords after the resize
		p1.x = p1.y = 0;
		ClientToScreen( handle, &p1 );	

		// calculate the movement delta
		p0.x = p1.x - p0.x;
		p0.y = p1.y - p0.y;

		if ( p0.x || p0.y )
      set_region( rect( p0.x + p1.x, p0.y + p1.y, area.right - area.left, area.bottom - area.top ) );

		if ( is_visible )
      show();

		// Invalidate the whole desktop area to force a redraw
		InvalidateRect( NULL, NULL, TRUE );
	}
}


void window::show()
{
  if( is_open() )
  {
    flags |= Visible;

    ShowWindow( handle, SW_SHOW );
    UpdateWindow( handle );
  }
}

void window::hide()
{
  if( is_open() )
  {
    flags &= ~Visible;
    ShowWindow( handle, SW_HIDE );
  }
}


void window::set_title( std::string const& t )
{
  if( is_open() )
    SetWindowText( handle, t.c_str() );
}



void window::close()
{
  if( ! is_open() )
    return;
  if( drawable && handle )
    exec_on_close();
  if( context )
    wglDeleteContext( context );
  
  ReleaseDC( handle, dc );
  DestroyWindow( handle );
  dc = 0;
  handle = 0;
  context = 0;
  flags &= ~window_flags::Open;
  app->remove_window( this );
}

void window::set_region( gott::gui::rect const& r ) 
{
  if(is_open() )
  {
    RECT area;
    area.left = r.left; 
    area.top = r.top; 
    area.bottom = r.top + r.height; 
    area.= r.top + r.height; 
    if( flags & window_flags::Visible )
      AdjustWindowRectEx( &area, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE );
    else
      AdjustWindowRectEx( &area, WS_POPUP, FALSE, WS_EX_APPWINDOW );
  }
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
}

void window::on_mouse(gott::gui::mouse_event const&)
{
}

void window::on_key(gott::gui::key_event const&)
{
}

void window::swap_buffer() 
{
  if( is_open() )
    SwapBuffers( dc );
}

void window::set_render_context()
{
  if( is_open() )
    wglMakeCurrent( dc, context );
}

HWND window::get_handle() const
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
