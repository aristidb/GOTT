#include "utility.hpp"

namespace gott{ namespace gui{ 


rect::rect( long l, long t, std::size_t w, std::size_t h)
  : left(l), top(t), width(w), height(h)
{
}
rect::rect()
  : left(0), top(0), width(1), height(1)
{}
  
pixelformat::pixelformat()
  : flags( DoubleBuffer ), color(false,1), depth(false,1), stencil(false,1), samples(false,0)
{
}

void widget_events::set_on_redraw( simple_event_type::slot_type const& slot )
{
  redraw_event.connect( slot );
}

void widget_events::set_on_focus_enter( simple_event_type::slot_type const& slot )
{
  focus_enter.connect( slot );
}

void widget_events::set_on_focus_leave( simple_event_type::slot_type const& slot )
{
  focus_leave.connect( slot );
}

void widget_events::set_on_close( simple_event_type::slot_type const& slot )
{
  close_event.connect( slot );
}

void widget_events::set_on_configure( size_signal_type::slot_type const& slot )
{
  configure_event.connect( slot );
}

void widget_events::set_on_mouse( mouse_signal_type::slot_type const& slot )
{
  mouse_ev.connect( slot );
}

void widget_events::set_on_key( key_signal_type::slot_type const& slot )
{
  key_ev.connect( slot );
}

void widget_events::exec_on_redraw()
{
  on_redraw();
  redraw_event();
}

void widget_events::exec_on_focus_enter()
{
  on_focus_enter();
  focus_enter();
}

void widget_events::exec_on_focus_leave()
{
  on_focus_leave();
  focus_leave();
}

void widget_events::exec_on_close()
{
  on_close();
  close_event();
}

void widget_events::exec_on_configure( gott::gui::rect const&r )
{
  on_configure(r);
  configure_event(r);
}

void widget_events::exec_on_mouse(gott::gui::mouse_event const&m)
{
  on_mouse(m);
  mouse_ev(m);
  
}

void widget_events::exec_on_key(gott::gui::key_event const&k)
{
  on_key(k);
  key_ev(k);
}

widget_events::~widget_events()
{
}


}}
