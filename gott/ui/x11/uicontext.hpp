// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
#ifndef GOTT_UI_X11_UICONTEXT_HPP_INCLUDED
#define GOTT_UI_X11_UICONTEXT_HPP_INCLUDED

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is a user interface abstraction library.
 *
 * The Initial Developer of the Original Code is
 * Andreas Pokorny (andreas.pokorny@gmail.com)
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Andreas Pokorny (andreas.pokorny@gmail.com)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <gott/ui/uicontext_base.hpp>
#include <gott/ui/window_base.hpp>
#include <gott/events/fd_manager.hpp>
#include <gott/events/main_loop.hpp>

namespace gott{namespace ui{namespace x11{

class window;
/**
 * \brief Xlib implementation of uicontext for X11.
 * \todo Use pimpl and some further abstraction to remove X11 headers, from our public headers?
 * \todo Should screen number be part of the window class?
 * \todo How should we react on illegal gott::x11::uicontext::register_window and 
 * gott::x11::uicontext::remove_window parameters?
 */
class GOTT_EXPORT uicontext : public uicontext_base {
  private:
    events::main_loop & loop_;
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
    uicontext( events::main_loop & loop, const char * connection = 0 );

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

