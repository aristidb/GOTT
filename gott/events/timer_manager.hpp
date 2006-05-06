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

#ifndef GOTT_BASE_EVENTS_TIMER_MANAGER_HPP
#define GOTT_BASE_EVENTS_TIMER_MANAGER_HPP

#include <gott/string/qid.hpp>
#include <gott/syswrap/clocks.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/function.hpp>
#include <vector>

namespace gott {
namespace events {

class main_loop;
class deadline_timer;
class monotonic_timer;

/**
 * Feature for main_loops able to deal with timer events.
 */
class GOTT_EXPORT timer_manager : boost::noncopyable {
public:
  /// Constructor.
  timer_manager();
  /// Pure virtual destructor.
  virtual ~timer_manager() = 0;
  
  static QID const qid;

  /**
   * Add a deadline timer (UTC based).
   * \param time_point The time when the timer should wake, in UTC.
   * \param callback The callback to call when the timer is woken. Gets this
   *                 object as parameter if it wants to add new timers.
   * \param wait Whether the main_loop should wait for the timer to be removed
   *             before quitting.
   */
  void add_deadline_timer(
      boost::posix_time::ptime const &time_point,
      boost::function<void (timer_manager &)> const &callback,
      bool wait = true) ;

  /**
   * Add a deadline_timer (UTC based).
   * \param tm The timer to add.
   */
  virtual void add_timer(deadline_timer const &tm) = 0;

  /**
   * Add a monotonic timer. The clock behind this timer should not do any jump
   * backwards if the system supports it.
   * \param time_point The time when the timer should wake in monotonic time.
   * \param callback The callback to call when the timer is woken. Gets this
   *                 object as parameter if it wants to add new timers.
   * \param wait Whether the main_loop should wait for the timer to be removed
   *             before quitting.
   * \see gott::monotonic_clock()
   */
  void add_monotonic_timer(
      boost::posix_time::time_duration const &time_point,
      boost::function<void (timer_manager &)> const &callback,
      bool wait = true);

  /**
   * Add a monotonic_timer. The clock behind this timer should not do any jump
   * backwards if the system supports it.
   * \param tm The timer to add.
   * \see gott::monotonic_clock()
   */
  virtual void add_timer(monotonic_timer const &tm) = 0;

  /**
   * Add a relative timer.
   * \param interval The amount of time from now when the timer should wake.
   * \param callback The callback to call when the timer is woken. Gets this
   *                 object as parameter if it wants to add new timers.
   * \param wait Whether the main_loop should wait for the timer to be removed
   *             before quitting.
   */
  void add_relative_timer(
      boost::posix_time::time_duration const &interval,
      boost::function<void (timer_manager &)> const &callback,
      bool wait = true);
  
  /**
   * Add a periodic timer.
   * \param interval The minimum and avaerage amount of time between wakes.
   * \param callback The callback to call whenever the timer is woken. Return
   *            false when the timer should end.
   * \param wait Whether the main_loop should wait for the timer to end (by
   *            returning false) before quitting.
   */
  void add_periodic_timer(
      boost::posix_time::time_duration const &interval,
      boost::function<bool ()> const &callback,
      bool wait = true);

  /**
   * Add a periodic timer that instead of being called multiple times at once,
   * gives an overdraw counter to the callback. This has the advantage of 
   * wasting less resources than add_periodic_timer(). The overdraw counter
   * specifies the number of times the timer should have been woken but has not
   * been, so it is always 0 if the timer is never late.
   * \param interval The minimum amount of time between wakes.
   * \param callback The callback to call whenever the timer is woken. Return
   *            false when the timer should end.
   * \param wait Whether the main_loop should wait for the timer to end (by
   *            return false) before quitting.
   */
  void add_periodic_timer_overdraw(
      boost::posix_time::time_duration const &interval,
      boost::function<bool (unsigned overdraw)> const &callback,
      bool wait = true);

protected:
  virtual boost::posix_time::time_duration monotonic_now() const;
  virtual boost::posix_time::ptime utc_now() const;

public:
  static timer_manager *get_for(main_loop &) GOTT_LOCAL { return 0; }

private:
  struct periodic_helper;
};

class GOTT_EXPORT standard_timer_manager : public timer_manager {
public:
  standard_timer_manager(main_loop *);
  ~standard_timer_manager();

private:
  void add_timer(deadline_timer const &);
  void add_timer(monotonic_timer const &);

  void add_relative_timer(
      boost::posix_time::time_duration const &interval,
      boost::function<void (timer_manager &)> const &callback,
      bool wait = true);

public:
  bool has_timers() const;

public:
  void handle_pending_timers() GOTT_LOCAL {
    do_time_action(true, 0);
  }
  
  boost::posix_time::time_duration time_left(bool handle = false) GOTT_LOCAL {
    boost::posix_time::time_duration result;
    do_time_action(handle, &result);
    return result;
  }

private:
  void do_time_action(bool handle, boost::posix_time::time_duration *left);

private:
  boost::posix_time::time_duration monotonic_now() const;
  boost::posix_time::ptime utc_now() const;

private:
  class impl;
  boost::scoped_ptr<impl> p;
};

}}

#endif
