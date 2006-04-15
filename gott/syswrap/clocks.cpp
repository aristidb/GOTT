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
 * The Original Code is Low-level System Wrapping Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#include "clocks.hpp"
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
 
using namespace boost::posix_time;
 
static time_duration fallback_monotonic_clock() { // TODO: FIXME, make monotonic
  // TODO: emit some warning
  struct timeval tp;
  ::gettimeofday(&tp, 0);
  return time_duration(0, 0,
      tp.tv_sec,
      tp.tv_usec / (1000000 / time_duration::ticks_per_second()));
}
 
time_duration gott::monotonic_clock() {
#if defined(_POSIX_TIMERS) && _POSIX_TIMERS>0
  struct timespec tp;
  if (::clock_gettime(CLOCK_MONOTONIC, &tp) == -1)
    return fallback_monotonic_clock();
  return time_duration(0, 0,
      tp.tv_sec,
      tp.tv_nsec / (1000000000 / time_duration::ticks_per_second()));
#else
  #warning "Monotonic clocks are not supported on your system."
  return fallback_monotonic_clock();
#endif
}
