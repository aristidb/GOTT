#include <iostream>
#include "window.hpp"
#include "input.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

namespace gott{namespace gui{
  
void window::set_on_key( key_signal_type::slot_type const& slot )
{
  key_ev.connect( slot );
}

void window::set_on_redraw( simple_event_type::slot_type const& slot )
{
  redraw_event.connect( slot );
}

void window::set_on_close( simple_event_type::slot_type const& slot )
{
  close_event.connect( slot );
}

void window::set_on_destroy( simple_event_type::slot_type const& slot )
{
  destroy_event.connect( slot );
}

void window::set_on_idle( simple_event_type::slot_type const& slot )
{
  idle_event.connect( slot );
}
void window::set_on_configure( size_signal_type::slot_type const& slot )
{
  configure_event.connect( slot );
}
void window::set_on_mouse( mouse_signal_type::slot_type const& slot )
{
  mouse_ev.connect( slot );
}

void window::on_key_event( gott::gui::key_event const& k)
{
  if( k.type == key_event::Press && k.code == KeyQ )
  {
    application::get_instance().remove_window( this );
  }
}

void window::on_redraw()
{

  set_rendercontext();
  std::cout << "on_redraw" << std::endl;
//  glClearColor( 0.8, 0.8,0.8,1.0);
  glClear( GL_COLOR_BUFFER_BIT |   GL_DEPTH_BUFFER_BIT );

  swap_buffers();
}

void window::on_configure( int w,int h)
{
  set_rendercontext();
  std::cout << "Configure event" << std::endl;
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(0,w,0,h);
  glClearColor( 0.8, 0.8,0.8,1.0);
}

void window::on_create( int w,int h )
{
  std::cout << "Createevent" << std::endl;
  on_configure(w,h);
}
void window::on_close(){}
void window::on_destroy(){}
void window::on_idle(){}
void window::on_mouse_event(gott::gui::mouse_event const&){}

void window::key( gott::gui::key_event const& k )
{
  on_key_event(k);
  key_ev(k);
}

void window::redraw()
{
  std::cout << "redraw" << std::endl;
  on_redraw();
  redraw_event();
}

void window::configure( int x,int y)
{
  on_configure(x,y);
  configure_event(x,y);
}

void window::create( int x ,int y)
{
  on_create(x,y);
}

void window::close()
{
  on_close();
  close_event();
}
void window::destroy()
{
  on_destroy();
  destroy_event();
}
void window::idle()
{
  on_idle();
  idle_event();
}

void window::mouse(gott::gui::mouse_event const& m)
{
  on_mouse_event(m);
  mouse_ev(m);
}

}}
