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
#include <gott/string/qid.hpp>
#include <sys/epoll.h>
#include <map>

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

void *epoll_loop::do_feature(gott::QID const &) {
  return 0;
}

void epoll_loop::run() {
  while (p->running) {
    epoll_event event;
    epoll_wait(p->epoll_fd, &event, 1, -1);
    //...
  }
}
