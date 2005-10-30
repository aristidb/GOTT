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
#include "slot.hpp"
#include <boost/shared_ptr.hpp>
#if !defined(BOOST_PARAMETER_MAX_ARITY) || BOOST_PARAMETER_MAX_ARITY < 6
#define BOOST_PARAMETER_MAX_ARITY 6
#endif
#include <boost/parameter.hpp>
#include <gott/util/string/string.hpp>
#include <iosfwd>
#include <ntl.h>

namespace gott {
namespace tdl {
namespace structure { class repatcher; }

namespace schema {

// Class rule_attr
// Defines the user-supplyable rule_attr_t for a rule_t
class rule_attr_t {
public:
  enum simple_tag { simple };
  rule_attr_t(simple_tag = simple, bool cc = true, 
      structure::repatcher const *rr = 0) 
  : c(cc), r(rr) {}

  explicit rule_attr_t(Vector<string> const &l, bool cc = true, 
      structure::repatcher const *rr = 0)
  : c(cc), t(l), r(rr) {}

  explicit rule_attr_t(string const &s, bool cc = true, 
      structure::repatcher const *rr = 0)
  : c(cc), t(Vector<string>() << s), r(rr) {}

  explicit rule_attr_t(Vector<string> const &l, bool cc, 
                     xany::Xany const &x, structure::repatcher const *rr = 0,
                     slotcfg const &I = slotcfg(), slotcfg const &O = slotcfg())
  : c(cc), t(l), u(x), r(rr), i(I), o(O) {}

  rule_attr_t(rule_attr_t const &o_)
  : c(o_.c), t(o_.t, 1), u(o_.u), r(o_.r), i(o_.i), o(o_.o) {}

  bool coat() const { return c; }

  Vector<string> const &tags() const { return t; }

  xany::Xany const &user() const { return u; }

  structure::repatcher const *repatcher() const { return r.get(); }

  bool operator==(rule_attr_t const &o) const {
    return c == o.c && range(t) == range(o.t) && u == o.u && r == o.r;
  }

  slotcfg const &inner() const { return i; }

  slotcfg const &outer() const { return o; }

private:
  bool c;
  Vector<string> t;
  xany::Xany u;
  boost::shared_ptr<structure::repatcher const> r;
  slotcfg i;
  slotcfg o;
};

namespace {

BOOST_PARAMETER_KEYWORD(tg, tag)
BOOST_PARAMETER_KEYWORD(tg, tags)
BOOST_PARAMETER_KEYWORD(tg, coat)
BOOST_PARAMETER_KEYWORD(tg, user)
BOOST_PARAMETER_KEYWORD(tg, repatcher)
BOOST_PARAMETER_KEYWORD(tg, inner)
BOOST_PARAMETER_KEYWORD(tg, outer)

typedef boost::parameter::parameters<
  tg::tags, tg::coat, tg::user, tg::repatcher, tg::inner, tg::outer
> rule_attr_params;

Vector<string> entag(string const &s) { return Vector<string>() << s; }
Vector<string> entag(Vector<string> const &v) { return v; }

}

BOOST_PARAMETER_FUN(rule_attr_t, rule_attr, 0, 6, rule_attr_params);

template<class Args>
rule_attr_t rule_attr_with_named_params(Args const &args) {
  struct combine_strip {
    Vector<string> operator() (string const &s) const { 
      if (s != "")
        return Vector<string>() << s; 
      else
        return Vector<string>();
    }
  };
  return rule_attr_t(
      args[tags | combine_strip()(args[tag | ""])],
      args[coat | true],
      args[user | xany::Xany()],
      args[repatcher | static_cast<structure::repatcher *>(0)],
      args[inner | slotcfg()],
      args[outer | slotcfg()]
  );
}

GOTT_EXPORT std::ostream &operator<<(std::ostream &s, rule_attr_t const &a);

}}}

#endif
