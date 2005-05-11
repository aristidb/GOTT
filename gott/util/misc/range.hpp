// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: C++ Range Abstractions
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

#ifndef GOTT_UTIL_MISC_RANGE_HPP
#define GOTT_UTIL_MISC_RANGE_HPP

#include <iterator>

namespace gott {
namespace util {

template<class T> struct range_t {
  typedef T value_type;
  T begin, end;
  range_t(T a, T b) : begin(a), end(b) {}
};

template<class T> 
range_t<T> range(T const &a, T const &b) {
  return range_t<T>(a, b);
}

template<class I, class S>
I advanced(I it, S s) {
  advance(it, s);
  return it;
}

template<class T, class S>
range_t<T> range(T const &a, S s) {
  return range(a, advanced(a, s));
}

template<class T, class S>
range_t<T> tail(T const &a, S s) {
  return range(advanced(a, -s), a);
}

template<class T> 
range_t<typename T::const_iterator> range(T const &c) {
  return range(c.begin(), c.end());
}

template<class T> 
range_t<typename T::iterator> range(T &c) {
  return range(c.begin(), c.end());
}

template<class T>
range_t<typename T::const_iterator> const_range(T const &c) {
  return range(c.begin(), c.end());
}

template<class T, class S>
range_t<T> offset(range_t<T> const &o, S a, S b = S()) {
  return range(advanced(o.begin, a), advanced(o.end, b));
}

}}

#endif
