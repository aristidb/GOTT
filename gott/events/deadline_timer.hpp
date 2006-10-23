// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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
 * Andreas Pokorny (andreas.pokorny@gmail.com)
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

#include "basic_timer.hpp"
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace gott { namespace events {

class deadline_timer : public basic_timer {
public:
  deadline_timer(
      boost::posix_time::ptime const &time_point,
      callback_type const &callback,
      bool wait)
    : basic_timer(callback, wait), time_point(time_point) {}

  bool operator>(deadline_timer const &o) const {
    return time_point > o.time_point;
  }

  boost::posix_time::time_duration time_left(
      boost::posix_time::ptime now =
        boost::posix_time::microsec_clock::local_time()) const {
    return time_point - now;
  }

private:
  boost::posix_time::ptime time_point;
};

}}

#endif

