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

#include <gott/util/misc/commonheaders.hpp>

namespace std {

inline ostream &operator<<(ostream &o, wchar_t const *in) {
  typedef codecvt<wchar_t, char, mbstate_t> CCV;
  CCV const &c = use_facet<CCV>(std::locale());
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

template<class Ch, class CT, class T>
basic_ostream<Ch,CT> &operator<<(basic_ostream<Ch,CT> &s, 
                                 gott::util::range_t<T> const &x) {
  typedef typename iterator_traits<T>::value_type value_type;
  copy(x.begin, x.end, 
       ostream_iterator<value_type>(s, 
                                    element_separator<value_type, Ch>::sep()));
  return s;
}

}

#endif
