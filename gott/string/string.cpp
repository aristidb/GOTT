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

#include "string.hpp"
#include "iterator.hpp"
#ifndef NO_STDLIB
#include "stl.hpp"
#endif

#include <gott/range.hpp>
#include <gott/range_algo.hpp>
#include <gott/thunk.hpp>
#ifndef NO_STDLIB
#include <list>
#include <vector>
#include <ostream>
#include <string>
#endif

using gott::string;
using gott::range_t;

#ifdef DEBUG
#include <fstream>
namespace {
std::ofstream &sizes() {
  static std::ofstream out("sizes.log");
  return out;
}
}
#endif

class string::representation {
public:
  representation(range_t<utf8_t const *> d, bool o = true)
  : ref_count(1), size(d.size()), length(0), owned(o), data(d.begin()) {
#ifdef DEBUG
    if (owned)
      sizes() << "string/standard:" << long(data) << ' ' << size << '\n';
#endif
  }

  enum foreign_tag { foreign_copy };

  representation(range_t<utf8_t const *> const &x, foreign_tag)
  : ref_count(1), size(x.size()), length(0), owned(true),
      data(new utf8_t[size]) {
#ifdef DEBUG
    sizes() << "string/foreign:" << long(data) << ' ' << size << '\n';
#endif
    utf8_t *out = const_cast<utf8_t *>(data);
    for (utf8_t const *it = x.begin(); it != x.end(); ++it)
      *out++ = *it;
  }

  ~representation() {
    if (owned) {
#ifdef DEBUG
      sizes() << "~string/*:" << long(data) << ' ' << size << '\n';
#endif
      delete [] data;
    }
  }

  std::size_t ref_count;
  std::size_t size;
  std::size_t length;
  bool owned;
  utf8_t const *data;
};

string::string(string const &o) : p(o.p) {
  ++p->ref_count;
}

string::~string() {
  if (--p->ref_count == 0)
    delete p;
}

void string::set_up(range_t<utf8_t const *> const &d, bool o) {
  p = new representation(d, o);
}

void string::foreign(range_t<utf8_t const *> const &d) {
  p = new representation(d, representation::foreign_copy);
}

range_t<gott::utf8_t const *> string::as_utf8() const {
  return range(p->data, p->size);
}

std::size_t string::length() const {
  if (!p->length)
    p->length = as_utf32().size();
  return p->length;
}

#ifndef NO_STDLIB
std::ostream &gott::operator<<(std::ostream &stream, string const &s) {
  for (utf8_t const *it = s.as_utf8().begin(); it < s.as_utf8().end(); ++it)
    stream << char(*it);
  return stream;
}

#ifdef HAVE_WIDE_STDLIB
std::wostream &gott::operator<<(std::wostream &stream, string const &s) {
  for (utf8_iterator it = s.as_utf32().begin(); it < s.as_utf32().end(); ++it)
    stream << wchar_t(*it);
  return stream;
}
#endif
#endif

bool gott::operator==(string const &a, string const &b) {
  if (a.as_utf8().begin() == b.as_utf8().begin())
    return true;
  return a.as_utf8() == b.as_utf8();
}

int gott::compare(string const &a, string const &b) {
  if (a.as_utf8().begin() == b.as_utf8().begin())
    return 0;

  utf8_t const *p1 = a.as_utf8().begin();
  utf8_t const *p2 = b.as_utf8().begin();
  for (; p1 != a.as_utf8().end() && p2 != b.as_utf8().end(); ++p1, ++p2)
    if (*p1 != *p2)
      break;
  if (p1 == a.as_utf8().end()) {
    if (p2 == b.as_utf8().end())
      return 0;
    return 1;
  } else if (p2 == b.as_utf8().end())
    return -1;
  
  return *p2 - *p1;
}

