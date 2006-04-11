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
#include <set>
#include <map>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>


namespace gott {
namespace events {
  class kqueue_loop::impl {
  public:
    bool running;
    scoped_unix_file queue;

    struct callback {
      unsigned mask;
      boost::function<void (unsigned)> call;
    };
    typedef std::map<int,callback> map_fd_cb;
    map_fd_cb callbacks;

    std::set<int> wait_fds;

    impl()
      : running(false),
      queue(kqueue::create_bsd())
    {}
  };

  kqueue_loop::kqueue_loop()
    : standard_timer_manager(
#ifdef BOOST_DATE_TIME_HAS_NANOSEC // TODO: is this low wait reasonable?
			     boost::posix_time::nanoseconds(1)
#else
           boost::posix_time::microseconds(1)
#endif
			     ),
      p(new impl),
      message_mgr(this),
      sig_mgr(&message_mgr)
  {}

  kqueue_loop::~kqueue_loop() {}

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
    if(mask & fd_manager::read || mask & fd_manager::exception) {
      struct kevent n;
      EV_SET(&n, fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, 0);
      kqueue::event_bsd(p->queue.access(), &n, 1, 0, 0, 0);
    }
    if(mask & fd_manager::write) {
      struct kevent n;
      EV_SET(&n, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, 0);
      kqueue::event_bsd(p->queue.access(), &n, 1, 0, 0, 0);
    }

    if(wait)
      p->wait_fds.insert(fd);

    impl::callback new_cb;
    new_cb.call = cb;
    new_cb.mask = mask;
    p->callbacks[fd] = new_cb;
  }

  void kqueue_loop::remove_fd(int fd) {
    impl::map_fd_cb::iterator i=p->callbacks.find(fd);
    if(i == p->callbacks.end())
      throw system_error("could not remove fd");

    if(i->second.mask & fd_manager::read ||
       i->second.mask & fd_manager::exception) {
      struct kevent n;
      EV_SET(&n, fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
      kqueue::event_bsd(p->queue.access(), &n, 1, 0, 0, 0);
    }
    if(i->second.mask & fd_manager::write) {
      struct kevent n;
      EV_SET(&n, fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
      kqueue::event_bsd(p->queue.access(), &n, 1, 0, 0, 0);
    }

    p->callbacks.erase(i);
    p->wait_fds.erase(fd);
  }

  void kqueue_loop::quit_local() {
    p->running = false;
  }

  void kqueue_loop::run() {
    p->running = true;
    enum { EVENTS_N=64 };
    struct kevent events[EVENTS_N];
    while(p->running) {
      timespec tm;
      bool has_timers_mem = has_timers();
      if (has_timers_mem) {
        handle_pending_timers();
      	tm.tv_sec = 0;
      	tm.tv_nsec = int(std::min(time_left().total_nanoseconds(), 
        boost::int64_t(INT_MAX)));
      } 
    
      if (!has_wait_timers() && p->wait_fds.empty())
        break;

      int n=kqueue::event_bsd(p->queue.access(), 0, 0, events, EVENTS_N,
			      has_timers_mem ? &tm : 0x0);
      for(int i=0; i<n; ++i) {
        impl::map_fd_cb::iterator j=p->callbacks.find(events[i].ident);
        if(j == p->callbacks.end())
      	  continue; //is this an error?
        if(events[i].filter & EVFILT_READ) {
          if(!(j->second.mask & fd_manager::read))
            continue; //do we really have to check this and should it be an error?
          j->second.call(fd_manager::read);
        }
        else if(events[i].filter & EVFILT_WRITE) {
          if(!(j->second.mask & fd_manager::write))
            continue; //do we really have to check this and should it be an error?
          j->second.call(fd_manager::write);
      	}
      }
    }
  }
}}
