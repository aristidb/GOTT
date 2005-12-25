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

#include "enumeration.hpp"
#include "../repatcher_by_name.hpp"
#include <gott/tdl/exceptions.hpp>
#include <gott/range_algo.hpp>
#include <gott/xany/xany.hpp>

namespace structure = gott::tdl::structure;

using structure::repatch_enumeration;
using structure::writable_structure;
using gott::string;

repatch_enumeration::repatch_enumeration(Vector<string> pick_ &x) 
: alternatives(x) {}

repatch_enumeration::~repatch_enumeration() {}

const char failure_message[] = 
    "repatch_enumeration: could not match enumeration";

writable_structure *
repatch_enumeration::deferred_write(writable_structure &s) const {
  struct context : simple_repatcher_context {
    Vector<string> const &alternatives;

    context(writable_structure &s, Vector<string> const &a) 
    : simple_repatcher_context(s), alternatives(a) {}

    void data(xany::Xany const &x) {
      if (x.compatible<string>()) {
        string input = xany::Xany_cast<string>(x);
        Vector<string>::const_iterator it;
        if ((it = find(range(alternatives), input)) == alternatives.end())
          throw failed_repatch(failure_message);
        target.data(xany::Xany(long(it - alternatives.begin())));
      } else
        throw failed_repatch(failure_message);
    }
  };
  return new context(s, alternatives);
}

void repatch_enumeration::reg() {
  struct getter : public repatcher_getter {
    getter() : inner(false) {}
    bool inner;
    Vector<string> all_strings;
    void begin() {
      if (inner) fail();
      inner = true;
    }
    void end() {
      if (!inner) fail();
      inner = false;
    }
    void data(xany::Xany const &x) {
      if (!inner) fail();
      all_strings.Add(xany::Xany_cast<string>(x));
    }
    void add_tag(string const &) {}
    void fail() {
      throw std::invalid_argument("repatch_enumeration arguments");
    }
    repatcher *result_alloc() const {
      return new repatch_enumeration(all_strings);
    }
    static repatcher_getter *alloc() { return new getter; }
  };
  repatcher_by_name().add("enumeration", &getter::alloc);
}
