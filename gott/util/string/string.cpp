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
#include "convert.hpp"
#include "buffer.hpp"
#include "iterator.hpp"
#include "stl.hpp"

#include <gott/util/range.hpp>
#include <gott/util/range_algo.hpp>
#include <gott/util/thunk.hpp>
#include <list>
#include <vector>
#include <ostream>
#include <string>

using gott::string;
using gott::range_t;

class string::representation {
public:
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
      delete [] data;
  }

  enum concatenation_tag { concat };

  template<class I> representation(range_t<I> const &, concatenation_tag);

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

string::string()
: p(new representation(offset(range("").cast<utf8_t const *>(), 0, -1), 
                       false)) {}

string::string(range_t<char const *> in, encoding enc)
: p(new representation(to_utf8_alloc(in, enc))) {}

string::string(char const *in, encoding enc)
: p(new representation(to_utf8_alloc(zero_terminated(in), enc))) {}

string::string(std::string const &s, encoding enc)
: p(new representation(to_utf8_alloc(range(&s[0], s.length()), enc))) {}

string::string(std::wstring const &s, encoding enc)
: p(new representation(
      to_utf8_alloc(range(&s[0], s.length()).cast<char const*>(), enc))) {}

string::string(thunk_t<utf8_t> &thk) : p(0) {
  std::size_t len = thk.size();
  utf8_t *buf = new utf8_t[len];
  for (std::size_t i = 0; i < len; ++i)
    buf[i] = thk.call();
  p = new representation(range(buf, len));
}

string::string(range_t<utf8_t const *> in, literal_tag)
: p(new representation(in, false)) {}

string::string(range_t<wchar_t const *> in, encoding enc)
: p(new representation(to_utf8_alloc(in.cast<char const *>(), enc))) {}

string::string(wchar_t const *in, encoding enc)
: p(new representation(to_utf8_alloc(zero_terminated(in).cast<char const *>(),
        enc))) {}

string::string(string_buffer const &b)
: p(new representation(to_utf8_alloc(range(b).cast<char const *>(), utf32))) {}

string::string(range_t<utf8_t const *> const &r)
: p(new representation(r, representation::foreign_copy)) {}

string::string(range_t<utf8_iterator> const &r)
: p(new representation(r, representation::foreign_copy)) {}

string::string(std::vector<string> const &v)
: p(new representation(range(v), representation::concat)) {}

string::string(std::list<string> const &v)
: p(new representation(range(v), representation::concat)) {}

string::string(range_t<string const *> cont)
: p(new representation(cont, representation::concat)) {}

template<class I>
string::representation::representation(range_t<I> const &r, concatenation_tag) 
: ref_count(1), size(0), length(0), owned(true) {
  for (I it = r.begin; it != r.end; ++it)
    size += it->size();
  utf8_t *current = new utf8_t[size];
  data = current;
  for (I it = r.begin; it != r.end; ++it)
    current = copy(it->as_utf8(), current);
}

range_t<gott::utf8_t const *> string::as_utf8() const {
  return range(p->data, p->size);
}

std::size_t string::length() const {
  if (!p->length)
    p->length = as_utf32().size();
  return p->length;
}

void string::swap(string &o) {
  std::swap(p, o.p);
}

std::ostream &gott::operator<<(std::ostream &stream, string const &s) {
  for (utf8_t const *it = s.as_utf8().begin; it < s.as_utf8().end; ++it)
    stream << char(*it);
  return stream;
}

std::wostream &gott::operator<<(std::wostream &stream, string const &s) {
  for (utf8_iterator it = s.as_utf32().begin; it < s.as_utf32().end; ++it)
    stream << wchar_t(*it);
  return stream;
}

bool gott::operator==(string const &a, string const &b) {
  if (a.as_utf8().begin == b.as_utf8().begin)
    return true;
  return a.as_utf8() == b.as_utf8();
}

int gott::compare(string const &a, string const &b) {
  if (a.as_utf8().begin == b.as_utf8().begin)
    return 0;

  utf8_t const *p1 = a.as_utf8().begin;
  utf8_t const *p2 = b.as_utf8().begin;
  for (; p1 != a.as_utf8().end && p2 != b.as_utf8().end; ++p1, ++p2)
    if (*p1 != *p2)
      break;
  if (p1 == a.as_utf8().end) {
    if (p2 == b.as_utf8().end)
      return 0;
    return 1;
  } else if (p2 == b.as_utf8().end)
    return -1;
  
  return *p2 - *p1;
}

std::size_t 
hashd::hash<gott::string>::operator() (gott::string const &s) const {
  std::size_t result = 0;
  for (gott::utf8_t const *it = s.as_utf8().begin; it != s.as_utf8().end; ++it)
    result = 5 * result + *it; // TODO: compare 31*result+*it
  return result;
}
