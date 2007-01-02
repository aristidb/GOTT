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
 * Rüdiger Sonderfeld <kingruedi@c-plusplus.de>.
 * Portions created by the Initial Developer are Copyright (C) 2006, 2007
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Rüdiger Sonderfeld <kingruedi@c-plusplus.de>
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

#ifndef GOTT_EVENTS_KQUEUE_LOOP_HPP
#define GOTT_EVENTS_KQUEUE_LOOP_HPP

#include "../main_loop.hpp"
#include "../fd_manager.hpp"
#include "../timer_manager.hpp"
#include "../selfpipe_message_manager.hpp"
#include "../signal_manager.hpp"
#include <boost/scoped_ptr.hpp>

namespace gott {
namespace events {

/**
 * A main_loop implementation using kqueue(2) which is specific to FreeBSD,
 *   MacOSX, NetBSD, OpenBSD and propably more
 *
 * Features:
 *  - gott::events::fd_manager
 *  - gott::events::timer_manager
 *  - gott::events::inprocess_message_manager
 *  - gott::events::signal_manager
 */
class GOTT_EXPORT kqueue_loop
  : public main_loop,
    private fd_manager,
    private standard_timer_manager,
    private signal_manager
{
 public:
  kqueue_loop();
  ~kqueue_loop();

 private:
  GOTT_LOCAL void run();
  GOTT_LOCAL void quit_local();
  GOTT_LOCAL boost::signal<void ()> &on_idle();

 public:
  // Interface for notify_fs
  void watch_fd(int fd, unsigned mask,
		boost::function<void (unsigned)> const &cb,
		bool wait);
  void unwatch_fd(int fd);

 private:
  GOTT_LOCAL void add_waitable();
  GOTT_LOCAL void remove_waitable();

  GOTT_LOCAL bool running() const; //TODO

 private:
  GOTT_LOCAL void add_fd(int fd, unsigned mask,
        		 boost::function<void (unsigned)> const &cb,
        		 bool wait = true);

  GOTT_LOCAL void remove_fd(int fd);

  GOTT_LOCAL boost::signal<void (int)> &on_signal(int sig);
  
  GOTT_LOCAL void *do_feature(QID const &);

  class impl;
  boost::scoped_ptr<impl> p;
  selfpipe_message_manager message_mgr;
};

}}

#endif
