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

#include "timer_manager.hpp"
#include <queue>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace pxtime = boost::posix_time;
using gott::events::timer_manager;
using gott::events::standard_timer_manager;

timer_manager::timer_manager() {}
timer_manager::~timer_manager() {}
gott::QID const timer_manager::qid("gott::events::timer_manager");

class standard_timer_manager::impl {
public:
  typedef 
    std::priority_queue<
      deadline_timer, 
      std::vector<deadline_timer>,
      std::greater<deadline_timer> >
    pending_t;
  pending_t pending;
};

standard_timer_manager::standard_timer_manager() : p(new impl) {}
standard_timer_manager::~standard_timer_manager() {}

void standard_timer_manager::add_timer(deadline_timer const &tm) {
  if (tm.timer.is_not_a_date_time())
    return;
  p->pending.push(tm);
}

bool standard_timer_manager::has_timers() const {
  return !p->pending.empty();
}

void standard_timer_manager::handle_pending_timers() {
  impl::pending_t &pending = p->pending;

  pxtime::ptime now(pxtime::microsec_clock::local_time());

  while (!pending.empty()) {
    deadline_timer const &current = pending.top();
    pxtime::time_duration time_left = now - current.timer;
    if (time_left > pxtime::seconds(0))
      break;
    add_timer(current.handler());
    pending.pop();
  }
}

boost::posix_time::time_duration standard_timer_manager::time_left() const {
  pxtime::ptime now(pxtime::microsec_clock::local_time());
  return now - p->pending.top().timer;
}
