// Copyright (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT main loop
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "epoll_loop.hpp"
#include "sigselfpipe.hpp"
#include <gott/string/qid.hpp>
#include <gott/syswrap/epoll_linux.hpp>
#include <gott/syswrap/scoped_unix_file.hpp>
#include <gott/syswrap/system_error.hpp>
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

void epoll_loop::quit() {
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
    int ready = epoll_wait_linux(p->epoll_conn.access(), event, 64, timeout);
    for (int i = 0; i < ready; ++i) {
      int fd = event[i].data.fd;
      std::map<int, impl::fd_entry>::iterator it = p->fd_map.find(fd);
      if (it != p->fd_map.end()) {
        impl::fd_entry &e = it->second;
        e.callback(e.mask);
      }
    }
  }
}
