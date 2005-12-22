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
  return repo.Get(name)();
}

repatcher_getter *repatcher_by_name_t::chain_alloc(
    Vector<string> const &names) const {
  //FIXME need _getter!
#if 0
  repatcher_chain *result = new repatcher_chain;
  using boost::bind;
  for_each(range(names), 
      bind(&repatcher_chain::push_back_alloc, result, 
        bind(&repatcher_by_name_t::get_alloc, this, _1)));
  return result;
#endif
}
