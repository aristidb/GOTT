// Copyright (C) 2005 by Aristid Breitkreuz
// Content: GOTT Assertions
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

#ifndef GOTT_UTIL_DEBUG_ASSERT_HPP
#define GOTT_UTIL_DEBUG_ASSERT_HPP

#include <iostream>

namespace gott {

#include <boost/current_function.hpp>
#ifndef BOOST_CURRENT_FUNCTION
#define BOOST_CURRENT_FUNCTION "<unkown-func>"
#endif

namespace x__assert {
template<class T, class U>
void fail(int line, char const *file, char const *function, char const *lhs,
	  char const *rhs, char const *message, T const &lhs_v,
	  U const &rhs_v) {
  std::cerr << "Assertion failed in " << function << '@' << file << ':'
	    << line << "\nMessage: " << message << '\n';
  if (lhs)
    std::cout << lhs << ": " << lhs_v << '\n';
  if (rhs)
    std::cout << rhs << ": " << rhs_v << '\n';
  std::cout << std::flush;
}

template<class T, class F>
void assert_1(T const &value, F function, 
              int line, char const *file, char const *func_name,
              char const *var_name,
              char const *message) {
  if (!function(value))
    fail(line, file, func_name, var_name, 0, message, value, 0);
}

template<class T, class U, class F>
void assert_2(T const &lhs, U const &rhs, F function,
              int line, char const *file, char const *func_name,
              char const *lhs_name, char const *rhs_name,
              char const *message) {
  if (!function(lhs, rhs))
    fail(line, file, func_name, lhs_name, rhs_name, message, lhs, rhs);
}

}

struct equals {
  template<class T, class U>
  bool operator() (T const &a, U const &b) const {
    return a == b;
  }
};

// MM: Good ol' gcc 3.3/3.2 doesn't like the parens around f and prints a
// syntax error.
#if defined(__GNUC__) && __GNUC__ == 3 && __GNUC_MINOR__ < 4
  #define GOTT_ASSERT_1(x, f, m) \
    gott::x__assert::assert_1((x), f,__LINE__,__FILE__,BOOST_CURRENT_FUNCTION,#x,m)
  #define GOTT_ASSERT_2(x, y, f, m) \
    gott::x__assert::assert_2((x),(y), f,__LINE__,__FILE__,BOOST_CURRENT_FUNCTION,#x,#y,m)
#else
  #define GOTT_ASSERT_1(x, f, m) \
    gott::x__assert::assert_1((x),(f),__LINE__,__FILE__,BOOST_CURRENT_FUNCTION,#x,m)
  #define GOTT_ASSERT_2(x, y, f, m) \
    gott::x__assert::assert_2((x),(y),(f),__LINE__,__FILE__,BOOST_CURRENT_FUNCTION,#x,#y,m)
#endif  

}

#endif
