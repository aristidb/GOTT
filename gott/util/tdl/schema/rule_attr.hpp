// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
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

#ifndef GOTT_UTIL_TDL_SCHEMA_RULE_ATTR_HPP
#define GOTT_UTIL_TDL_SCHEMA_RULE_ATTR_HPP

#include "rule.hpp"
#include "../structure/repatch.hpp"
#include <boost/shared_ptr.hpp>
#include <gott/util/nstring/nstring.hpp>

namespace gott {
namespace tdl {
namespace structure { class repatcher; }

namespace schema {

// Class rule::rule_attr
// Defines the user-supplyable rule_attr for a rule
class rule_attr {
public:
  enum simple_tag { simple };
  explicit rule_attr(simple_tag = simple, bool cc = true, 
      structure::repatcher const *rr = 0) 
  : c(cc), r(rr) {}

  explicit rule_attr(std::list<nstring> const &l, bool cc = true, 
      structure::repatcher const *rr = 0)
  : c(cc), t(l), r(rr) {}

  explicit rule_attr(nstring const &s, bool cc = true, 
      structure::repatcher const *rr = 0)
  : c(cc), t(1, s), r(rr) {}

  explicit rule_attr(std::list<nstring> const &l, bool cc, 
                     xany::Xany const &x, structure::repatcher const *rr = 0)
  : c(cc), t(l), u(x), r(rr) {}

  bool coat() const { return c; }
  void set_coat(bool x) { c = x; }

  std::list<nstring> const &tags() const { return t; }
  void add_tag(nstring const &x) { t.push_back(x); }

  xany::Xany const &user() const { return u; }

  structure::repatcher const *repatcher() const { return r.get(); }

private:
  bool c;
  std::list<nstring> t;
  xany::Xany u;
  boost::shared_ptr<structure::repatcher const> r;
};

}}}

#endif
