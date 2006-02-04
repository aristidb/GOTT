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
  bool has_wait_timers() const;

public:
  void handle_pending_timers();
  boost::posix_time::time_duration time_left() const;

private:
  class impl;
  boost::scoped_ptr<impl> p;
};

}}

#endif
