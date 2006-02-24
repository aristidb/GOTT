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

#include "rule.hpp"
#include "rule_attr.hpp"
#include "by_name.hpp"
#include "item.hpp"
#include "match.hpp"
#include <boost/variant.hpp>

namespace sh = tdl::schema;
using sh::rule_t;
using sh::rule_attr_t;
using sh::item;
using std::vector;
using gott::string;

class rule_t::impl {
public:
  struct immediate {
    abstract_rule abstract;
    rule_attr_t attr;
    vector<rule_t> children;

    immediate(abstract_rule const &ar, rule_attr_t const &a, 
        vector<rule_t> const &c)
    : abstract(ar), attr(a), children(c) {}

    immediate(immediate const &o)
    : abstract(o.abstract), attr(o.attr), children(o.children) {}
  };

  boost::variant<immediate, rule_t const *> data;

  impl(abstract_rule const &ar, rule_attr_t const &a, vector<rule_t> const &c)
  : data(immediate(ar, a, c)) {}

  impl(rule_t const *p)
  : data(p) {}
};

rule_t::rule_t() {}

rule_t::rule_t(abstract_rule const &ar, rule_attr_t const &a,
    vector<rule_t> const &c)
: p(new impl(ar, a, c)) {}

rule_t::rule_t(rule_t const &o) : p(o.p) {}

rule_t::rule_t(rule_t const *ptr) : p(new impl(ptr)) {}

rule_t::~rule_t() {}

void rule_t::operator=(rule_t const &o) {
  p = o.p;
}

item *rule_t::get(match &m) const {
  switch (p->data.which()) {
  case 0: // immediate
    { 
      impl::immediate const &im = boost::get<impl::immediate>(p->data);
      return im.abstract.constructor(im.attr, im.children ,m); 
    }
  case 1: // indirect
    return boost::get<rule_t const *>(p->data)->get(m);
  }
  throw 0;
}

rule_attr_t const &rule_t::attributes() const {
  switch (p->data.which()) {
  case 0: // immediate
    return boost::get<impl::immediate>(p->data).attr;
  case 1: // indirect
    return boost::get<rule_t const *>(p->data)->attributes();
  }
  throw 0;
}

bool rule_t::accept_empty() const {
  switch (p->data.which()) {
  case 0: // immediate
    { 
      impl::immediate const &imm = boost::get<impl::immediate>(p->data);
      return imm.abstract.accept_empty(imm.attr, imm.children);
    }
  case 1: // indirect
    return boost::get<rule_t const *>(p->data)->accept_empty();
  }
  throw 0;
}

rule_t sh::rule(string const &id, rule_attr_t const &a,vector<rule_t> const &c){
  return by_name().get(id, a, c);
}
