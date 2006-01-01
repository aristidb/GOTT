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

#include "find_literal.hpp"
#include "../repatcher_by_name.hpp"
#include <gott/tdl/exceptions.hpp>
#include <gott/xany/xany.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/optional.hpp>

#include <iostream>

using gott::tdl::structure::repatch_find_literal;
using gott::tdl::structure::writable_structure;
namespace algo = boost::algorithm;

repatch_find_literal::repatch_find_literal(type::location l, string const &s)
: loc(l), literal(s) {}

repatch_find_literal::~repatch_find_literal() {}

writable_structure *
repatch_find_literal::deferred_write(writable_structure &target) const {
  struct context : simple_repatcher_context {
    context(type::location l, string s, writable_structure &t)
    : simple_repatcher_context(t), loc(l), literal(s) {}

    type::location loc;
    string literal;

    void data(xany::Xany const &x) {
      if (!x.compatible<string>())
        throw failed_repatch("repatch_find_literal: need input string");
      string s = xany::Xany_cast<string>(x);
      bool fail = true;
      switch (loc) {
      case type::start:
        fail = !algo::starts_with(s.as_utf32(), literal.as_utf32());
        break;
      case type::end:
        fail = !algo::ends_with(s.as_utf32(), literal.as_utf32());
        break;
      case type::whole:
        fail = !algo::equals(s.as_utf32(), literal.as_utf32());
        break;
      case type::substring:
        fail = !algo::contains(s.as_utf32(), literal.as_utf32());
        break;
      }
      std::cout << "Searched for " << literal << " in " << s <<
        (fail ? " - BAH: " : " - and found: ") << loc << std::endl;
      if (fail) {
        throw failed_repatch("repatch_find_literal " + literal 
            + ": condition not true");
      }
      target.data(x);
    }
  };
  return new context(loc, literal, target);
}

void repatch_find_literal::reg() {
  struct getter : public repatcher_getter {
    getter() : pos(outer) {}
    enum { outer, inner, p_left, p_right } pos;
    boost::optional<type::location> loc;
    boost::optional<string> literal;
    bool has_loc, has_literal;
    void begin() {
      if (pos != outer) fail();
      pos = inner;
    }
    void end() {
      if (pos == outer) fail();
      pos = outer;
    }
    void data(xany::Xany const &x) {
      std::cerr << "<<<" << x.type().name() << ">>>\n";
      if (x.compatible<type::location>()) {
        if (loc) fail();
        loc = xany::Xany_cast<type::location>(x);
       } else if (x.compatible<string>()) {
        if (literal) fail();
        literal = xany::Xany_cast<string>(x);
      } else
        fail();
    }
    void add_tag(string const &) {}
    void fail() const {
      throw std::invalid_argument("repatch_find_literal arguments");
    }
    repatcher *result_alloc() const {
      if (!loc || !literal)
        fail();
      return new repatch_find_literal(*loc, *literal);
    }
    static repatcher_getter *alloc() { return new getter; }
  };
  repatcher_by_name().add("find_literal", &getter::alloc);
}
