// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT Carbon Window class
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

#include <stdexcept>
#include "window.hpp"
namespace gott{namespace gui{namespace carbon{



pascal OSStatus window_close (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);

pascal OSStatus window_close (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData)
{
  window *win = reinterpret_cast<window*>(data);
	
  if( win ) {
    win->on_close();
    win->close();
  }

	return CallNextEventHandler (nextHandler, theEvent);
}

pascal OSStatus window_draw(EventHandlerCallRef next_handler, EventRef event, void* data)
{
  window *win = reinterpret_cast<window*>(data);

  if(win)
  {

    // draw ..
    
  }

	return CallNextEventHandler( next_handler, event );
}

window::window( application& app, rect const& r, std::string const& title, pixelformat const& p, std::size_t flags )
{
  open(app,r,title,p,flags);
}


//window::window( rect const& r, std::string const& title, pixelformat const& p, std::size_t flags );
void window::swap_buffer()
{
}


window::~window()
{
}


void window::open( application& app, rect const&r, std::string const& title, pixelformat const& p, std::size_t flags )
{
  WindowAttributes	windowAttrs;
  Rect				bounds;

  // window
  windowAttrs = kWindowCloseBoxAttribute | kWindowCollapseBoxAttribute | kWindowStandardHandlerAttribute;
  SetRect(&bounds, 0, 0, r.width, r.height);
  OffsetRect(&bounds, r.left, r.top);
  CreateNewWindow(kDocumentWindowClass, windowAttrs, &bounds, &ref);

  if(ref == 0)
  {
    throw std::runtime_error("Failure creating windo");
  }

  set_title( title );

  eventType.eventClass = kEventClassWindow;
  eventType.eventKind = kEventWindowClose;

  handlerUPP = NewEventHandlerUPP(window_close);
  InstallWindowEventHandler (m_specific->m_window, handlerUPP, 1, &eventType, this, NULL);

  eventType.eventKind = kEventWindowDrawContent;
  handlerUPP = NewEventHandlerUPP(window_draw);
  InstallWindowEventHandler (m_specific->m_window, handlerUPP, 1, &eventType, this, NULL);

 
  m_specific->create_pmap(width, height, &m_rbuf_window);
  m_initial_width = width;
  m_initial_height = height;
  on_init();
  on_resize(width, height);
  m_specific->m_redraw_flag = true;

  ShowWindow (m_specific->m_window);
  SetPortWindowPort (m_specific->m_window);
}

void window::show()
{
}


void window::hide()
{
}

bool window::is_open() const
{
}


bool window::has_decoration() const
{
}


void window::close()
{
}

void window::set_window_type( std::size_t flags )
{
}


void window::set_visible( bool vis )
{
}


void window::set_title( std::string const& title )
{
  // Change to kCFStringEncodingASCII, kCFStringEncodingMacRoman, kCFStringEncodingISOLatin1, kCFStringEncodingUTF8 or what else you need.
  SetWindowTitleWithCFString( ref, CFStringCreateWithCStringNoCopy(0, title.c_str(), kCFStringEncodingUTF8 , 0));
}


void window::set_region( rect const& region )
{
}


void window::set_render_context()
{
}


virtual void window::on_redraw()
{
}


virtual void window::on_focus_enter()
{
}


virtual void window::on_focus_leave()
{
}


virtual void window::on_close()
{
}


virtual void window::on_configure( gott::gui::rect const&)
{
}


virtual void window::on_mouse(gott::gui::mouse_event const&)
{
}


virtual void window::on_key(gott::gui::key_event const&)
{
}






}}}
