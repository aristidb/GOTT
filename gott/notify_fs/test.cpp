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

#include "watch.hpp"
#include "inotify/engine.hpp"
#include <gott/events/select_loop.hpp>
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

int main() {
  static inotify_engine ee;
  default_engine = &ee;
  watch w("/tmp/testfile", all_events);
  watch w2("/tmp", all_events);
  w.on_fire().connect(boost::bind(&output, _1, 1));
  w2.on_fire().connect(boost::bind(&output, _1, 2));
  boost::scoped_ptr<main_loop> loop(new select_loop);
  loop->feature<fd_manager>()
    .add_read_fd(ee.fd, boost::bind(&inotify_engine::notify, &ee));
  loop->feature<signal_manager>().on_signal(SIGINT).connect(
    boost::bind(&main_loop::quit, boost::ref(*loop)));
  loop->run();
}
