// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT X11 Window class
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




#ifndef GOTT_GUI_X11_WINDOW_HPP_INLCUDED
#define GOTT_GUI_X11_WINDOW_HPP_INLCUDED

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <set>
#include <string>
#include "../widget_events.hpp"
#include <gott/util/visibility.hpp>

#ifdef USE_ANTIGRAIN
#include "antigrain.hpp"
#else
#include "cairo.hpp"
#endif

#ifdef HAVE_XDAMAGE
#include <X11/extensions/Xdamage.h>
#endif
#ifdef HAVE_XSYNC
#include <X11/extensions/sync.h>
#endif

namespace gott{ namespace gui{ namespace x11{

class application;
class GOTT_EXPORT window : public gott::gui::widget_events, public gott::gui::window_flags,
#ifdef USE_ANTIGRAIN
                           public antigrain 
#else
                             public cairo
#endif
{
  public:
    friend class gott::gui::x11::application;
  private:
    rect window_rect;
    application *app;
    ::Window		handle;	
    std::size_t flags;
    int xdamage_event_base, xdamage_error_base;
    bool use_xdamage, use_xsync, avail_request, needs_redraw;
    enum Protocols { DeleteWindow, Focus, Ping, ContextHelp, SyncRequest };
    ::Atom protocols[5];
    ::Atom wm_name, wm_icon_name, wm_type;
    window * parent;
#ifdef HAVE_XSYNC
    XSyncValue last_request;
    XSyncCounter counter;
#endif
#ifdef HAVE_XDAMAGE
    Damage damage;
#endif
    
    // Implementation specific functions:

  public:
    window( application& app, rect const& r, std::string const& title, pixelformat const& p, std::size_t flags );
    window( rect const& r, std::string const& title, pixelformat const& p, std::size_t flags );

    ~window();
   
    void swap_buffer(); ///< old
    
    void open( application& app, rect const&r, std::string const& title, pixelformat const& p, std::size_t flags );
    void open( rect const&r, std::string const& title, pixelformat const& p, std::size_t flags );

    void show();
    void hide();

    bool is_open() const;
    bool has_decoration() const;
    ::Window get_handle() const;
    rect const& get_rect() const;

    void close();
    
    void set_window_type( std::size_t flags );
    void set_visible( bool vis );
    void set_title( std::string const& title );
    void set_region( rect const& region );
    void set_render_context();

    void damage_region(rect const& region );


    // event handlers:
    virtual void on_redraw();
    virtual void on_focus_enter();
    virtual void on_focus_leave();
    virtual void on_close();
    virtual void on_configure( gott::gui::rect const&);
    virtual void on_mouse(gott::gui::mouse_event const&);
    virtual void on_key(gott::gui::key_event const&);


};

}}}

#endif


