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
 * The Original Code is Some Assertion Macros and Classes.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005
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
