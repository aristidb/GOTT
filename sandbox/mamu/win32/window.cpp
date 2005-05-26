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
