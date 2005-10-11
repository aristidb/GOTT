// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT Input Frontend Code
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

#ifndef GOTT_GUI_WIDGETS_EVENTS_HPP_INCLUDED
#define GOTT_GUI_WIDGETS_EVENTS_HPP_INCLUDED


#include <boost/signals.hpp>
#include "utility.hpp"
#include "input.hpp"

namespace gott{namespace gui{

struct  GOTT_EXPORT widget_events
{
  public:
    typedef boost::signal<void()> simple_event_type;
    typedef boost::signal<void(gott::gui::rect const&)> size_signal_type;
    typedef boost::signal<void(gott::gui::mouse_event const&)> mouse_signal_type;
    typedef boost::signal<void(gott::gui::key_event const&)> key_signal_type;
  private:
    simple_event_type redraw_event, close_event, focus_enter, focus_leave;
    size_signal_type configure_event;
    mouse_signal_type mouse_ev;
    key_signal_type key_ev;

#if 0
    virtual void on_redraw() = 0;
    virtual void on_focus_enter() = 0;
    virtual void on_focus_leave() = 0;
    virtual void on_close() = 0;
    virtual void on_configure( gott::gui::rect const&) = 0;
    virtual void on_mouse(gott::gui::mouse_event const&) = 0;
    virtual void on_key(gott::gui::key_event const&) = 0;
#endif

  public:
    void set_on_redraw( simple_event_type::slot_type const& slot );
    void set_on_focus_enter( simple_event_type::slot_type const& slot );
    void set_on_focus_leave( simple_event_type::slot_type const& slot );
    void set_on_close( simple_event_type::slot_type const& slot );
    void set_on_configure( size_signal_type::slot_type const& slot );
    void set_on_mouse( mouse_signal_type::slot_type const& slot );
    void set_on_key( key_signal_type::slot_type const& slot );
    virtual ~widget_events();
  protected:
    void exec_on_redraw();
    void exec_on_focus_enter();
    void exec_on_focus_leave();
    void exec_on_close();
    void exec_on_configure( gott::gui::rect const&);
    void exec_on_mouse(gott::gui::mouse_event const&);
    void exec_on_key(gott::gui::key_event const&);
};

}}

#endif

