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
#include <gott/util/tdl/exceptions.hpp>
#include <gott/util/autoconv.hpp>
#include <gott/util/range_algo.hpp>

namespace structure = gott::tdl::structure;

using structure::repatch_enumeration;
using structure::writable_structure;

repatch_enumeration::repatch_enumeration(Vector<nstring> pick_ &x) 
: alternatives(x) {}

repatch_enumeration::~repatch_enumeration() {}

const wchar_t failure_message[] = 
    L"repatch_enumeration: could not match enumeration";

writable_structure *
repatch_enumeration::deferred_write(writable_structure &s) const {
  struct context : simple_repatcher_context {
    Vector<nstring> const &alternatives;

    context(writable_structure &s, Vector<nstring> const &a) 
    : simple_repatcher_context(s), alternatives(a) {}

    void data(xany::Xany const &x) {
      if (x.compatible<std::wstring>()) {
        nstring input = xany::Xany_cast<nstring>(x);
        Vector<nstring>::const_iterator it;
        if ((it = find(range(alternatives), input)) == alternatives.end())
          throw failed_repatch(failure_message);
        target.data(xany::Xany(long(it - alternatives.begin())));
      } else
        throw failed_repatch(failure_message);
    }
  };
  return new context(s, alternatives);
}
