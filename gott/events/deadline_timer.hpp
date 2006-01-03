#ifndef GOTT_BASE_EVENTS_DEADLINE_TIMER_HPP_INCLUDED
#define GOTT_BASE_EVENTS_DEADLINE_TIMER_HPP_INCLUDED

// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT select loop
// Authors: Andreas Pokorny
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


#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/function.hpp>
namespace gott{namespace events{

/**
 * \brief timed event structure. similar to http://asio.sourceforge.net/asio-0.3.4/doc/reference/a00114.html
 *
 * The deadline_timer carries a boost::date_time::posix_time::ptime object and a function object
 * returing a deadline_timer. An event loop will try to call the handler when the specified 
 * time is reached. The handler may return another valid deadline_timer to the event_loop. 
 * 
 * \paragraph example code
 * In this example do_something will be called every 10 seconds ( + time to execute do_something ).
 * \code
 *   using namespace gott::events;
 *   using namespace boost::date_time::posix_time;
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

  /**
   * \brief constructs an invalid deadline_timer.
   */
  deadline_timer() : timer(boost::date_time::not_a_date_time) {}
  deadline_timer( time_type const& t, handler_type const&  h) 
    : timer(t),handler(h){}

  static const deadline_timer no_timer;
};

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

