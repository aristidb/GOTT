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
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#include "find_literal.hpp"
#include "../repatcher_by_name.hpp"
#include <gott/tdl/exceptions.hpp>
#include <gott/xany.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/optional.hpp>

using gott::string;
using tdl::structure::repatch_find_literal;
using tdl::structure::writable_structure;
namespace algo = boost::algorithm;

repatch_find_literal::repatch_find_literal(type::location l, string const &s)
: loc(l), literal(s) {}

repatch_find_literal::~repatch_find_literal() {}

writable_structure *
repatch_find_literal::deferred_write(writable_structure &target) const {
  struct context : simple_repatcher_context {
    context(type::location l, string s, writable_structure &t)
    : simple_repatcher_context(t), loc(l), literal(s) {}

    type::location loc;
    string literal;

    void data(gott::xany::Xany const &x) {
      if (!x.compatible<string>())
        throw tdl_error("TDL Structure repatcher", 
            "find_literal needs string input");
      string s = gott::xany::Xany_cast<string>(x);
      bool fail = true;
      switch (loc) {
      case type::start:
        fail = !algo::starts_with(s.as_utf32(), literal.as_utf32());
        break;
      case type::end:
        fail = !algo::ends_with(s.as_utf32(), literal.as_utf32());
        break;
      case type::whole:
        fail = !algo::equals(s.as_utf32(), literal.as_utf32());
        break;
      case type::substring:
        fail = !algo::contains(s.as_utf32(), literal.as_utf32());
        break;
      }
      if (fail)
        throw tdl_error("TDL Structure repatcher", 
            "find_literal condition not fulfilled");
      target.data(x);
    }
  };
  return new context(loc, literal, target);
}

void repatch_find_literal::reg() {
  struct getter : public repatcher_getter {
    getter() : pos(outer) {}
    enum { outer, inner, p_left, p_right } pos;
    boost::optional<type::location> loc;
    boost::optional<string> literal;
    bool has_loc, has_literal;
    void begin(source_position const &) {
      if (pos != outer) fail();
      pos = inner;
    }
    void end() {
      if (pos == outer) fail();
      pos = outer;
    }
    void data(gott::xany::Xany const &x) {
      if (x.compatible<type::location>()) {
        if (loc) fail();
        loc = gott::xany::Xany_cast<type::location>(x);
       } else if (x.compatible<string>()) {
        if (literal) fail();
        literal = gott::xany::Xany_cast<string>(x);
      } else
        fail();
    }
    void add_tag(string const &) {}
    void fail() const {
      throw tdl_error("TDL Structure repatcher loader",
          "non-sensible arguments");
    }
    repatcher *result_alloc() const {
      if (!loc || !literal)
        fail();
      return new repatch_find_literal(*loc, *literal);
    }
    static repatcher_getter *alloc() { return new getter; }
  };
  repatcher_by_name().add("find-literal", &getter::alloc);
}
