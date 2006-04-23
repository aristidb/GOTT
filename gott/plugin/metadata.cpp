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
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <gott/tdl/write/writer.hpp>
#include <gott/exceptions.hpp>
#include <gott/range_algo.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/thread.hpp>
#include <map>
#include <fstream>

using namespace tdl::schema;
using namespace tdl::structure;
using namespace gott::xany;
using namespace gott::plugin;
using namespace gott;
using std::istream;
using std::ostream;

namespace {
  static boost::recursive_mutex metadata_biglock;
  #define BIGLOCK boost::recursive_mutex::scoped_lock B_lock(metadata_biglock)

  typedef std::vector<plugin_metadata> plugin_metadata_list_t;
  plugin_metadata_list_t known_plugin_metadata;

  /*
  ordered
    named (plugin-id), node
    unordered
      :list named (has-interface), node
      named (module-type), enumeration $ dynamic-native
      named (file-path), node
   */
  static rule_t metadata_schema = 
    rule("ordered", rule_attr(coat = false, outer = optional()),
        boost::assign::list_of
        (rule_one("named", rule_attr(tag = "plugin-id"), rule("node")))
        (rule("unordered", rule_attr(coat = false),
          boost::assign::list_of
          (rule_one("named", rule_attr(tag = "has-interface", outer = list()),
                    rule("node")))
          (rule_one("named", rule_attr(tag = "module-type"),
                    rule("node",
                      rule_attr(tdl::schema::repatcher = 
                        new repatch_enumeration(
                          std::vector<string>(1, "dynamic-native"))))))
          (rule_one("named", rule_attr(tag = "file-path"), rule("node"))))));

  struct accepter : writable_structure {
    accepter(plugin_metadata &ref) : ref(ref) {}

    plugin_metadata &ref;

    string tag;
    Xany data_;

    void begin(tdl::source_position const &) {}
    void end() {
      if (tag == "has-interface")
        ref.interfaces.push_back(Xany_cast<string>(data_));
      else if (tag == "plugin-id")
        ref.plugin_id = Xany_cast<string>(data_);
      else if (tag == "module-type")
        ref.module_type = Xany_cast<plugin_metadata::module_type_t>(data_);
      else if (tag == "file-path")
        ref.file_path = Xany_cast<string>(data_);
      tag = string();
    }

    void add_tag(string const &t) { tag = t; }
    void data(Xany const &x) { data_ = x; }
  };
}

void gott::plugin::load_standard_metadata() {
  BIGLOCK;
  static bool loaded;
  if (!loaded) {
    std::ifstream in_std("plugin_registry.tdl");
    extract_plugin_metadata(in_std);
    loaded = true;
  }
}

void gott::plugin::enumerate_plugin_metadata_p(
    boost::function<void (plugin_metadata const &)> const &function,
    boost::optional<QID const &> const &plugin_id,
    boost::optional<QID const &> const &interface_id,
    bool cancel_early) {
  BIGLOCK;
  for (plugin_metadata_list_t::iterator it = known_plugin_metadata.begin();
      it != known_plugin_metadata.end();
      ++it) {
    if (plugin_id && it->plugin_id != *plugin_id)
      continue;
    if (interface_id && 
        find(range(it->interfaces), *interface_id) == it->interfaces.end())
      continue;
    function(*it);
    if (cancel_early)
      break;
  }
}

void gott::plugin::add_plugin_metadata(plugin_metadata const &metadata) {
  BIGLOCK;
  known_plugin_metadata.push_back(metadata);
}

void gott::plugin::extract_plugin_metadata(istream &stream) {
  struct multi_accepter : writable_structure {
    multi_accepter() : level(0), inner(current) {}

    unsigned level;
    plugin_metadata current;
    accepter inner;

    void begin(tdl::source_position const &w) {
      if (level == 0) {
        current = plugin_metadata();
      } else
        inner.begin(w);
      ++level;
    }
    void end() {
      --level;
      if (level == 0) {
        add_plugin_metadata(current);
      } else
        inner.end();
    }

    void data(Xany const &x) { inner.data(x); }
    void add_tag(string const &s) { inner.add_tag(s); }
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
  out_value.interfaces.clear();
  accepter out(out_value);
  revocable_adapter adapter(out);
  match(
      rule_one("document", rule_attr(coat = false), metadata_schema),
      adapter)
    .parse(stream);
  return stream;
}

ostream &gott::plugin::operator<<(ostream &stream, plugin_metadata const &val) {
  tdl::tdl_writer w(stream, 2);
  w.down();
  {
    w.node("plugin-id"); 
    w.down();
      w.node(val.plugin_id);
    w.up();
  }
  {
    w.node("module-type");
    w.down();
      switch (val.module_type) {
      case plugin_metadata::dynamic_native:
        w.node("dynamic-native");
        break;
      };
    w.up();
  }
  {
    w.node("file-path");
    w.down();
      w.node(val.file_path);
    w.up();
  }
  {
    for (plugin_metadata::interface_list_t::const_iterator it =
          val.interfaces.begin();
        it != val.interfaces.end();
        ++it) {
      w.node("has-interface");
      w.down();
        w.node(*it);
      w.up();
    }
  }
  w.up();
  return stream;
}
