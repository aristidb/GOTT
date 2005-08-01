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

#include <cstdlib>
#include <cwchar>
#include <cstring>
#include <list>
#include <vector>
#include <ostream>

using gott::nstring;

class nstring::representation {
public:
  representation(utf8_t *d, std::size_t s) 
  : ref_count(1), size(s), length(0), data(d) {}

  std::size_t ref_count;
  std::size_t size;
  std::size_t length;
  utf8_t *data;
};

nstring::nstring(char const *in, encoding enc)
: p(new representation(to_utf8_alloc(in, enc), std::strlen(in))) {}

nstring::nstring(wchar_t const *in, encoding enc)
: p(new representation(to_utf8_alloc((char const*)in, enc), 
                          std::wcslen(in))) {}

nstring::nstring(nstring const &o) : p(o.p) {
  ++p->ref_count;
}

nstring::~nstring() {
  if (--p->ref_count == 0)
    delete p;
}

nstring::nstring(std::vector<nstring> const &v)
: p(new representation(0, 0)) {
  for (std::vector<nstring>::const_iterator it = v.begin(); it != v.end(); ++it)
    p->size += it->size();
  utf8_t *current = p->data = new utf8_t[p->size];
  for (std::vector<nstring>::const_iterator it = v.begin(); it != v.end(); ++it)
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
  if (!p->length && p->size) {
    utf8_iterator it = begin();
    while (it != end()) {
      ++it;
      ++p->length;
    }
  }
  return p->length;
}

void nstring::swap(nstring &o) {
  representation *tmp = p;
  p = o.p;
  o.p = tmp;
}

void nstring::operator=(nstring const &o) {
  nstring(o).swap(*this);
}

std::wostream &gott::operator<<(std::wostream &stream, nstring const &s) {
  for (utf8_iterator it = s.begin(); it != s.end(); ++it)
    stream << wchar_t(*it);
  return stream;
}
