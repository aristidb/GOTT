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

#include "buffer.hpp"
#include "convert.hpp"
#include "util.hpp"
#include <algorithm>

using gott::nstring_buffer;
using gott::utf32_t;

class nstring_buffer::representation {
public:
  representation() : begin(0), end(0), storage_end(0) {}
  utf32_t *begin, *end, *storage_end;

  utf32_t *ensure(std::size_t add_len) {
    if (std::size_t(storage_end - end) < add_len)
      grow(end - begin + add_len);
    utf32_t *old_end = end;
    end += add_len;
    return old_end;
  }
  
  void grow(std::size_t length) {
    std::size_t new_size = 1, tmp = length;
    while (tmp) {
      new_size <<= 1;
      tmp >>= 1;
    }
    begin = new utf32_t[new_size];
    end = begin + length;
    storage_end = begin + new_size;
  }
};

nstring_buffer::nstring_buffer()
: data(new representation) {}

nstring_buffer::~nstring_buffer() { delete data; }

utf32_t *nstring_buffer::begin() {
  return data->begin;
}

utf32_t *nstring_buffer::end() {
  return data->end;
}

utf32_t const *nstring_buffer::begin() const {
  return data->begin;
}

utf32_t const *nstring_buffer::end() const {
  return data->begin;
}

void nstring_buffer::operator+=(utf32_t const *x) {
  append(x, utf32len(x));
}

void nstring_buffer::operator+=(nstring const &x) {
  std::copy(x.begin(), x.end(), data->ensure(x.length()));
}

void nstring_buffer::append(utf32_t const *x, std::size_t add_len) {
  std::copy(x, x + add_len, data->ensure(add_len));
}
