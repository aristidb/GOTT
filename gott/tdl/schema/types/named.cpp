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

#include "named.hpp"
#include "../event.hpp"
#include <gott/string/string.hpp>
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <boost/assign/list_of.hpp>
#include <cassert>

using std::vector;
using gott::string;
using gott::xany::Xany;
using gott::xany::Xany_cast;

namespace schema = tdl::schema;
namespace ev = tdl::schema::ev;
using schema::item;
using schema::rule_attr_t;
using schema::match_named;
using namespace boost::assign;

rule_attr_t match_named::attributes(string const &s, bool cc) {
  return rule_attr_t(vector<string>(1, s), cc, Xany(s));
}

match_named::match_named(rule_attr_t const &a, vector<rule_t> const &s,match &m)
: happy_once(a, m), 
  tag(Xany_cast<string>(a.user())),
  rewritten(
      rule("follow", rule_attr_t(rule_attr_t::simple, false), list_of 
        (rule("node", rule_attr_t(rule_attr_t::simple, false, 
          new structure::repatch_enumeration(std::vector<string>(1, tag)))))
        (s[0]))) {
  assert(s.size() == 1);
  matcher().add(rewritten);
}

match_named::~match_named() {}

bool match_named::play(ev::child_succeed const &) {
  be_happy();
  return true;
}

gott::string match_named::name() const {
  return gott::string("named");
}
