// Copyright (C) 2005-2006 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT main loop spawner test program
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
#include "loop_requirement.hpp"
#include "timer_manager.hpp"
#include "signal_manager.hpp"
#include "main_loop.hpp"
#include <signal.h>
#include <boost/bind.hpp>

using namespace gott::events;
using namespace boost::posix_time;
using boost::bind;

deadline_timer timefunc() {
  std::cout << "beep!" << std::endl;
  return deadline_timer(microsec_clock::local_time() + seconds(3), &timefunc);
}

void init_timer(main_loop &m) {
  m.feature<timer_manager>().add_timer(
      deadline_timer(
        microsec_clock::local_time(),
        &timefunc
        ));
}

void init_sig(main_loop &m, auto_loop *pa) {
  m.feature<signal_manager>().on_signal(SIGINT).connect(
      bind(&auto_loop::quit_all, pa));
}

int main() {
  auto_loop loops;
  loops.before_run(loops.add(feature<timer_manager>())).connect(&init_timer);
  loops.before_run(loops.add(feature<signal_manager>())).connect(bind(&init_sig, _1, &loops));
  loops.spawn_block();
}
