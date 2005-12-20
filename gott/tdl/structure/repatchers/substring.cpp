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
#include <gott/tdl/exceptions.hpp>
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
        throw failed_repatch("repatch_substring: substring range out of bounds");
      string::utf32_range r = s.as_utf32();
      string out;
      if (right > 0)
        out = offset(simply(r.begin()), left, right);
      else
        out = offset(r, left, right);
      target.data(xany::Xany(out));
    }
  };
  return new context(target, left, right);
}
