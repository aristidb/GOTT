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

#include "nstring.hpp"
#include "convert.hpp"
#include "util.hpp"
#include "buffer.hpp"
#include "iterator.hpp"

#include <gott/util/range.hpp>
#include <gott/util/range_algo.hpp>
#include <cstring>
#include <list>
#include <vector>
#include <ostream>

using gott::nstring;

class nstring::representation {
public:
  representation(utf8_t const *d, bool o = true) 
  : ref_count(1), size(utf8_len(d)), length(0), owned(o), data(d) {}

  representation(range_t<utf8_t const *> d, bool o = true)
  : ref_count(1), size(d.size()), length(0), owned(o), data(d.begin) {}

  enum foreign_tag { foreign_copy };

  representation(range_t<utf8_t const *> const &x, foreign_tag)
  : ref_count(1), size(x.size()), length(0), owned(true),
      data(new utf8_t[x.size()]) {
    copy(x, const_cast<utf8_t *>(data));
  }

  ~representation() {
    if (owned) 
      delete data;
  }

  enum concatenation_tag { concat };

  template<class I> representation(range_t<I> const &, concatenation_tag);

  std::size_t ref_count;
  std::size_t size;
  std::size_t length;
  bool owned;
  utf8_t const *data;
};

nstring::nstring(nstring const &o) : p(o.p) {
  ++p->ref_count;
}

nstring::~nstring() {
  if (--p->ref_count == 0)
    delete p;
}

nstring::nstring(range_t<char const *> in, encoding enc)
: p(new representation(to_utf8_alloc(in, enc))) {}

nstring::nstring(range_t<utf8_t const *> in, literal_tag)
: p(new representation(in, false)) {}

nstring::nstring(range_t<wchar_t const *> in, encoding enc)
: p(new representation(to_utf8_alloc(in.cast<char const *>(), enc))) {}

nstring::nstring(nstring_buffer const &b)
: p(new representation(to_utf8_alloc(range(b).cast<char const *>(), utf32))) {}

nstring::nstring(range_t<utf8_t const *> const &r)
: p(new representation(r, representation::foreign_copy)) {}

nstring::nstring(range_t<utf8_iterator> const &r)
: p(new representation(r, representation::foreign_copy)) {}

nstring::nstring(std::vector<nstring> const &v)
: p(new representation(range(v), representation::concat)) {}

nstring::nstring(std::list<nstring> const &v)
: p(new representation(range(v), representation::concat)) {}

nstring::nstring(range_t<nstring const *> cont)
: p(new representation(cont, representation::concat)) {}

template<class I>
nstring::representation::representation(range_t<I> const &r, concatenation_tag) 
: ref_count(1), size(0), length(0), owned(true) {
  for (I it = r.begin; it != r.end; ++it)
    size += it->size();
  utf8_t *current = new utf8_t[size];
  data = current;
  for (I it = r.begin; it != r.end; ++it)
    current = std::copy(it->data(), it->data() + it->size(), current);
}

gott::utf8_t const *nstring::data() const {
  return p->data;
}

gott::utf8_t const *nstring::begin_raw() const {
  return p->data;
}

gott::utf8_t const *nstring::end_raw() const {
  return p->data + p->size;
}

gott::utf8_iterator nstring::begin() const {
  return begin_raw();
}

gott::utf8_iterator nstring::end() const {
  return end_raw();
}

std::size_t nstring::size() const {
  return p->size;
}

std::size_t nstring::length() const {
  if (!p->length)
    p->length = range(*this).size();
  return p->length;
}

void nstring::swap(nstring &o) {
  std::swap(p, o.p);
}

void nstring::operator=(nstring const &o) {
  nstring(o).swap(*this);
}

std::wostream &gott::operator<<(std::wostream &stream, nstring const &s) {
  for (utf8_iterator it = s.begin(); it != s.end(); ++it)
    stream << wchar_t(*it);
  return stream;
}

nstring gott::operator+(nstring const &a, nstring const &b) {
  nstring const both[] = { a, b };
  return nstring(range(both));
}

bool gott::operator==(nstring const &a, nstring const &b) {
  if (a.data() == b.data())
    return true;
  if (a.size() != b.size())
    return false;
  return std::equal(a.begin_raw(), a.end_raw(), b.begin_raw());
}

int gott::compare(nstring const &a, nstring const &b) {
  if (a.data() == b.data())
    return 0;

  utf8_t const *p1 = a.begin_raw();
  utf8_t const *p2 = b.begin_raw();
  for (; p1 != a.end_raw() && p2 != b.end_raw(); ++p1, ++p2)
    if (*p1 != *p2)
      break;
  if (p1 == a.end_raw()) {
    if (p2 == b.end_raw())
      return 0;
    return 1;
  } else if (p2 == b.end_raw())
    return -1;
  
  return *p2 - *p1;
}

std::size_t 
hashd::hash<gott::nstring>::operator() (gott::nstring const &s) const {
  std::size_t result = 0;
  for (gott::utf8_t const *it = s.begin_raw(); it != s.end_raw(); ++it)
    result = 5 * result + *it;
  return result;
}
