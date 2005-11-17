// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: C++ Range Abstractions
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
    return std::A(r.begin, r.end); \
  }

#define GOTT_A_R1_P1(A, R) \
  template<class I1, class I2> \
  R A(range_t<I1> const &r, I2 x) { \
    return std::A(r.begin, r.end, x); \
  }

#define GOTT_A_R1_P01(A, R) \
  GOTT_A_R1(A, R) \
  GOTT_A_R1_P1(A, R)

#define GOTT_A_R2(A, R) \
  template<class I1, class I2> \
  R A(range_t<I1> const &r1, range_t<I2> const &r2) { \
    return std::A(r1.begin, r1.end, r2.begin, r2.end); \
  }

#define GOTT_A_R2_P1(A, R) \
  template<class I1, class I2, class I3> \
  R A(range_t<I1> const &r1, range_t<I2> const &r2, I3 x) { \
    return std::A(r1.begin, r1.end, r2.begin, r2.end, x); \
  }

#define GOTT_A_R2_P01(A, R) \
  GOTT_A_R2(A, R) \
  GOTT_A_R2_P1(A, R)

#define GOTT_A_R2I(A, R) \
  template<class I1, class I2> \
  R A(range_t<I1> const &r1, range_t<I2> const &r2) { \
    return std::A(r1.begin, r1.end, r2.begin); \
  } \
  GOTT_A_R1_P1(A, R)

#define GOTT_A_R2I_P1(A, R) \
  template<class I1, class I2, class I3> \
  R A(range_t<I1> const &r1, range_t<I2> const &r2, I3 x) { \
    return std::A(r1.begin, r1.end, r2.begin, x); \
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
