#ifndef GOTT_UI_X11_UICONTEXT_HPP_INCLUDED
#define GOTT_UI_X11_UICONTEXT_HPP_INCLUDED

// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT User interface context base class
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


#include <X11/X.h>
#include <X11/Xatom.h>
#include <gott/ui/uicontext_base.hpp>
#include <gott/ui/window_base.hpp>

namespace gott{namespace ui{namespace x11{

class window;
/**
 * \brief Xlib implementation of uicontext for X11.
 * \todo use pimpl and some further abstraction to remove X11 headers?
 * \todo should screen number be part of the window class?
 */
class GOTT_EXPORT uicontext : public uicontext_base {
  private:
    std::vector<gott::ui::x11::window*> windows_;
    Display * display_;
    int screen_; 
    Atom protocols_atom_;
    /**
     * \brief handles a window event.
     * \todo Test all uses of XSync and XFlush
     * \todo Focus handling not yet implemented
     * \todo window closing not yet resolved
     */
    void process_event( window* win, XEvent& e );
  public:

    /**
     * \brief Opens a xlib connection. 
     * If you omit the connection parameter, the default Display will be used to 
     * connect to the server.
     */
    uicontext( const char * connection = 0 );

    /**
     * \returns xlibs file descriptor to use in the select_loop main loop class
     */
    int get_descriptor() const;

    /**
     * \name select_loop handlers
     */
    //\{ 
    void process_read();
    void process_exception();
    //\}

    void register_window( window_base * ref );
    void remove_window( window_base *ref );

    gott::ui::x11::window* find_window( Window handle );


    /**
     * \brief terminate the context.
     * A call to this method ends this user interface session, and might also 
     * quit the current application if no other control loop is running. 
     */
    void quit();

    /**
     * \name special x11 functions
     * \{
     */
    /**
     * \brief returns the x11 display pointer.
     */
    Display* get_display();
    int get_screen() const;
    Atom get_atom( const char* atom ) const;
    //\}


    ~uicontext();
};
}}}

#endif

