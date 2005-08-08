// GOTT utilities
//
// Copyright (C) 2005 Aristid Breitkreuz
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) Xany later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef GOTT_UTIL_MISC_AUTOCONV_HPP
#define GOTT_UTIL_MISC_AUTOCONV_HPP

#include <ostream>
#include <string>
#include <gott/util/range.hpp>

namespace std {

// "Fix" basic_ostream::operator<<(void*)
template<class C, class CT>
void operator<<(basic_ostream<C, CT> &, void *) {}

struct print_ptr {
  void *data;
  print_ptr(void *p) : data(p) {}
};

template<class C, class CT>
basic_ostream<C, CT> &operator<<(basic_ostream<C, CT> &s, print_ptr const &p) {
  return s.operator<<(p.data);
}

inline ostream &operator<<(ostream &o, wchar_t const *in) {
  typedef codecvt<wchar_t, char, mbstate_t> CCV;
  CCV const &c = use_facet<CCV>(o.getloc());
  mbstate_t st = mbstate_t();
  size_t l = wcslen(in);
  string out(l / c.encoding(), '$');
  wchar_t const *in_n = 0;
  char *out_n = 0;
  c.out(st, in, in + l, in_n, &out[0], &out[0] + out.length(), out_n);
  return o << out;
}

inline ostream &operator<<(ostream &o, wstring const &x) {
  return o << x.c_str();
}

inline wostream &operator<<(wostream &o, string const &x) {
  return o << x.c_str();
}

template<class T, class C>
struct element_separator {
};

template<class T>
struct element_separator<T, char> {
  static char const *sep() { return " "; }
};

template<class T>
struct element_separator<T, wchar_t> {
  static wchar_t const *sep() { return L" "; }
};

template<> struct element_separator<char, char> {
  static char const *sep() { return ""; }
};

template<> struct element_separator<wchar_t, char> {
  static char const *sep() { return ""; }
};

template<> struct element_separator<char, wchar_t> {
  static wchar_t const *sep() { return L""; }
};

template<> struct element_separator<wchar_t, wchar_t> {
  static wchar_t const *sep() { return L""; }
};

template<class Ch, class CT, class T>
void print_separated(basic_ostream<Ch,CT> &s, gott::range_t<T> const &x,
                     Ch const *sep) {
  T i = x.begin;
  s << *i++;
  for (; i != x.end; ++i)
    s << sep << *i;
}

template<class Ch, class CT, class T>
basic_ostream<Ch,CT> &operator<<(basic_ostream<Ch,CT> &s, 
                                 gott::range_t<T> const &x) {
  print_separated(s, x, 
      element_separator<typename iterator_traits<T>::value_type, Ch>::sep());
  return s;
}

}

#endif