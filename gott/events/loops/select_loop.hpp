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
 * The Original Code is An Event Handling Class Library.
 *
 * The Initial Developer of the Original Code is
 * Andreas Pokorny (andreas.pokorny@gmail.com )
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#ifndef GOTT_BASE_EVENTS_SELECT_LOOP_HPP_INCLUDED
#define GOTT_BASE_EVENTS_SELECT_LOOP_HPP_INCLUDED

#include <map> 
#include <algorithm>
#include <queue>
#include <set>
#include <sys/select.h> 
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/optional/optional.hpp>
#include <gott/visibility.hpp>
#include "../main_loop.hpp"
#include "../fd_manager.hpp"
#include "../timer_manager.hpp"
#include "../selfpipe_message_manager.hpp"
#include "../signal_manager.hpp"

namespace gott{namespace events{

/**
 * A main_loop implementation that uses select to dispatch events.
 *
 * Features:
 *  - gott::events::fd_manager
 *  - gott::events::timer_manager
 *  - gott::events::inprocess_message_manager
 *  - gott::events::signal_manager
 */
class select_loop 
    : public main_loop,
      private fd_manager, 
      private standard_timer_manager {
private:
  struct GOTT_LOCAL handler {
    boost::function<void (unsigned)> callback;
    unsigned mask;
  };
  struct GOTT_LOCAL fd_sets_t {
    fd_sets_t() {
      FD_ZERO(&read_fds);
      FD_ZERO(&write_fds);
      FD_ZERO(&except_fds);
    }
    fd_set read_fds, write_fds, except_fds;
  } fd_sets;
  typedef std::map<int, handler> callback_map;
  std::set<int> wait_fds;
  callback_map callbacks;
  selfpipe_message_manager message_mgr; 
  standard_signal_manager sig_mgr;
  bool running_;
  boost::signal<void ()> on_idle_;
  int wait;

public:
  select_loop();
  ~select_loop();

private:
  GOTT_LOCAL
  void add_fd(int, unsigned, boost::function<void (unsigned)> const &, 
      bool = true);

  GOTT_LOCAL
  void remove_fd(int);

private:
  GOTT_LOCAL void run();
  GOTT_LOCAL bool running() const;
  GOTT_LOCAL void quit_local();

  GOTT_LOCAL void add_waitable();
  GOTT_LOCAL void remove_waitable();

  GOTT_LOCAL boost::signal<void ()> &on_idle() { return on_idle_; }

private:
  GOTT_LOCAL void *do_feature(gott::QID const &);
};

}}

#endif

