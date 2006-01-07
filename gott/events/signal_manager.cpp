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

#include "signal_manager.hpp"
#include <stdexcept>
#include <signal.h>
#include <map>

using gott::events::signal_manager;

signal_manager::signal_manager() {}
signal_manager::~signal_manager() {}

gott::QID const signal_manager::qid("gott::events::signal_manager");

namespace {
  typedef std::map<int, signal_manager *> handler_map;
  handler_map handlers;
}

void signal_manager::register_signal(int sig, signal_manager *handler) {
  if (handlers.find(sig) != handlers.end())
    throw std::runtime_error(
        "cannot register more than one signal_manager per signal");
  handlers[sig] = handler;
  signal(sig, signal_handler);
}

void signal_manager::unregister_all(signal_manager *handler) {
  handler_map::iterator it = handlers.begin();
  while (it != handlers.end())
    if (it->second == handler) {
      handler_map::iterator del = it++;
      handlers.erase(del);
    } else
      ++it;
}

signal_manager *signal_manager::find(int sig) {
  return handlers[sig];
}

void signal_manager::signal_handler(int sig) {
  find(sig)->immediate_action(sig);
}
