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
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#include "kqueue_loop.hpp"
#include <gott/syswrap/kqueue_bsd.hpp>
#include <gott/syswrap/scoped_unix_file.hpp>
#include <vector>

namespace gott {
namespace events {
  class kqueue_loop::impl {
  public:
    bool running;
    scoped_unix_file fd;

    std::vector<kevent> in_events;

    struct callback {
      unsigned mask;
      boost::function<void (unsigned)> call;
    };
    std::vector<callback> callbacks;

    impl()
      : running(false), fd(kqueue_create())
    {}
  };

  kqueue_loop::kqueue_loop()
    : standard_timer_manager(boost::posix_time::nanoseconds(1)),
      p(new impl),
      message_mgr(this),
      sig_mgr(&message_mgr)
  { }

  void *kqueue_loop::do_feature(gott::QID const &qid) {
    GOTT_EVENTS_FEATURE(qid, fd_manager);
    GOTT_EVENTS_FEATURE(qid, timer_manager);
    if (qid == inprocess_message_manager::qid)
      return &message_mgr;
    if (qid == signal_manager::qid)
      return &sig_mgr;
    return 0;
  }

  void kqueue_loop::add_fd(int fd, unsigned mask, 
			   boost::function<void (unsigned)> const &cb,
			   bool wait)
  {
    if(mask & fd_manager::read) {
      kevent n;
      EV_SET(&n, fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0,
	     reinterpret_cast<void*>(wait));
      p->in_events.push_back(n);
    }
    if(mask & fd_manager::write) {
      kevent n;
      EV_SET(&n, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0,
	     reinterpret_cast<void*>(wait));
      p->in_events.push_back(n);
    }
    if(mask & fd_manager::exception) {
      // ??
    }

    callback new_cb;
    new_cb.call = cb;
    new_cb.mask = mask;
    callbacks.push_back(new_cb);
  }

  namespace {
    struct find_fd {
      int fd;
      find_fd(int f) : fd(f) { }
      bool operator()(kevent const &event) {
	return event.ident == fd;
      }
    };
  }

  void kqueue_loop::remove_fd(int fd) {
    find_fd fnd(fd);
    std::vector<kevent>::iterator i=std::find(p->in_events.begin(),
					      p->in_events.end(), fnd);
    if(i == p->in_events.end())
      throw system_error("could not remove fd");

    i->flags = EV_DELETE;

  }

  void kqueue_loop::quit_local() {
    p->running = false;
  }

  void kqueue_loop::run() {
    p->running = true;
    while(p->running) {
      
    }
  }
}}
