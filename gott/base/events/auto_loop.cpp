// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT main loop spawner
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

#include "auto_loop.hpp"

using gott::events::auto_loop;
using gott::events::main_loop;

class auto_loop::IMPL {

};

auto_loop::auto_loop() : p(new IMPL) {}
auto_loop::~auto_loop() {}

void auto_loop::spawn_noblock() {

}

void auto_loop::spawn_block() {

}

sigc::signal1<void, main_loop &> &auto_loop::add(loop_requirement const &e) {

}

bool auto_loop::try_feature(QID const &, loop_requirement const *) {
  return false;
}
