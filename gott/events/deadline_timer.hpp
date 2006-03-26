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
 * Andreas Pokorny (andreas.pokorny@gmail.com )
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Andreas Pokorny (andreas.pokorny@gmail.com)
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

#ifndef GOTT_BASE_EVENTS_DEADLINE_TIMER_HPP_INCLUDED
#define GOTT_BASE_EVENTS_DEADLINE_TIMER_HPP_INCLUDED

#include <gott/visibility.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/function.hpp>

namespace gott{namespace events{

//TODO -> -> ->

/*|* (deactivated doxygen)
 * \brief timed event structure. 
 * similar to http://asio.sourceforge.net/asio-0.3.4/doc/reference/a00114.html
 *
 * The deadline_timer carries a boost::date_time::posix_time::ptime object and a function object
 * returing a deadline_timer. An event loop will try to call the handler when the specified 
 * time is reached. The handler may return another valid deadline_timer to the event_loop. 
 * 
 * \paragraph example code
 * In this example do_something will be called every 10 seconds ( + time to execute do_something ).
 * \code
 *   using namespace gott::events;
 *   using namespace boost::posix_time;
 *   deadline_timer do_something() {
 *      // do something 
 *      return deadline_timer( microsec_clock::local_time() + seconds(10), &do_something );
 *   }
 *   ...
 *
 *   {
 *     select_loop loop;
 *     loop.add_timer( deadline_timer( microsec_clock::local_time() + seconds(10), &do_something ) );
 *     loop.run();
 *   }
 * \endcode
 *
 * To end the execution after 20 executions, do_something would look like that.
 * \code
 *   deadline_timer do_something() {
 *      // do something 
 *     static int exec = 0;
 *     if( ++exec == 20 )
 *       return deadline_timer::no_timer;
 *     else
 *       return deadline_timer( microsec_clock::local_time() + seconds(10), &do_something );
 *   }
 * \endcode
 */
struct GOTT_LOCAL deadline_timer{
  typedef boost::posix_time::ptime time_type;
  typedef boost::function<deadline_timer ()> handler_type;
  time_type timer;
  handler_type handler;
  bool wait;

  /**
   * \brief constructs an invalid deadline_timer.
   */
  deadline_timer() : timer(boost::date_time::not_a_date_time) {}

  /**
   * Construct a deadline_timer.
   * \param t The time when the timer is triggered.
   * \param h The timer's callback.
   * \param w Whether the timer lets main_loop::run wait.
   */
  deadline_timer( time_type const& t, handler_type const&  h, bool w = true) 
    : timer(t),handler(h),wait(w){}

  static const deadline_timer no_timer;
};

namespace detail {
  using namespace boost::posix_time;
  struct periodic_timeval_functor {
    boost::function<void ()> delegate;
    time_duration interval;
    bool wait;
    deadline_timer operator() () const {
      delegate();
      return deadline_timer(microsec_clock::local_time() + interval, 
          deadline_timer::handler_type(*this), wait);
    }
  };
}

/**
 * Construct a periodic timer.
 * \param interval The time between triggers.
 * \param delegate The callback.
 * \param start_now Does not wait for interval to pass if set.
 * \param wait Whether the timer lets main_loop::run wait.
 */
inline deadline_timer periodic_timer(boost::posix_time::time_duration interval,
      boost::function<void ()> const &delegate,
      bool start_now = true, bool wait = true) {
  using namespace boost::posix_time;
  detail::periodic_timeval_functor func = { delegate, interval, wait };
  return deadline_timer(
      microsec_clock::local_time() + (start_now ? seconds(0) : interval),
      deadline_timer::handler_type(func), wait);
}

inline GOTT_LOCAL 
bool operator<( deadline_timer const& left, deadline_timer const& right ) { 
  return left.timer < right.timer; 
} 
inline GOTT_LOCAL 
bool operator>( deadline_timer const& left, deadline_timer const& right ) { 
  return left.timer > right.timer; 
} 

}}

#endif

