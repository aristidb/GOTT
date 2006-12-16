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
 * The Original Code is A General-Purpose Iterator Range Class.
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

#ifndef GOTT_UTIL_MISC_RANGE_HPP
#define GOTT_UTIL_MISC_RANGE_HPP

#ifndef NO_STDLIB
#include <iterator>
#include <iosfwd>
#endif

namespace gott {

/**
 * \name Range tools
 * Functions and classes for dealing with iterator ranges.
 * \{
 */

/**
 * Simple iterator-based range class.
 */
template<class T> struct range_t {
  typedef T value_type;

  typedef T iterator;
  typedef T const_iterator;
  
  /**
   * Iterator to start of the range.
   */
  value_type Begin;

  /**
   * Iterator to after the end of the range.
   */
  value_type End;

  /**
   * Construct range with null-iterators. This is a defined state.
   */
  range_t() : Begin(value_type()), End(value_type()) {}

  /**
   * Construct range from two iterators.
   */
  range_t(value_type const &a, value_type const &b) : Begin(a), End(b) {}

  /**
   * Construct range from range of implicitly convertible type.
   */
  template<class U>
  range_t(range_t<U> const &o) : Begin(o.Begin), End(o.End) {}

  /**
   * Cast range to range of another type.
   */
  template<class U>
  range_t<U> cast() const {
    return range_t<U>(U(Begin), U(End));
  }

  template<class Result, class U> 
  range_t<Result> call(U method) const {
    return range_t<Result>((Begin.*method)(), (End.*method)());
  }

  value_type begin() const {
    return Begin;
  }

  value_type &begin() {
    return Begin;
  }

  value_type end() const {
    return End;
  }

  value_type &end() {
    return End;
  }

  /**
   * Return the size of the range.
   */
  std::size_t size() const {
#ifndef NO_STDLIB
    return std::distance(Begin, End);
#else
    return End - Begin;
#endif
  }

  /**
   * Check if the range is empty. Check if you are rather interested in
   * filled().
   */
  bool empty() const {
    return begin() == end();
  }

  /**
   * Check if the range contains data if used with data.begin < data.end
   * conventions. This is not the opposite of empty().
   */
  bool filled() const {
    return begin() < end();
  }
};


/**
 * Compare two ranges for element-by-element equality.
 */
template<class T, class U>
bool operator==(range_t<T> a, range_t<U> b) {
  if (a.begin() == b.begin())
    return a.end() == b.end();
  if (a.size() != b.size())
    return false;
  for (; a.Begin != a.End; ++a.Begin, ++b.Begin)
    if (*a.Begin != *b.Begin)
      return false;
  return true;
}

/**
 * Compare two ranges for element-by-element inequality.
 */
template<class T, class U>
bool operator!=(range_t<T> const &a, range_t<U> const &b) {
  return !(a == b);
}

#ifndef NO_STDLIB
template<class Ch, class ChT, class T>
std::basic_ostream<Ch, ChT> &
operator<<(std::basic_ostream<Ch, ChT> &s, range_t<T> const &r) {
  T i = r.Begin;
  s << *i++;
  for (; i != r.End; ++i)
    s << ' ' << *i;
  return s;
}
#endif

/**
 * Build range from two iterators.
 */
template<class T> 
range_t<T> range(T const &a, T const &b) {
  return range_t<T>(a, b);
}

/**
 * Return (logically) @p it + @p s.
 */
template<class I, class S>
I advanced(I it, S s) {
#ifndef NO_STDLIB
  std::advance(it, s);
  return it;
#else
  return it + s;
#endif
}

/**
 * Build a range from iterator and size.
 */
template<class T, class S>
range_t<T> range(T const &a, S s) {
  return range(a, advanced(a, s));
}

/**
 * Build a range from iterator to the end and size backwards.
 */
template<class T, class S>
range_t<T> tail(T const &a, S s) {
  return range(advanced(a, -s), a);
}

/**
 * Build a range from a constant array.
 */
template<class T, std::size_t N>
range_t<T const *> range(T const (&arr)[N]) {
  return range(static_cast<T const *>(arr), N);
}

/**
 * Build a range from an array.
 */
template<class T, std::size_t N>
range_t<T *> range(T (&arr)[N]) {
  return range(static_cast<T *>(arr), N);
}

/**
 * Build a range from a (STL-style) constant container.
 */
template<class T> 
range_t<typename T::const_iterator> range(T const &c) {
  return range(c.begin(), c.end());
}

/**
 * Build a range from a (STL-style) container.
 */
template<class T> 
range_t<typename T::iterator> range(T &c) {
  return range(c.begin(), c.end());
}

/**
 * Build a constant-value range from a (STL-style) container.
 */
template<class T>
range_t<typename T::const_iterator> const_range(T const &c) {
  return range(c.begin(), c.end());
}

/**
 * Build a range from another range and adjust edges.
 */
template<class T, class S>
range_t<T> offset(range_t<T> const &o, S a, S b = S()) {
  return range(advanced(o.begin(), a), advanced(o.end(), b));
}

/**
 * Return a range containing simply one iterator.
 * Warning: Neither range_t::size nor any code relying on range_t::end will 
 *          probably function.
 */
template<class T>
range_t<T> simply(T const &x) {
  return range(x, x);
}

/**
 * Return a range from a value-terminated sequence.
 */
template<class I, class T>
range_t<I> value_terminated(I start, T val) {
  I end = start;
  while (*end != val)
    ++end;
  return range(start, end);
}

/**
 * Return a range from a zero-terminated sequence.
 */
template<class I>
range_t<I> zero_terminated(I start) {
  return value_terminated(start, 0);
}

// \}

}

#endif
