// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Filesystem notification library
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

#include "engine.hpp"
#include "../watch.hpp"
#include <gott/syswrap/read_write_unix.hpp>
#include <algorithm>
#include <unistd.h>
extern "C" {
#include "inotify.h"
#include "inotify-syscalls.h"
}

#include <iostream>

using gott::string;
using gott::notify_fs::inotify_engine;
using gott::notify_fs::watch_implementation;
using gott::notify_fs::ev_t;
using gott::notify_fs::watch;

inotify_engine::inotify_engine() : conn(inotify_init()) {
  std::cout << "Inotify up and running..." << std::endl;
}

inotify_engine::~inotify_engine() {
  std::cout << "Shut down Inotify." << std::endl;
}

typedef sigc::signal1<void, gott::notify_fs::event const &> sgnl;

namespace {
boost::int32_t get_watch(inotify_engine &eng, string const &path, 
    ev_t mask) {
  char *c_path = new char[path.size() + 1];
  c_path[path.size()] = '\0';
  std::copy(path.as_utf8().begin(), path.as_utf8().end(), c_path);
  boost::int32_t result = inotify_add_watch(eng.conn.access(), c_path, mask);
  delete [] c_path;
  return result;
}

struct inotify_watch : watch_implementation {
  inotify_engine &eng;
  boost::int32_t wd;
  watch &context;

  inotify_watch(inotify_engine *e, string const &path, ev_t mask, watch &w)
  : eng(*e), wd(get_watch(eng, path, mask)), context(w) {
    if (wd < 0)
      throw gott::notify_fs::watch_installation_failure(path);
  }

  ~inotify_watch() {
    std::cout << "Remove watch from " << &eng.watches << " : " << wd << std::endl;
    eng.watches.erase(wd);
    inotify_rm_watch(eng.conn.access(), wd);
  }
};
}

watch_implementation *
inotify_engine::watch_alloc(string const &path, ev_t mask, watch &w) {
  inotify_watch *p = new inotify_watch(this, path, mask, w);
  std::cout << "Add watch to " << &watches << " : " << p->wd << std::endl;
  watches[p->wd] = p;
  return p;
}

#if 0 // reference
int read_events (queue_t q, int fd)
{
	char buffer[16384];
	size_t buffer_i;
	struct inotify_event *pevent, *event;
	ssize_t r;
	size_t event_size;
	int count = 0;

	r = read (fd, buffer, 16384);

	if (r <= 0) {
		perror ("read(fd, buffer, 16384) = ");
		return r;
	}

	buffer_i = 0;
	printf("read = %d\n", r);

	printf("sizeof inotify_event = %d\n", sizeof(struct inotify_event));
	while (buffer_i < r) {
		/* Parse events and queue them ! */
		pevent = (struct inotify_event *)&buffer[buffer_i];
		event_size = sizeof(struct inotify_event) + pevent->len;
		printf("pevent->len = %d\n", pevent->len);
		event = malloc(event_size);
		memmove(event, pevent, event_size);
		queue_enqueue(event, q);
		buffer_i += event_size;
		count++;
	}

	return count;
}
#endif

void inotify_engine::notify() {
  std::cout << "New events:" << std::endl;
  char buffer[16384];
  size_t r = read_unix(conn.access(), buffer, sizeof(buffer));
  for (size_t i = 0; i < size_t(r);) {
    inotify_event *pevent = reinterpret_cast<inotify_event *>(buffer + i);
    if (i + sizeof(inotify_event) >= sizeof(buffer) 
        || i + sizeof(inotify_event) + pevent->len >= sizeof(buffer)) {
      std::cout << "inotify: moveback" << std::endl;
      size_t rest = sizeof(buffer) - i;
      memmove(pevent, buffer, rest);
      i = 0;
      r = read_unix(conn.access(), buffer + rest, sizeof(buffer) - rest) + rest;
      continue;
    }
    i += sizeof(inotify_event) + pevent->len;
    if (pevent->mask & IN_Q_OVERFLOW)
      return;
    if (pevent->wd == -1) {
      std::cout << "inotify: Common event " << pevent->mask << std::endl;
      continue;
    }
    event ev = {
      static_cast<inotify_watch *>(watches[pevent->wd])->context,
      ev_t(pevent->mask),
      pevent->cookie,
      pevent->len ? pevent->name : ""
    };
    ev.context.on_fire().emit(ev);
  }
}
