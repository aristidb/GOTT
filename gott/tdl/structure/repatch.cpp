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

#include "repatch.hpp"
#include "../exceptions.hpp"
#include <gott/range_algo.hpp>
#include <boost/checked_delete.hpp>
#include <boost/bind.hpp>

namespace structure = gott::tdl::structure;
using structure::repatcher;
using structure::failed_repatch;
using structure::writable_structure;
using structure::simple_repatcher_context;
using structure::repatch_nothing;
using structure::repatcher_chain;

repatcher::repatcher() {}
repatcher::~repatcher() {}

failed_repatch::failed_repatch(string const &s) : tdl_exception(s) {}
failed_repatch::~failed_repatch() throw() {}

simple_repatcher_context::~simple_repatcher_context() {}

void simple_repatcher_context::begin() {
  target.begin();
}

void simple_repatcher_context::end() {
  target.end();
}

void simple_repatcher_context::data(xany::Xany const &x) {
  target.data(x);
}

void simple_repatcher_context::add_tag(string const &s) {
  target.add_tag(s);
}

repatch_nothing::repatch_nothing() {}
repatch_nothing::~repatch_nothing() {}

writable_structure *
repatch_nothing::deferred_write(writable_structure &s) const {
  return new simple_repatcher_context(s);
}

repatcher_chain::repatcher_chain() {}
repatcher_chain::~repatcher_chain() {
  for_each(range(el), boost::checked_deleter<repatcher>());
}

repatcher_chain::repatcher_chain(repatcher_chain const &o) 
: concrete_repatcher<repatcher_chain>() {
  el.resize(o.el.size());
  transform(range(o.el), range(el), boost::bind(&repatcher::clone, _1));
}

void repatcher_chain::push_back(repatcher const &r) {
  el.push_back(r.clone());
}

void repatcher_chain::push_back_alloc(repatcher *r) {
  el.push_back(r);
}

writable_structure *
repatcher_chain::deferred_write(writable_structure &s) const {
  struct context : public writable_structure {
    std::vector<writable_structure *> out;
    context(std::vector<repatcher *> const &el, writable_structure &target) {
      out.resize(el.size());
      int i = el.size() - 1;
      out[i] = el[i]->deferred_write(target);
      while (--i >= 0)
        out[i] = el[i]->deferred_write(*out[i + 1]);
    }
    ~context() {
      for_each(range(out), boost::checked_deleter<writable_structure>());
    }
    void begin() { out[0]->begin(); }
    void end() { out[0]->end(); }
    void data(xany::Xany const &x) { out[0]->data(x); }
    void add_tag(string const &s) { out[0]->add_tag(s); }
  };
  return new context(el, s);
}
