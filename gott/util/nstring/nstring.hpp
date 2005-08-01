// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Text storage and manipulation
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

#ifndef GOTT_UTIL_NSTRING_NSTRING_HPP
#define GOTT_UTIL_NSTRING_NSTRING_HPP

#include "iterator.hpp"
#include <gott/util/misc/visibility.hpp>

namespace std {
  template<class> class allocator;
  template<class> class char_traits;
  template<class, class> class vector;
  template<class, class> class list;
  template<class, class> class basic_ostream;
}

namespace gott {

class nstring_buffer;

class GOTT_EXPORT nstring {
public:
  nstring(char const *, encoding = utf8);
  nstring(wchar_t const *, encoding = utf32);
  nstring(nstring_buffer const &);
  nstring(std::vector<nstring, std::allocator<nstring> > const &);
  nstring(std::list<nstring, std::allocator<nstring> > const &);
  nstring(nstring const *, std::size_t);

  nstring(nstring const &);
  ~nstring();

  void swap(nstring &other);
  void operator=(nstring const &other);

  char const *c_str(encoding enc) const;
  utf8_t const *data() const;
 
  utf8_iterator begin() const;
  utf8_iterator end() const;

  std::size_t length() const;

  std::size_t size() const;

private:
  class representation;
  representation *p;

  GOTT_LOCAL nstring(); // unimplemented
};

GOTT_EXPORT 
std::basic_ostream<wchar_t, std::char_traits<wchar_t> > &
operator<<(std::basic_ostream<wchar_t, std::char_traits<wchar_t> > &, 
           nstring const &);

GOTT_EXPORT nstring operator+(nstring const &, nstring const &);

}

#endif
