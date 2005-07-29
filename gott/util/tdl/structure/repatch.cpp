// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

#include "repatch.hpp"
#include "../exceptions.hpp"

namespace structure = gott::util::tdl::structure;
using structure::repatcher;
using structure::failed_repatch;
using structure::writable_structure;
using structure::repatch_nothing;
using structure::repatch_node_context;

repatcher::repatcher() {}
repatcher::~repatcher() {}

failed_repatch::failed_repatch(std::wstring const &s) : tdl_exception(s) {}
failed_repatch::~failed_repatch() throw() {}

repatch_nothing::repatch_nothing() {}
repatch_nothing::~repatch_nothing() {}

writable_structure *
repatch_nothing::deferred_write(writable_structure &s) const {
  struct context : writable_structure {
    writable_structure &target;
    context(writable_structure &s) : target(s) {}

    void begin() { target.begin(); }
    void end() { target.end(); }
    void data(xany::Xany const &x) { target.data(x); }
    void add_tag(std::wstring const &s) { target.add_tag(s); }
    void set_tags(std::list<std::wstring> const &x) { target.set_tags(x); }
  };
  return new context(s);
}

void repatch_node_context::begin() {
  throw failed_repatch(L"accept node data solely");
}

void repatch_node_context::end() {
  throw failed_repatch(L"accept node data solely");
}

void repatch_node_context::add_tag(std::wstring const &) {
  throw failed_repatch(L"accept node data solely");
}

void repatch_node_context::set_tags(std::list<std::wstring> const &) {
  throw failed_repatch(L"accept node data solely");
}


