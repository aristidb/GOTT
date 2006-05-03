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
 * The Original Code is A General Purpose Configuration Library.
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

#include <gott/configuration/read_tdl.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/structure/repatchers/integer.hpp>
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/structure/container.hpp>
#include <gott/tdl/structure/revocable_adapter.hpp>
#include <gott/tdl/structure/print.hpp>
#include <boost/assign/list_of.hpp>
#include <iostream>

using namespace gott;
using namespace tdl;
using namespace tdl::schema;
using namespace boost::assign;

int main() {
  std::istream::sync_with_stdio(false);

  std::cerr 
    <<
    "Simple config test: TDL configuration\n"
    "There are the following keys:\n"
    "  - conf::int : an integer\n"
    "  - conf::string : an arbitrary string\n"
    "  - get::more::stuff : a list of integers\n"
    ;
  
  rule_t conf = 
    rule_one("tdl::schema::document",
      rule("tdl::schema::list", rule_attr(tag = "container"),
        list_of(rule("gott::tdl_config", rule_attr(outer = list(), tag = "element"),
          list_of
          (rule("tdl::schema::node", rule_attr(tag = "conf::int", 
              repatcher = new structure::repatch_integer())))
          (rule("tdl::schema::node", rule_attr(tag = "conf::string")))
          (rule("tdl::schema::list", rule_attr(tag = "get::more::stuff"),
            list_of(rule("tdl::schema::node", rule_attr(outer = list())))))))));

  structure::container out;
  structure::revocable_adapter r(out);
  match m(conf, r);
  m.parse(std::cin);
  std::cout << out << '\n';
}
