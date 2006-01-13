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
#include <sys/epoll.h>
#include <set>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/optional/optional.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <climits>

using gott::events::epoll_loop;

class epoll_loop::impl {
  static int epoll_create_wrapper(int size) {
    int result = epoll_create(size);
    // TODO check for errors
    return result;
  }

public:
  impl() : running(false), epoll_fd(epoll_create_wrapper(1024)) {}
  ~impl() { close(epoll_fd); }

  bool running;
  int epoll_fd;

  struct fd_entry {
    unsigned mask;
    boost::function<void (unsigned)> callback;
    bool wait;

    fd_entry(unsigned m, boost::function<void (unsigned)> const &cb, bool w)
    : mask(m), callback(cb), wait(w) {}
  };
  boost::ptr_map<int, fd_entry> fd_map;
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
  impl::fd_entry *e = new impl::fd_entry(mask, cb, wait);
  p->fd_map.insert(fd, e);

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
  ev.data.ptr = e;

  int result = epoll_ctl(p->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
  if (result == -1)
    throw fd_manager::installation_error();
}

void epoll_loop::remove_fd(int fd) {
  boost::ptr_map<int, impl::fd_entry>::iterator it = p->fd_map.find(fd);
  if (it->wait)
    p->wait_fds.erase(fd);
  p->fd_map.erase(it);
  int result = epoll_ctl(p->epoll_fd, EPOLL_CTL_DEL, fd, 0);
  if (result == -1)
    throw fd_manager::installation_error();
}

void epoll_loop::run() {
  p->running = true;
  while (p->running) {
    int timeout = -1;
    if (has_timers()) {
      handle_pending_timers();
      timeout = int(std::min(time_left().total_milliseconds(), long(INT_MAX)));
    } else if (p->wait_fds.empty())
      break;
    
    epoll_event event;
    int ready = epoll_wait(p->epoll_fd, &event, 1, timeout);
    if (ready > 0) {
      impl::fd_entry &e = *static_cast<impl::fd_entry *>(event.data.ptr);
      e.callback(e.mask);
    }
  }
}
