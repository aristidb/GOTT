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

#include "integer.hpp"
#include "../repatcher_by_name.hpp"
#include <gott/tdl/exceptions.hpp>
#include <cctype>

namespace structure = gott::tdl::structure;

using structure::repatch_integer;
using structure::writable_structure;

repatch_integer::repatch_integer() {}
repatch_integer::~repatch_integer() {}

writable_structure *
repatch_integer::deferred_write(writable_structure &s) const {
  struct context : simple_repatcher_context {
    context(writable_structure &s) : simple_repatcher_context(s) {}

    void data(xany::Xany const &x) {
      if (x.compatible<long>())
        target.data(x);
      else if (x.compatible<string>()) {
        string input = xany::Xany_cast<string>(x);
        long result;
        if (!is_integer(input, result))
          throw failed_repatch("repatch_integer: could not match integer");
        target.data(xany::Xany(result));
      } else
        throw failed_repatch("repatch_integer: could not match integer");
    }

    bool is_integer(string const &s, long &val) {
      long sign = 1;
      val = 0;
  
      string::utf8_range rng = s.as_utf8();
  
      if (rng.empty())
        return false;

      if (*rng.begin() == '-') {
        ++rng.begin();
        sign = -1;
        if (rng.empty())
          return false;
      }
  
      if (!std::isdigit(*rng.begin()))
        return false;
    
//    if (*it == L'0' && it[1] == L'x')
//      return is_hex(it + 2, s.end(), v, sign)

      for (; !rng.empty() && std::isdigit(*rng.begin()); ++rng.begin())
        val = val * 10 + (*rng.begin() - '0');

      val *= sign;
      return true;
    }
  };
  return new context(s);
}

void repatch_integer::reg() {
  struct getter : public repatcher_getter {
    getter() : begun(false), ended(false) {}
    bool begun;
    bool ended;
    void begin() { 
      if (begun) fail();
      begun = true;
    }
    void end() {
      if (!begun || ended) fail();
      ended = true;
    }
    void data(xany::Xany const &) { fail(); }
    void add_tag(string const &) { fail(); }
    void fail() {
      throw std::invalid_argument("repatch_integer arguments");
    }
    repatcher *result_alloc() const { return new repatch_integer(); }
    static repatcher_getter *alloc() { return new getter; }
  };
  repatcher_by_name().add("integer", &getter::alloc);
}
