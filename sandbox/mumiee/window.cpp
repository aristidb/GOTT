#include "window.hpp"

namespace gott{namespace gui{
  
void window::set_on_key( key_signal_type::slot_type const& slot )
{
  key_event.connect( slot );
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

void window::on_key( std::string const& ){}
void window::on_redraw(){}
bool window::on_configure( int,int){}
bool window::on_create( int,int){}
void window::on_close(){}
void window::on_destroy(){}
void window::on_idle(){}
void window::on_mouse_event(gott::gui::mouse_event const&){}

void window::key( std::string const& k )
{
  on_key(k);
  key_event(k);
}

void window::redraw()
{
  on_redraw();
  redraw_event();
}

bool window::configure( int x,int y)
{
  on_configure(x,y);
  configure_event(x,y);
}

bool window::create( int x ,int y)
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
