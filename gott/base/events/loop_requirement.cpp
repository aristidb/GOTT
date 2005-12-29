// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT main loop spawner
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

#include "loop_requirement.hpp"
#include "auto_loop.hpp"
#include <boost/variant.hpp>
#include <boost/tuple/tuple.hpp>

using boost::variant;
using boost::tuple;
using gott::events::loop_requirement;

class loop_requirement::IMPL {
public:
  typedef tuple<loop_requirement, loop_requirement, combiner_t> combined;
  typedef tuple<std::type_info const &, feature_flag_t> feature;
  typedef 
    variant<
      feature,
      combined
    > data_t;
  data_t data;

  IMPL(data_t const &d) : data(d) {}
};

loop_requirement::loop_requirement(
    loop_requirement const &lhs,
    loop_requirement const &rhs,
    combiner_t comb)
: p(new IMPL(IMPL::combined(lhs, rhs, comb))) {}

loop_requirement::loop_requirement(
    std::type_info const &id,
    feature_flag_t indicator)
: p(new IMPL(IMPL::feature(id, indicator))) {}

bool loop_requirement::do_try(auto_loop &x) const {
  return do_try(x, this);
}

bool loop_requirement::do_try(auto_loop &x, loop_requirement const *w) const {
  if (p->data.type() == typeid(IMPL::combined)) {
    IMPL::combined const &comb = boost::get<IMPL::combined>(p->data);
    switch (comb.get<2>()) {
    case combine_and:
      return comb.get<0>().do_try(x, w) && comb.get<1>().do_try(x, w);
    case combine_or:
      return comb.get<0>().do_try(x, w) || comb.get<1>().do_try(x, w);
    default: throw 0;
    }
  } else if (p->data.type() == typeid(IMPL::feature)) {
    return x.try_feature(boost::get<IMPL::feature>(p->data).get<0>(), w);
  } else throw 0;
}
