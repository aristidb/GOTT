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

#include "by_name.hpp"
#include "../exceptions.hpp"
#include <gott/string/string.hpp>
#include <map>

namespace schema = tdl::schema;
using schema::by_name_t;
using gott::string;

by_name_t &schema::by_name() {
  static by_name_t m;
  return m;
}

class by_name_t::impl {
public:
  typedef std::map<string, abstract_rule> mapping; 
  mapping items;

  abstract_rule const &get(string const &name) {
    mapping::const_iterator it = items.find(name);
    if (it == items.end())
      throw unregistered_type(name);
    return it->second;
  }
};

by_name_t::by_name_t() : p(new impl) {
}

by_name_t::~by_name_t() {
}

void by_name_t::add(string const &name, abstract_rule const &type) {
  p->items.insert(impl::mapping::value_type(name, type));
}

schema::rule_t by_name_t::get(string const &n, rule_attr_t const &a, 
                              std::vector<rule_t> const &c) const {
  return rule_t(p->get(n), a, c);
}
