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
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#include "epoll_loop.hpp"
#include "sigselfpipe.hpp"
#include <gott/string/qid.hpp>
#include <gott/syswrap/epoll_linux.hpp>
#include <gott/syswrap/scoped_unix_file.hpp>
#include <gott/syswrap/system_error.hpp>
#include <gott/syswrap/errno.hpp>
#include <errno.h>
#include <set>
#include <map>
#include <boost/optional/optional.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <boost/cstdint.hpp>
#include <climits>

using gott::events::epoll_loop;

class epoll_loop::impl {
public:
  impl() : running(false), epoll_conn(epoll_create_linux(1024)) {}

  bool running;
  scoped_unix_file epoll_conn;

  struct fd_entry {
    unsigned mask;
    boost::function<void (unsigned)> callback;
    bool wait;

    fd_entry(unsigned m, boost::function<void (unsigned)> const &cb, bool w)
    : mask(m), callback(cb), wait(w) {}
  };
  std::map<int, fd_entry> fd_map;
  std::set<int> wait_fds;

  boost::optional<sigselfpipe> sigmgr;

  class entry {
    epoll_event event;
    boost::function<void ()> read, write, exception;
  };
  std::map<int, entry> file_descriptors;
};


epoll_loop::epoll_loop() 
: p(new impl) {}

epoll_loop::~epoll_loop() {}

void epoll_loop::quit_local() {
  p->running = false;
}

void *epoll_loop::do_feature(gott::QID const &qid) {
  GOTT_EVENTS_FEATURE(qid, fd_manager);
  GOTT_EVENTS_FEATURE(qid, timer_manager);
  if (qid == signal_manager::qid) {
    if (!p->sigmgr) p->sigmgr = boost::in_place(this);
    return p->sigmgr.get_ptr();
  }
  return 0;
}

void epoll_loop::add_fd(int fd, unsigned mask, 
    boost::function<void (unsigned)> const &cb, bool wait) {
  p->fd_map.insert(std::make_pair(fd, impl::fd_entry(mask, cb, wait)));

  if (wait)
    p->wait_fds.insert(fd);

  epoll_event ev;
  ev.events = 0;
  if (mask & fd_manager::read)
    ev.events |= EPOLLIN | EPOLLPRI;
  if (mask & fd_manager::write)
    ev.events |= EPOLLOUT;
  if (mask & fd_manager::exception)
    ev.events |= EPOLLERR | EPOLLHUP;
  ev.data.fd = fd;

  try {
    epoll_ctl_linux(p->epoll_conn.access(), EPOLL_CTL_ADD, fd, &ev);
  } catch (system_error const &) {
    throw fd_manager::installation_error();
  }
}

void epoll_loop::remove_fd(int fd) {
  std::map<int, impl::fd_entry>::iterator it = p->fd_map.find(fd);
  if (it == p->fd_map.end())
    throw fd_manager::installation_error();
  if (it->second.wait)
    p->wait_fds.erase(fd);
  p->fd_map.erase(it);
  try {
    epoll_ctl_linux(p->epoll_conn.access(), EPOLL_CTL_DEL, fd, 0);
  } catch (system_error const &) {
    throw fd_manager::installation_error();
  }
}

void epoll_loop::run() {
  p->running = true;
  while (p->running) {
    int timeout = -1;
    if (has_timers()) {
      handle_pending_timers();
      timeout = int(std::min(time_left().total_milliseconds(), 
            boost::int64_t(INT_MAX)));
    } 
    
    if (!has_wait_timers() && p->wait_fds.empty())
      break;
    
    epoll_event event[64];
    try {
      int ready = epoll_wait_linux(p->epoll_conn.access(), event, 64, timeout);
      for (int i = 0; i < ready; ++i) {
        int fd = event[i].data.fd;
        std::map<int, impl::fd_entry>::iterator it = p->fd_map.find(fd);
        if (it != p->fd_map.end()) {
          impl::fd_entry &e = it->second;
          e.callback(e.mask);
        }
      }
    } catch (errno_exception const &e) {
      if (e.number() != EINTR)
        throw;
    }
  }
}
