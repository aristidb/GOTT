// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Andreas Pokorny (andreas.pokorny@gmail.com)
 *  Aristid Breitkreuz (aribrei@arcor.de)
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
#include <boost/bind.hpp>
#include <gott/ui/ncurses/uicontext.hpp>
// #include <gott/ui/ncurses/input.hpp>
#include <gott/graphics/geometry.hpp>
#include <gott/ui/ncurses/window.hpp>
#include <gott/events/fd_manager.hpp>
#include <gott/ui/ncurses/screen_activator.hpp>

using gott::ui::ncurses::uicontext;

uicontext::uicontext(
    events::main_loop &loop,
    int outfd, int infd,
    char const * termtype) 
: loop_(loop), outfd_(outfd), infd_(infd), terminal_(0)
{
  terminal_ = newterm(
      const_cast<char*>(termtype),
      fdopen(outfd_, "a"),
      fdopen(infd_, "r"));

  screen_activator(this);

  if (!terminal_) 
    throw system_error("Terminal creation failed.");

  try { 
    loop.feature<events::fd_manager>().add_fd(
        infd,
        events::fd_manager::read,
        boost::bind(&uicontext::process_read, this));
  } catch(...) {
    endwin();
    delscreen(terminal_);
    throw;
  }

  ::keypad(stdscr, true); // enable keyboard mapping 

  nonl();         /* tell curses not to do NL->CR/NL on output */
  cbreak();       /* take input chars one at a time, no wait for \n */
  echo();         /* echo input - in color */

  if (has_colors()) {
    start_color();
    //use_default_colors();

    /*
     * Simple color assignment, often all we need.  Color pair 0 cannot
     * be redefined.  This example uses the same value for the color
     * pair as for the foreground color, though of course that is not
     * necessary:
     */
#if 0
    init_pair(1, COLOR_RED,     -1);
    init_pair(2, COLOR_GREEN,   -1);
    init_pair(3, COLOR_YELLOW,  -1);
    init_pair(4, COLOR_BLUE,    -1);
    init_pair(5, COLOR_CYAN,    -1);
    init_pair(6, COLOR_MAGENTA, -1);
    init_pair(7, COLOR_WHITE,   -1);
#else
    init_pair(1, COLOR_RED,     COLOR_BLACK);
    init_pair(2, COLOR_GREEN,   COLOR_BLACK);
    init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
    init_pair(4, COLOR_BLUE,    COLOR_BLACK);
    init_pair(5, COLOR_CYAN,    COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_WHITE,   COLOR_BLACK);
#endif
  }
}

void uicontext::register_window(window_base *ref) {
  window *curseswin = dynamic_cast<window*>(ref);
  if (curseswin)
    windows_.push_back(curseswin);
  else {
    // do what?
  }
}

void uicontext::remove_window(window_base *ref) {
  window *curseswin = dynamic_cast<window*>(ref);
  if (curseswin) {
    std::vector<gott::ui::ncurses::window*>::iterator it =
      find(windows_.begin(), windows_.end(), curseswin);
    if (it != windows_.end())
      windows_.erase(it);
  } else {
    // do what?
  }

}

void uicontext::quit() {
}

int uicontext::get_output_descriptor() const {
  return outfd_;
}

int uicontext::get_input_descriptor() const {
  return infd_;
}

void uicontext::process_read() {
}

void uicontext::process_exception() {
}
void uicontext::process_sigwinch()
{
  screen_activator(this);
  endwin();
  refresh();
  // run repaint here.. 
}

uicontext::~uicontext() {
  screen_activator(this);
  endwin();
  delscreen(terminal_);
}

