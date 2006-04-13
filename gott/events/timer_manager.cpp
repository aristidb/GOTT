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
#include <boost/optional.hpp>
#include <boost/none.hpp>

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
    pxtime::time_duration const &last,
    pxtime::time_duration const &interval,
    boost::function<bool ()> const &callback,
    bool wait,
    timer_manager &man) {
  if (callback())
    man.add_monotonic_timer(
        last + interval,
        boost::bind(&do_periodic_timer,
          last + interval, interval, callback, wait, _1),
        wait);
}
}

void timer_manager::add_periodic_timer(
    pxtime::time_duration const &interval,
    boost::function<bool ()> const &callback,
    bool wait) {
  pxtime::time_duration now = monotonic_clock();
  add_monotonic_timer(
      now + interval, 
      boost::bind(&do_periodic_timer,
        now + interval, interval, callback, wait, _1),
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

  boost::optional<pxtime::time_duration> saved_monotonic_time;

  impl() : waitfor(0) {}
};

standard_timer_manager::standard_timer_manager()
  : p(new impl()) {}
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

void standard_timer_manager::add_relative_timer(
    pxtime::time_duration const &interval,
    boost::function<void (timer_manager &)> const &callback,
    bool wait) {
  pxtime::time_duration now = 
    p->saved_monotonic_time ? *p->saved_monotonic_time : monotonic_clock();
  add_monotonic_timer(now + interval, callback, wait);
}

bool standard_timer_manager::has_timers() const {
  return !p->scheduled_deadline.empty() || !p->scheduled_monotonic.empty();
}

bool standard_timer_manager::has_wait_timers() const {
  return p->waitfor > 0;
}

void standard_timer_manager::do_time_action(
    bool handle, pxtime::time_duration *left) {
  // Get current time
  pxtime::ptime now_deadline;
  if (!p->scheduled_deadline.empty())
    now_deadline = pxtime::microsec_clock::universal_time();
  pxtime::time_duration now_monotonic;
  if (!p->scheduled_monotonic.empty()) {
    now_monotonic = monotonic_clock();
    p->saved_monotonic_time = now_monotonic;
  }

  unsigned long no_handled = 0;
  // Handle timers
  if (handle) {
    { // handle deadline_timers
      while (!p->scheduled_deadline.empty()) {
        deadline_timer current = p->scheduled_deadline.top();

        if (current.time_left(now_deadline) > pxtime::seconds(0))
          break;

        if (current.must_wait())
          --p->waitfor;
        p->scheduled_deadline.pop();
        current.emit(*this);
        ++no_handled;
      }
    }
    { // handle monotonic_timers
      while (!p->scheduled_monotonic.empty()) {
        monotonic_timer current = p->scheduled_monotonic.top();

        if (current.time_left(now_monotonic) > pxtime::seconds(0))
          break;

        if (current.must_wait())
          --p->waitfor;
        p->scheduled_monotonic.pop();
        current.emit(*this);
        ++no_handled;
      }
    }
  }

  if (no_handled > 0) {
    if (!p->scheduled_deadline.empty())
      now_deadline = pxtime::microsec_clock::universal_time();
    if (!p->scheduled_monotonic.empty()) {
      now_monotonic = monotonic_clock();
      p->saved_monotonic_time = now_monotonic;
    }
  }

  // Get time left
  if (left) {
    bool has_deadline = !p->scheduled_deadline.empty();
    bool has_monotonic = !p->scheduled_monotonic.empty();
    if (has_deadline && has_monotonic)
      *left = std::min(
          p->scheduled_deadline.top().time_left(now_deadline),
          p->scheduled_monotonic.top().time_left(now_monotonic));
    else if (has_deadline)
      *left = p->scheduled_deadline.top().time_left(now_deadline);
    else if (!p->scheduled_monotonic.empty())
      *left = p->scheduled_monotonic.top().time_left(now_monotonic);
    else
      *left = pxtime::time_duration(pxtime::pos_infin);
    *left = std::max(*left, pxtime::time_duration(0,0,0,0));
  }
  
  p->saved_monotonic_time = boost::none;
}
