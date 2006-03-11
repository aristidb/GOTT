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
 * The Original Code is A Filesystem Notification Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
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

#include "watch.hpp"
#include "inotify/engine.hpp"
#include <gott/events/select_loop.hpp>
#include <gott/events/epoll_loop.hpp>
#include <gott/events/fd_manager.hpp>
#include <gott/events/signal_manager.hpp>
#include <gott/events/timer_manager.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <signal.h>

using namespace gott::notify_fs;
using namespace gott::events;

void output(event const &ev, int context) {
  std::cout << context;
  std::cout << "ev" << std::hex << ev.flags;
  std::cout << " cookie" << ev.cookie;
  std::cout << " name " << ev.name;
  std::cout << std::endl;
}

void blink() {
  std::cout << boost::posix_time::microsec_clock::local_time() << " blink" << std::endl;
}

int main() {
  static inotify_engine ee;
  default_engine = &ee;
  watch w("/tmp/testfile", all_events);
  watch w2("/tmp", all_events);
  w.on_fire().connect(boost::bind(&output, _1, 1));
  w2.on_fire().connect(boost::bind(&output, _1, 2));

  boost::scoped_ptr<main_loop> loop(
#if 1
      new select_loop
#else
      new epoll_loop
#endif
  );

#if 1
  loop->feature<fd_manager>().add_fd(
    ee.conn.access(),fd_manager::read,boost::bind(&inotify_engine::notify,&ee));
#endif
#if 1
  loop->feature<signal_manager>().on_signal(SIGINT).connect(
    boost::bind(&main_loop::quit_local, boost::ref(*loop)));
#endif
#if 1
  loop->feature<timer_manager>().add_timer(
    periodic_timer(boost::posix_time::seconds(3), &blink, true, true));
#endif
  loop->run();
}
