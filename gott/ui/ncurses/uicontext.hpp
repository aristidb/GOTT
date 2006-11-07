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

#include <curses.h>
#include <gott/ui/uicontext_base.hpp>
#include <gott/ui/window_base.hpp>
#include <gott/events/fd_manager.hpp>
#include <gott/events/main_loop.hpp>

namespace gott{namespace ui{namespace ncurses{

class screen_activator;
class window;
/**
 * \brief ncurses implementation of uicontext for terminals.
 * \todo Should screen number be part of the window class?
 * \todo How should we react on illegal gott::ncurses::uicontext::register_window and 
 * gott::ncurses::uicontext::remove_window parameters?
 */
class GOTT_EXPORT uicontext 
  : public uicontext_base 
{
  private:
    events::main_loop & loop_;
    std::vector<gott::ui::ncurses::window*> windows_;
    int outfd_, infd_;
    SCREEN *terminal_;
    /**
     * \brief handles a window event.
     * \todo Test all uses of XSync and XFlush
     * \todo Focus handling not yet implemented
     * \todo window closing not yet resolved
     */
  public:

    /**
     * \brief Opens a curses terminal. 
     * \todo add a pty device string parameter to connect to arbitrary terminals of the user 
     * (if thats possible) and check ncurses how to support that. 
     */
    explicit uicontext( events::main_loop & loop, int outfd = 0, int infd = 1, char const* termtype = 0 );

    /**
     * Returns the terminals input file descriptor to use in main loop.
     */
    int get_input_descriptor() const;
    /**
     * Returns the terminals output file descriptor to use in main loop.
     * This descriptor might be identical to the input descriptor.
     */
    int get_output_descriptor() const;

    /**
     * \name select_loop handlers
     */
    //\{ 
    void process_read();
    void process_exception();
    /**
     * Resize signal handler. Xterm usually sends a SIGWINCH event
     * to notify about a terminal resize. 
     */
    void process_sigwinch();
    //\}

    void register_window( window_base * ref );
    void remove_window( window_base *ref );

    /**
     * \brief terminate the context.
     * A call to this method ends this user interface session, and might also 
     * quit the current application if no other control loop is running. 
     */
    void quit();

    /**
     * \name special ncurses functions
     * \{
     */

    // none yet
   
    //\}


    ~uicontext();

    friend class screen_activator;
};
}}}

#endif

