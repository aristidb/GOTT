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

#include <gott/util/misc/range.hpp>
#include <gott/util/misc/range_algo.hpp>
#include <cstdlib>
#include <cwchar>
#include <cstring>
#include <list>
#include <vector>
#include <ostream>
#include <boost/bind.hpp>

using gott::nstring;

class nstring::representation {
public:
  representation(utf8_t const *d, bool o = true) 
  : ref_count(1), size(utf8_len(d)), length(0), owned(o), data(d) {}

  representation(range_t<utf8_t *> d, bool o = true)
  : ref_count(1), size(d.end - d.begin), length(0), owned(o), data(d.begin) {}

  ~representation() {
    if (owned) 
      delete data;
  }

  template<class I> representation(range_t<I> const &);

  std::size_t ref_count;
  std::size_t size;
  std::size_t length;
  bool owned;
  utf8_t const *data;
};

nstring::nstring(char const *in, encoding enc)
: p(new representation(to_utf8_alloc(in, in + std::strlen(in), enc))) {}

nstring::nstring(utf8_t const *in, literal_tag)
: p(new representation(in, false)) {}

nstring::nstring(wchar_t const *in, encoding enc)
: p(new representation(to_utf8_alloc((char const*)in, 
                                     (char const*)(in+std::wcslen(in)),
                                     enc))) {}

nstring::nstring(nstring const &o) : p(o.p) {
  ++p->ref_count;
}

nstring::nstring(nstring_buffer const &b)
: p(new representation(to_utf8_alloc((char const*)b.begin(), 
                                     (char const*)b.end(), 
                                     utf32))) {}

nstring::~nstring() {
  if (--p->ref_count == 0)
    delete p;
}

nstring::nstring(std::vector<nstring> const &v)
: p(new representation(range(v))) {}

nstring::nstring(std::list<nstring> const &v)
: p(new representation(range(v))) {}

nstring::nstring(nstring const *arr, std::size_t len)
: p(new representation(range(arr, len))) {}

template<class I>
nstring::representation::representation(range_t<I> const &r) 
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

gott::utf8_iterator nstring::begin() const {
  return p->data;
}

gott::utf8_iterator nstring::end() const {
  return p->data + p->size;
}

std::size_t nstring::size() const {
  return p->size;
}

std::size_t nstring::length() const {
  if (!p->length)
    for (utf8_iterator it = begin(); it != end(); ++it)
      ++p->length;
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
  nstring both[] = { a, b };
  return nstring(both, 2);
}

bool gott::operator==(nstring const &a, nstring const &b) {
  if (a.p == b.p)
    return true;
  if (a.size() != b.size())
    return false;
  return equal(range(a), range(b));
}

bool gott::operator!=(nstring const &a, nstring const &b) {
  return !(a == b);
}
