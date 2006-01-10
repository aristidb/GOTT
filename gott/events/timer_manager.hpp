// Copyright (C) 2005-2006 by Aristid Breitkreuz (aribrei@arcor.de)
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

#ifndef GOTT_BASE_EVENTS_TIMER_MANAGER_HPP
#define GOTT_BASE_EVENTS_TIMER_MANAGER_HPP

#include "deadline_timer.hpp"
#include <gott/string/qid.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace gott {
namespace events {

/**
 * Feature for main_loops able to deal with timer events.
 */
class GOTT_EXPORT timer_manager {
public:
  /// Constructor.
  timer_manager();
  /// Pure virtual destructor.
  virtual ~timer_manager() = 0;
  
  static QID const qid;
  
  /// Add a deadline_timer.
  virtual void add_timer(deadline_timer const &) = 0;

  /// Check if there are timers to be scheduled sometime.
  virtual bool has_timers() const = 0;
};

class standard_timer_manager : public timer_manager {
public:
  standard_timer_manager();
  ~standard_timer_manager();

  void add_timer(deadline_timer const &);
  bool has_timers() const;

public:
  void handle_pending_timers();
  boost::posix_time::time_duration time_left() const;

private:
  class impl;
  boost::scoped_ptr<impl> p;
};

}}

#endif
