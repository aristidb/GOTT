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

#include "repatcher_by_name.hpp"
#include "repatch.hpp"
#include <gott/range_algo.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

using gott::tdl::structure::repatcher_getter;
using gott::tdl::structure::repatcher_by_name_t;

NTL_MOVEABLE(boost::function<repatcher_getter *()>)

repatcher_getter::repatcher_getter() {}
repatcher_getter::~repatcher_getter() {}

repatcher_by_name_t::repatcher_by_name_t() {}
repatcher_by_name_t::~repatcher_by_name_t() {}

repatcher_by_name_t &gott::tdl::structure::repatcher_by_name() {
  static repatcher_by_name_t instance;
  return instance;
}

void repatcher_by_name_t::add(
    string const &name, 
    boost::function<repatcher_getter *()> const &func) {
  repo.Add(name, func);
}

repatcher_getter *repatcher_by_name_t::get_alloc(string const &name) const {
  int pos = repo.Find(name);
  if (pos < 0)
    throw std::bad_exception();
  return repo[pos]();
}

repatcher_getter *repatcher_by_name_t::chain_alloc() const {
  struct getter : repatcher_getter {
    getter(repatcher_by_name_t const &r) 
    : ref(r), pos(outer), result(new repatcher_chain) {}
    repatcher_by_name_t const &ref;
    string what;
    boost::scoped_ptr<repatcher_getter> where;
    repatcher_chain *result;
    enum { outer, inner1, inner2 } pos;
    unsigned inner2_level;
    void begin() {
      switch (pos) {
      case outer:
        pos = inner1; break;
      case inner1:
        where.reset(ref.get_alloc(what));
        inner2_level = 0;
        pos = inner2; break;
      case inner2:
        ++inner2_level;
        where->begin();
      }
    }
    void end() {
      switch (pos) {
      case inner2:
        if (inner2_level) {
          where->end();
          --inner2_level;
        } else {
          result->push_back_alloc(where->result_alloc());
          where.reset();
          pos = inner1;
        }
        break;
      case inner1:
        pos = outer; break;
      case outer:
        fail();
      }
    }
    void data(xany::Xany const &x) {
      switch (pos) {
      case outer:
        fail();
      case inner1:
        what = xany::Xany_cast<string>(x); break;
      case inner2:
        where->data(x); break;
      }
    }
    void add_tag(string const &s) {
      if (pos == inner2)
        where->add_tag(s);
    }
    void fail() {
      throw std::invalid_argument("chain_alloc: invalid input");
    }
    repatcher *result_alloc() const {
      return result;
    }
  };
  return new getter(*this);
}
