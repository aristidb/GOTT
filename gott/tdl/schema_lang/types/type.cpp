// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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

#include <gott/plugin.hpp>
#include <gott/tdl/schema/slot.hpp>
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/happy_once.hpp>
#include <gott/tdl/schema/type.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <gott/tdl/schema/event.hpp>
#include <boost/assign/list_of.hpp>

using namespace tdl::schema;
using tdl::structure::writable_structure;
using gott::string;
using gott::Xany;
using std::vector;

namespace {

struct make_rule {
  vector<rule_t> &out;

  void operator() (type const *tp) const {
    out.push_back(
      rule_one("named",
        rule_attr(tag = tp->get_id().get_string()),
        rule("ordered", rule_attr(tag = "par"),
          boost::assign::list_of
          (rule("tree",
            rule_attr(tag = "parameter", outer = tp->parameters())))
          (rule("tdl::schema_lang::type",
            rule_attr(tag = "child", outer = tp->children())))
        )
      )
    );
  }
};

class xmatcher : public happy_once {
public:
  xmatcher(rule_attr_t const &a, vector<rule_t> const &, match &m)
  : happy_once(a, m) {
    vector<rule_t> alternatives;
    make_rule helper = { alternatives };
    tdl::resource::list<type>(helper);
    m.add(rule("any", rule_attr(coat = false), alternatives));
  }

  static gott::atom const id;

  static bool accept_empty(rule_attr_t const &, std::vector<rule_t> const &)
  { return false; }

  static slotcfg n_parameters() { return exactly(0); }
  static slotcfg n_children() { return exactly(0); }

  string name() const { return id.get_string(); }

private:
  bool play(ev::child_succeed const &) {
    be_happy();
    return true;
  }

  slotcfg out;
};

}

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(plugin_lang_type, concrete_type<xmatcher>)

GOTT_PLUGIN_METADATA(
"plugin \"\"\n"
"interface tdl::resource\n"
"enclosing-module tdl::schema_lang\n"
"symbol plugin_lang_type\n\n"

"module tdl::schema_lang\n"
"file-path ??FILE??\n"
)

gott::atom const xmatcher::id("tdl::schema_lang::type");
