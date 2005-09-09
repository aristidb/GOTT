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
#include "iterator.hpp"
#ifndef NO_STDLIB
#include <algorithm>
#include <gott/util/range_algo.hpp>
#endif

using gott::string_buffer;
using gott::utf32_t;

class string_buffer::representation {
public:
  representation() : begin(0), end(0), storage_end(0) {}

  ~representation() { delete [] begin; }
  
  utf32_t *begin, *end, *storage_end;

  void ensure(std::size_t add_len) {
    std::size_t old_len = end - begin;
    if (std::size_t(storage_end - end) < add_len)
      grow(old_len + add_len);
    end = begin + old_len + add_len;
  }

private:
  void grow(std::size_t length) {
    std::size_t new_size = 1;
    while (new_size < length) 
      new_size <<= 1;
    utf32_t *old = begin;
    begin = new utf32_t[new_size];
    copy(range(old, end), begin);
    delete [] old;
    storage_end = begin + new_size;
  }
};

string_buffer::string_buffer()
: data(new representation) {}

string_buffer::string_buffer(string_buffer const &b) 
: data(new representation) {
  range_t<const_iterator> r = range(b);
  insert(end(), r.begin, r.end);
}

string_buffer::~string_buffer() { delete data; }

void string_buffer::swap(string_buffer &o) {
  std::swap(data, o.data);
}

void string_buffer::operator=(string_buffer const &b) {
  string_buffer(b).swap(*this);
}

string_buffer::string_buffer(range_t<utf32_t const *> const &x)
: data(new representation) {
  copy(x, insert(end(), x.size()));
}

string_buffer::string_buffer(string const &x)
: data(new representation) {
  copy(x.as_utf32(), insert(end(), x.length()));
}

utf32_t *string_buffer::begin() {
  return data->begin;
}

utf32_t *string_buffer::end() {
  return data->end;
}

utf32_t const *string_buffer::begin() const {
  return data->begin;
}

utf32_t const *string_buffer::end() const {
  return data->end;
}

std::size_t string_buffer::size() const {
  return data->end - data->begin;
}

utf32_t &string_buffer::operator[](std::size_t i) {
  return data->begin[i];
}

string_buffer::iterator string_buffer::erase(range_t<iterator> const &r) {
  iterator a = r.begin;
  iterator b = r.end;

  while (b != data->end)
    *a++ = *b++;
  data->end = a;
  return r.begin;
}

gott::range_t<string_buffer::iterator>
string_buffer::insert(iterator p, std::size_t len) {
  if (std::size_t(data->storage_end - data->end) < len) {
    std::size_t pp = p - data->begin;
    data->ensure(len);
    p = data->begin + pp;
  } else
    data->end += len;
  for (iterator it = data->end - 1; it >= p + len; --it)
    *it = *(it - len);
  return range(p, len);
}

gott::range_t<string_buffer::iterator> string_buffer::append(std::size_t len) {
  return insert(end(), len);
}

void string_buffer::operator+=(string const &s) {
  copy(s.as_utf32(), append(s.length()));
}
