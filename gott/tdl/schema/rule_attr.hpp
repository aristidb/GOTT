/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef GOTT_UTIL_TDL_SCHEMA_RULE_ATTR_HPP
#define GOTT_UTIL_TDL_SCHEMA_RULE_ATTR_HPP

#include "../structure/repatch.hpp"
#include "slot.hpp"
#include <boost/shared_ptr.hpp>
#if !defined(BOOST_PARAMETER_MAX_ARITY) || BOOST_PARAMETER_MAX_ARITY < 6
#undef BOOST_PARAMETER_MAX_ARITY
#define BOOST_PARAMETER_MAX_ARITY 6
#endif
#include <boost/parameter.hpp>
#include <gott/string/string.hpp>
#include <iosfwd>
#include <vector>

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

  explicit rule_attr_t(std::vector<string> const &l, bool cc = true, 
      structure::repatcher const *rr = 0)
  : c(cc), t(l), r(rr) {}

  explicit rule_attr_t(string const &s, bool cc = true, 
      structure::repatcher const *rr = 0)
  : c(cc), t(std::vector<string>(1, s)), r(rr) {}

  explicit rule_attr_t(std::vector<string> const &l, bool cc, 
                     xany::Xany const &x, structure::repatcher const *rr = 0,
                     slotcfg const &I = slotcfg(), slotcfg const &O = slotcfg())
  : c(cc), t(l), u(x), r(rr), i(I), o(O) {}

  rule_attr_t(rule_attr_t const &o_)
  : c(o_.c), t(o_.t), u(o_.u), r(o_.r), i(o_.i), o(o_.o) {}

  bool coat() const { return c; }

  std::vector<string> const &tags() const { return t; }

  xany::Xany const &user() const { return u; }

  structure::repatcher const *repatcher() const { return r.get(); }

  bool operator==(rule_attr_t const &o) const {
    return c == o.c && range(t) == range(o.t) && u == o.u && r == o.r;
  }

  slotcfg const &inner() const { return i; }

  slotcfg const &outer() const { return o; }

private:
  bool c;
  std::vector<string> t;
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

}

BOOST_PARAMETER_FUN(rule_attr_t, rule_attr, 0, 6, rule_attr_params);

template<class Args>
rule_attr_t rule_attr_with_named_params(Args const &args) {
  struct combine_strip {
    std::vector<string> operator() (string const &s) const { 
      if (s != "")
        return std::vector<string>(1, s); 
      else
        return std::vector<string>();
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
