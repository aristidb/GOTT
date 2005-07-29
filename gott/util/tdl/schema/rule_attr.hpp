// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
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

#ifndef GOTT_UTIL_TDL_SCHEMA_RULE_ATTR_HPP
#define GOTT_UTIL_TDL_SCHEMA_RULE_ATTR_HPP

#include "rule.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace structure { class repatcher; }

namespace schema {

// Class rule::attributes
// Defines the user-supplyable attributes for a rule
class rule::attributes {
public:
  explicit attributes(bool cc = true, structure::repatcher const *rr = 0) 
  : c(cc), r(rr) {}

  explicit attributes(std::list<std::wstring> const &l, bool cc = true, 
      structure::repatcher const *rr = 0)
  : c(cc), t(l), r(rr) {}

  explicit attributes(std::wstring const &s, bool cc = true, 
      structure::repatcher const *rr = 0)
  : c(cc), t(1, s), r(rr) {}

  template<class T> 
  explicit attributes(std::list<std::wstring> const &l, bool cc, T const &x,
      structure::repatcher const *rr = 0)
  : c(cc), t(l), u(x), r(rr) {}

  bool coat() const { return c; }
  void set_coat(bool x) { c = x; }

  std::list<std::wstring> const &tags() const { return t; }
  void add_tag(std::wstring const &x) { t.push_back(x); }

  xany::Xany const &user() const { return u; }

  structure::repatcher const *repatcher() const { return r; }

private:
  bool c;
  std::list<std::wstring> t;
  xany::Xany u;
  structure::repatcher const *r;
};

}}}}

#endif
