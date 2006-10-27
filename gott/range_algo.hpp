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
 * The Original Code is A General-Purpose Iterator Range Class - Algorithms.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
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

#ifndef GOTT_UTIL_MISC_RANGE_ALGO_HPP
#define GOTT_UTIL_MISC_RANGE_ALGO_HPP

#include <gott/range.hpp>

#ifndef NO_STDLIB
#include <algorithm>
#endif

namespace gott {

#define GOTT_A_R1(A, R) \
  template<class I1> \
  R A(range_t<I1> const &r) { \
    return std::A(r.begin(), r.end()); \
  }

#define GOTT_A_R1_P1(A, R) \
  template<class I1, class I2> \
  R A(range_t<I1> const &r, I2 x) { \
    return std::A(r.begin(), r.end(), x); \
  }

#define GOTT_A_R1_P01(A, R) \
  GOTT_A_R1(A, R) \
  GOTT_A_R1_P1(A, R)

#define GOTT_A_R2(A, R) \
  template<class I1, class I2> \
  R A(range_t<I1> const &r1, range_t<I2> const &r2) { \
    return std::A(r1.begin(), r1.end(), r2.begin(), r2.end()); \
  }

#define GOTT_A_R2_P1(A, R) \
  template<class I1, class I2, class I3> \
  R A(range_t<I1> const &r1, range_t<I2> const &r2, I3 x) { \
    return std::A(r1.begin(), r1.end(), r2.begin(), r2.end(), x); \
  }

#define GOTT_A_R2_P01(A, R) \
  GOTT_A_R2(A, R) \
  GOTT_A_R2_P1(A, R)

#define GOTT_A_R2I(A, R) \
  template<class I1, class I2> \
  R A(range_t<I1> const &r1, range_t<I2> const &r2) { \
    return std::A(r1.begin(), r1.end(), r2.begin()); \
  } \
  GOTT_A_R1_P1(A, R)

#define GOTT_A_R2I_P1(A, R) \
  template<class I1, class I2, class I3> \
  R A(range_t<I1> const &r1, range_t<I2> const &r2, I3 x) { \
    return std::A(r1.begin(), r1.end(), r2.begin(), x); \
  }

#define GOTT_A_R2I_P01(A, R) \
  GOTT_A_R2I(A, R) \
  GOTT_A_R2I_P1(A, R)

#ifndef NO_STDLIB
GOTT_A_R1_P1(for_each, I2)
GOTT_A_R1_P1(find, I1)
GOTT_A_R1_P1(find_if, I1)
GOTT_A_R1_P01(adjacent_find, I1)
GOTT_A_R2_P01(find_first_of, I1)
GOTT_A_R1_P1(count, typename std::iterator_traits<I1>::difference_type)
GOTT_A_R1_P1(count_if, typename std::iterator_traits<I1>::difference_type)
GOTT_A_R2I_P01(equal, bool)
GOTT_A_R2_P01(search, I1)
// search_n
GOTT_A_R2_P01(find_end, I1)
GOTT_A_R2I(copy, I2)
// copy_n omitted
GOTT_A_R2I(copy_backward, I2)
GOTT_A_R2I(swap_ranges, I2)
GOTT_A_R2I_P1(transform, I2)
#endif //NO_STDLIB

#undef GOTT_A_R1
#undef GOTT_A_R1_P1
#undef GOTT_A_R1_P01
#undef GOTT_A_R2
#undef GOTT_A_R2_P1
#undef GOTT_A_R2_P01
#undef GOTT_A_R2I
#undef GOTT_A_R2I_P1
#undef GOTT_A_R2I_P01

}

#endif
