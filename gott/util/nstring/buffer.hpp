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

#ifndef GOTT_UTIL_NSTRING_BUFFER_HPP
#define GOTT_UTIL_NSTRING_BUFFER_HPP

#include "nstring.hpp"
#include "types.hpp"

namespace gott {

template<class> class range_t;

class GOTT_EXPORT nstring_buffer {
public:
  typedef utf32_t value_type;
  typedef utf32_t *iterator;
  typedef utf32_t const *const_iterator;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  
  nstring_buffer();
  nstring_buffer(nstring_buffer const &);
  nstring_buffer(nstring const &);
  nstring_buffer(utf32_t const *);
  ~nstring_buffer();

  utf32_t *begin();
  utf32_t *end();
  utf32_t const *begin() const;
  utf32_t const *end() const;

  size_t size() const;
  
  void swap(nstring_buffer &);

  void operator=(nstring_buffer const &b);

  utf32_t &operator[](std::size_t);
  utf32_t &at(std::size_t);
  
  iterator erase(iterator, iterator);

  iterator insert(iterator, std::size_t);
  iterator insert(iterator, std::size_t, utf32_t);

  template<class I>
  iterator insert(iterator o, range_t<I> const &r) {
    o = insert(o, std::distance(r.begin, r.end));
    std::copy(r.begin, r.end, o);
    return o;
  }

private:
  class representation;
  representation *data;
};

}

#endif
