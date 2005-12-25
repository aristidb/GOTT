// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
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

#include "substring.hpp"
#include "../repatcher_by_name.hpp"
#include <gott/tdl/exceptions.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>
#include <stdexcept>

using gott::tdl::structure::repatch_substring;
using gott::tdl::structure::writable_structure;

repatch_substring::repatch_substring(long l, long r) : left(l), right(r) {
  if (left < 0)
    throw std::invalid_argument(
        "repatch_substring: substring must begin at non-negative position");
}
repatch_substring::~repatch_substring() {}

writable_structure *repatch_substring::deferred_write(
    writable_structure &target) const {
  struct context : simple_repatcher_context {
    long left, right;
    context(writable_structure &target, long l, long r) 
      : simple_repatcher_context(target), left(l), right(r) {}
    void data(xany::Xany const &x) {
      if (!x.compatible<string>())
        throw failed_repatch("repatch_substring: need input string");
      string s = xany::Xany_cast<string>(x);
      long len = s.length();
      if (left > len || right > len || right < -len)
        throw failed_repatch(
            "repatch_substring: substring range out of bounds");
      string::utf32_range in = s.as_utf32(), out;
      if (right > 0) 
        out = offset(simply(in.begin()), left, right);
      else
        out = offset(in, left, right);
      if (out.end() < out.begin())
        throw failed_repatch("repatch_substring: invalid substring");
      target.data(xany::Xany(string(out)));
    }
  };
  return new context(target, left, right);
}

void repatch_substring::reg() {
  struct getter : public repatcher_getter {
    getter() 
    : begun_outer(false), ended_outer(false), pos(outer) {}
    bool begun_outer;
    bool ended_outer;
    enum { outer, inner, p_left, p_right } pos;
    long left, right;
    boost::optional<long> delayed;
    void begin() {
      if (begun_outer) {
        if (pos != outer) fail();
        pos = inner;
      }
      begun_outer = true;
    }
    void end() {
      if (!begun_outer || ended_outer) fail();
      if (pos == outer)
        ended_outer = true;
      else
        pos = outer;
    }
    void data(xany::Xany const &x) {
      if (delayed)
        fail();
      switch (inner) {
      case p_left:
        left = xany::Xany_cast<long>(x);
        break;
      case p_right:
        right = xany::Xany_cast<long>(x);
        break;
      case inner:
        delayed = xany::Xany_cast<long>(x);
        break;
      case outer:
        fail();
      }
    }
    void add_tag(string const &s) {
      if (!begun_outer || ended_outer) fail();
      if (pos != inner)
        return;
      if (s == "left")
        pos = p_left;
      else if (s == "right")
        pos = p_right;
      else
        return;
      if (delayed) {
        if (pos == p_left)
          left = *delayed;
        else if (pos == p_right)
          right = *delayed;
        delayed = boost::none;
      }
    }
    void fail() {
      throw std::invalid_argument("repatch_substring arguments");
    }
    repatcher *result_alloc() const {
      return new repatch_substring(left, right);
    }
    static repatcher_getter *alloc() { return new getter; }
  };
  repatcher_by_name().add("substring", &getter::alloc);
}
