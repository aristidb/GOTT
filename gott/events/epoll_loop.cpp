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
#include <gott/string/qid.hpp>
#include <gott/syswrap/epoll_linux.hpp>
#include <gott/syswrap/scoped_unix_file.hpp>
#include <gott/exceptions.hpp>
#include <gott/syswrap/errno.hpp>
#include <errno.h>
#include <set>
#include <map>
#include <boost/optional/optional.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <boost/cstdint.hpp>
#include <climits>
#include <cmath>

using gott::events::epoll_loop;

class epoll_loop::impl {
public:
  impl() 
    : running(false),
    epoll_conn(epoll_create_linux(1024)),
    wait(0)
  {}

  bool running;
  scoped_unix_file epoll_conn;
  int wait;

  struct fd_entry {
    unsigned mask;
    boost::function<void (unsigned)> callback;
    bool wait;

    fd_entry(unsigned m, boost::function<void (unsigned)> const &cb, bool w)
    : mask(m), callback(cb), wait(w) {}
  };
  std::map<int, fd_entry> fd_map;
  std::set<int> wait_fds;

  class entry {
    epoll_event event;
    boost::function<void ()> read, write, exception;
  };
  std::map<int, entry> file_descriptors;

  sigc::signal0<void> on_idle;
};


epoll_loop::epoll_loop() 
: standard_timer_manager(this),
  p(new impl),
  message_mgr(this),
  sig_mgr(&message_mgr) {}

epoll_loop::~epoll_loop() {
  on_destroy().emit();
}

void epoll_loop::quit_local() {
  p->running = false;
}

sigc::signal0<void> &epoll_loop::on_idle() {
  return p->on_idle;
}

void *epoll_loop::do_feature(gott::QID const &qid) {
  GOTT_EVENTS_FEATURE(qid, fd_manager);
  GOTT_EVENTS_FEATURE(qid, timer_manager);
  if (qid == inprocess_message_manager::qid)
    return &message_mgr;
  if (qid == signal_manager::qid)
    return &sig_mgr;
  return 0;
}

void epoll_loop::add_waitable() {
  ++p->wait;
}

void epoll_loop::remove_waitable() {
  --p->wait;
}

void epoll_loop::add_fd(int fd, unsigned mask, 
    boost::function<void (unsigned)> const &cb, bool wait) {
  p->fd_map.insert(std::make_pair(fd, impl::fd_entry(mask, cb, wait)));

  if (wait) 
    add_waitable();

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
    throw system_error("could not add fd");
  }
}

void epoll_loop::remove_fd(int fd) {
  std::map<int, impl::fd_entry>::iterator it = p->fd_map.find(fd);
  if (it == p->fd_map.end())
    throw system_error("could not remove fd");
  if (it->second.wait)
    remove_waitable();
  p->fd_map.erase(it);
  try {
    epoll_ctl_linux(p->epoll_conn.access(), EPOLL_CTL_DEL, fd, 0);
  } catch (system_error const &) {
    throw system_error("could not remove fd");
  }
}

void epoll_loop::run() {
  p->running = true;
  while (p->running && p->wait > 0) {
    int timeout = -1;
    if (has_timers()) {
      typedef boost::int64_t i64;
      timeout = int(std::min(
          i64(std::floor(time_left(true).total_microseconds() / 1000.0 + 0.5)),
          i64(INT_MAX)));
      if (!has_timers())
        timeout = -1;
    }

    p->on_idle.emit();
    
    if (!p->running || p->wait <= 0)
      break;
    
    epoll_event event[64];
    try {
      int ready = epoll_wait_linux(p->epoll_conn.access(), event, 64, timeout);
      for (int i = 0; i < ready; ++i) {
        int fd = event[i].data.fd;
        std::map<int, impl::fd_entry>::iterator it = p->fd_map.find(fd);
        if (it != p->fd_map.end()) {
          impl::fd_entry &e = it->second;
          unsigned mask = 0;
          if (event[i].events & (EPOLLIN | EPOLLPRI))
            mask |= fd_manager::read;
          if (event[i].events & EPOLLOUT)
            mask |= fd_manager::write;
          if (event[i].events & (EPOLLERR | EPOLLHUP))
            mask |= fd_manager::exception;
          e.callback(mask);
        }
      }
    } catch (errno_exception const &e) {
      if (e.number() != EINTR)
        throw;
    }
  }
}
