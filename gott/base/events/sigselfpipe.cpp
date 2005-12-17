// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
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

#include "sigselfpipe.hpp"
#include <boost/bind.hpp>
#include <unistd.h>
#include <signal.h>
#include <gott/range_algo.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>
#include <exception>

using gott::events::signal_manager;
using gott::events::sigselfpipe;

typedef sigc::signal1<void, int> signl_t;
NTL_MOVEABLE(signl_t)

sigselfpipe::sigselfpipe(fd_manager *fdm) {
  if (pipe(selfpipe) == -1)
    throw std::runtime_error("pipe() failed");
  fdm->add_read_fd(selfpipe[0], boost::bind(&sigselfpipe::notify_in, this));
}

sigselfpipe::~sigselfpipe() {
  signal_manager::unregister_all(this);
  close(selfpipe[0]);
  close(selfpipe[1]);
}

signl_t &sigselfpipe::on_signal(int sig) {
  int pos = handlers.Find(sig);
  if (pos < 0) {
    signal_manager::register_signal(sig, this);
    pos = handlers.FindAdd(sig);
    signal(sig, signal_handler);
  }
  return handlers[pos];
}

void sigselfpipe::signal_handler(int sig) {
  write(static_cast<sigselfpipe *>(signal_manager::find(sig))->selfpipe[1], 
      &sig, sizeof(sig));
}

void sigselfpipe::notify_in() {
  int sig;
  read(selfpipe[0], &sig, sizeof(sig));
  handlers.FindPtr(sig)->emit(sig);
}
