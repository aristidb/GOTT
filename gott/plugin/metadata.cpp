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
#include <gott/range_algo.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/optional/optional.hpp>
#include <boost/none.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <list>

using gott::plugin::plugin_metadata;
using std::istream;

using namespace tdl::schema;
using namespace tdl::structure;
using namespace gott::xany;
using gott::string;

static std::list<plugin_metadata> plugin_metadata_database;

namespace {
  /*
  ordered
    named (plugin-id), node
    :list named (interface), node
   */
  static rule_t metadata_schema = 
    rule("ordered", rule_attr(coat = false, outer = optional()),
        boost::assign::list_of
        (rule_one("named", rule_attr(tag = "plugin-id"), rule("node"))) 
        (rule_one("named", rule_attr(tag = "interface", outer = list()),
                  rule("node"))));

  struct accepter : writable_structure {
    accepter(plugin_metadata &ref) : ref(ref) {}

    plugin_metadata &ref;

    string tag;
    Xany data_;

    void begin(tdl::source_position const &) {}
    void end() {
      if (tag == "interface")
        ref.interfaces.push_back(Xany_cast<string>(data_));
      else if (tag == "plugin-id")
        ref.plugin_id = Xany_cast<string>(data_);
      tag = string();
    }

    void add_tag(string const &t) { tag = t; }
    void data(Xany const &x) { data_ = x; }
  };
}

void gott::plugin::enumerate_plugin_metadata(
    boost::function<void (plugin_metadata const &)> const &function) {
  for_each(range(plugin_metadata_database), function);
}

void gott::plugin::add_plugin_metadata(plugin_metadata const &metadata) {
  plugin_metadata_database.push_back(metadata);
}

void gott::plugin::extract_plugin_metadata(istream &stream) {
  struct multi_accepter : writable_structure {
    multi_accepter() : level(0) {}

    boost::optional<accepter> inner;
    unsigned level;

    void begin(tdl::source_position const &w) {
      if (level == 0) {
        plugin_metadata_database.push_back(plugin_metadata());
        inner = boost::in_place(boost::ref(plugin_metadata_database.back()));
      } else
        inner->begin(w);
      ++level;
    }

    void end() {
      --level;
      if (level == 0)
        inner = boost::none;
      else
        inner->end();
    }

    void data(Xany const &x) {
      if (inner) inner->data(x);
    }

    void add_tag(string const &s) {
      if (inner) inner->add_tag(s);
    }
  };
  multi_accepter out;
  revocable_adapter adapter(out);
  match(
      rule_one("document", rule_attr(coat = false),
        rule_one("ordered", rule_attr(outer = list()),
          metadata_schema)),
      adapter).parse(stream);
}

istream &gott::plugin::operator>>(istream &stream, plugin_metadata &out_value) {
  accepter out(out_value);
  revocable_adapter adapter(out);
  match(
      rule_one("document", rule_attr(coat = false), metadata_schema),
      adapter)
    .parse(stream);
  return stream;
}
