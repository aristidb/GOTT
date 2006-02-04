/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is An Event Handling Class Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** *//

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
    scheduled_t;
  scheduled_t scheduled;
  unsigned waitfor;

  impl() : waitfor(0) {}
};

standard_timer_manager::standard_timer_manager() : p(new impl) {}
standard_timer_manager::~standard_timer_manager() {}

void standard_timer_manager::add_timer(deadline_timer const &tm) {
  if (tm.timer.is_not_a_date_time())
    return;
  if (tm.wait)
    ++p->waitfor;
  p->scheduled.push(tm);
}

bool standard_timer_manager::has_timers() const {
  return !p->scheduled.empty();
}

bool standard_timer_manager::has_wait_timers() const {
  return p->waitfor > 0;
}

void standard_timer_manager::handle_pending_timers() {
  impl::scheduled_t &scheduled = p->scheduled;

  pxtime::ptime now = pxtime::microsec_clock::local_time();

  while (!scheduled.empty()) {
    deadline_timer const &current = scheduled.top();

    if (now < current.timer)
      break;

    deadline_timer new_timer = current.handler();
    if (current.wait)
      --p->waitfor;
    scheduled.pop();
    add_timer(new_timer);
  }
}

boost::posix_time::time_duration standard_timer_manager::time_left() const {
  pxtime::ptime now = pxtime::microsec_clock::local_time();
  pxtime::time_duration ret = p->scheduled.top().timer - now;
  if (ret < pxtime::seconds(0))
    return pxtime::seconds(0);
  return ret;
}
