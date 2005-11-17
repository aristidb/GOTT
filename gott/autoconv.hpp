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

inline std::ostream &operator<<(std::ostream &o, wchar_t const *in) {
  typedef std::codecvt<wchar_t, char, mbstate_t> CCV;
  CCV const &c = std::use_facet<CCV>(o.getloc());
  std::mbstate_t st = std::mbstate_t();
  std::size_t l = std::wcslen(in);
  std::string out(l / c.encoding(), '$');
  wchar_t const *in_n = 0;
  char *out_n = 0;
  c.out(st, in, in + l, in_n, &out[0], &out[0] + out.length(), out_n);
  return o << out;
}

#endif
