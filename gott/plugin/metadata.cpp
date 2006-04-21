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
 * The Original Code is A Plugin / Loadable Module Engine.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#include "metadata.hpp"
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/by_name.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <gott/tdl/schema/slot.hpp>
#include <gott/tdl/structure/revocable_adapter.hpp>
#include <boost/assign/list_of.hpp>

using gott::plugin::plugin_metadata;
using std::istream;

istream &gott::plugin::operator>>(istream &stream, plugin_metadata &out_value) {
  using namespace tdl::structure;
  using namespace tdl::schema;
  using namespace boost::assign;
  using gott::xany::Xany;

  struct accepter : writable_structure {
    accepter(plugin_metadata &ref) : ref(ref) {}

    plugin_metadata &ref;

    string tag;
    Xany data_;

    void begin(tdl::source_position const &) {}
    void end() {
      using gott::xany::Xany_cast;
      if (tag == "interface")
        ref.interfaces.push_back(Xany_cast<string>(data_));
      else if (tag == "plugin-id")
        ref.plugin_id = Xany_cast<string>(data_);
      tag = string();
    }

    void add_tag(string const &t) { tag = t; }
    void data(Xany const &x) { data_ = x; }
  };

  accepter out(out_value);
  revocable_adapter adapter(out);
  match matcher(adapter);
  matcher.add(
      rule_one("document", rule_attr(coat = false),
        rule("unordered", rule_attr(coat = false),
          list_of
          (rule_one("named",
                    rule_attr(tag = "plugin-id"),
                    rule("node")))
          (rule_one("named",
                    rule_attr(tag = "interface", outer = list()),
                    rule("node"))))));
  matcher.parse(stream);
  return stream;
}
