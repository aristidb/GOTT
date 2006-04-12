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
 * ***** END LICENSE BLOCK ***** */

#include "timer_manager.hpp"
#include "main_loop.hpp"
#include "deadline_timer.hpp"
#include "monotonic_timer.hpp"
#include <gott/range_algo.hpp>
#include <queue>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>

namespace pxtime = boost::posix_time;
using gott::events::timer_manager;
using gott::events::standard_timer_manager;

timer_manager::timer_manager() {}
timer_manager::~timer_manager() {}
gott::QID const timer_manager::qid("gott::events::timer_manager");

void timer_manager::add_deadline_timer(
      pxtime::ptime const &time_point,
      boost::function<void (timer_manager &)> const &callback,
      bool wait) {
  add_timer(deadline_timer(time_point, callback, wait));
}

void timer_manager::add_monotonic_timer(
    pxtime::time_duration const &time_point,
    boost::function<void (timer_manager &)> const &callback,
    bool wait) {
  add_timer(monotonic_timer(time_point, callback, wait));
}

void timer_manager::add_relative_timer(
    pxtime::time_duration const &interval,
    boost::function<void (timer_manager &)> const &callback,
    bool wait) {
  add_monotonic_timer(monotonic_clock() + interval, callback, wait);
}

namespace {
static void do_periodic_timer(
    pxtime::time_duration const &interval,
    boost::function<void ()> const &callback,
    bool wait,
    timer_manager &man) {
  callback();
  man.add_relative_timer(
      interval,
      boost::bind(&do_periodic_timer, interval, callback, wait, _1),
      wait);
}
}

void timer_manager::add_periodic_timer(
    pxtime::time_duration const &interval,
    boost::function<void ()> const &callback,
    bool wait) {
  add_relative_timer(
      interval, 
      boost::bind(&do_periodic_timer, interval, callback, wait, _1),
      wait);
}

class standard_timer_manager::impl {
public:
  typedef 
    std::priority_queue<
      deadline_timer, 
      std::vector<deadline_timer>,
      std::greater<deadline_timer> >
    scheduled_deadline_t;
  scheduled_deadline_t scheduled_deadline;

  typedef
    std::priority_queue<
      monotonic_timer,
      std::vector<monotonic_timer>,
      std::greater<monotonic_timer> >
    scheduled_monotonic_t;
  scheduled_monotonic_t scheduled_monotonic;
  
  unsigned waitfor;

  pxtime::time_duration min_wait;

  impl(pxtime::time_duration min_wait) : waitfor(0), min_wait(min_wait) {}
};

standard_timer_manager::standard_timer_manager(
    pxtime::time_duration const &min_wait)
  : p(new impl(min_wait)) {}
standard_timer_manager::~standard_timer_manager() {}

void standard_timer_manager::add_timer(deadline_timer const &tm) {
  if (tm.must_wait())
    ++p->waitfor;
  p->scheduled_deadline.push(tm);
}

void standard_timer_manager::add_timer(monotonic_timer const &tm) {
  if (tm.must_wait())
    ++p->waitfor;
  p->scheduled_monotonic.push(tm);
}

bool standard_timer_manager::has_timers() const {
  return !p->scheduled_deadline.empty() || !p->scheduled_monotonic.empty();
}

bool standard_timer_manager::has_wait_timers() const {
  return p->waitfor > 0;
}

void standard_timer_manager::handle_pending_timers() {
  { // handle deadline_timers
    impl::scheduled_deadline_t &scheduled_deadline = p->scheduled_deadline;

    pxtime::ptime now = pxtime::microsec_clock::universal_time();

    while (!scheduled_deadline.empty()) {
      deadline_timer current = scheduled_deadline.top();

      if (current.time_left(now) >= p->min_wait)
        break;

      if (current.must_wait())
        --p->waitfor;
      scheduled_deadline.pop();
      current.emit(*this);
    }
  }
  { // handle monotonic_timers
    pxtime::time_duration now = monotonic_clock();

    while (!p->scheduled_monotonic.empty()) {
      monotonic_timer current = p->scheduled_monotonic.top();

      if (current.time_left(now) >= p->min_wait)
        break;

      if (current.must_wait())
        --p->waitfor;
      p->scheduled_monotonic.pop();
      current.emit(*this);
    }
  }
}

boost::posix_time::time_duration standard_timer_manager::time_left() const {
  pxtime::time_duration ret = pxtime::seconds(0);
  if (!p->scheduled_deadline.empty())
    ret = p->scheduled_deadline.top().time_left();
  if (!p->scheduled_monotonic.empty())
    ret = std::min(ret, p->scheduled_monotonic.top().time_left());
  if (ret < p->min_wait)
    ret = pxtime::seconds(0);
  return ret;
}
