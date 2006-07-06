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

#define EXPLICITLY_LOAD_METADATA_INTERNALS
#include "../load.hpp"
#include "../plugin.hpp"
#include "../module.hpp"
#include "../interface.hpp"
#include "../transaction.hpp"
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/by_name.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <gott/tdl/schema/slot.hpp>
#include <gott/tdl/structure/revocable_adapter.hpp>
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <gott/string/qid.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>

using namespace gott::metadata;
using namespace gott::xany;
using namespace tdl::schema;
using namespace tdl::structure;
using gott::string;
using gott::QID;
using std::istream;
using std::ostream;

namespace {
  struct plugin_accepter : writable_structure {
    plugin_accepter() : priority(normal_priority) {}
    
    QID plugin_id;
    std::vector<QID> interfaces;
    QID enclosing_module_id;
    string symbol;
    plugin_priority priority;

    string tag;
    Xany data_;

    void begin(tdl::source_position const &) {}
    void end() {
      if (tag == "has-interface")
        interfaces.push_back(Xany_cast<string>(data_));
      else if (tag == "plugin-id")
        plugin_id = Xany_cast<string>(data_);
      else if (tag == "enclosing-module")
        enclosing_module_id = Xany_cast<string>(data_);
      else if (tag == "symbol")
        symbol = Xany_cast<string>(data_);
      else if (tag == "priority")
        priority = Xany_cast<plugin_priority>(data_);
      tag = string();
    }

    void add_tag(string const &t) { tag = t; }
    void data(Xany const &x) { data_ = x; }
  };
}

void gott::metadata::detail::update_plugin_resource(
    istream &stream,
    string const &resource,
    transaction &tr) {
  tr.remove_resource(resource);
  /*
  ordered
    named (plugin-id), node
    unordered
      :list named (has-interface), node
      named (enclosing-module), node
      named (symbol), node
   */
  rule_t const plugin_schema = 
    rule("tdl::schema::ordered", rule_attr(coat = false),
        boost::assign::list_of
        (rule_one("tdl::schema::named", rule_attr(tag = "plugin-id"),
                  rule("tdl::schema::node")))
        (rule("tdl::schema::unordered", rule_attr(coat = false),
          boost::assign::list_of
          (rule_one("tdl::schema::named",
                    rule_attr(tag = "has-interface"),
                    rule("tdl::schema::node")))
          (rule_one("tdl::schema::named", rule_attr(tag = "enclosing-module"),
                    rule("tdl::schema::node")))
          (rule_one("tdl::schema::named", rule_attr(tag = "symbol"),
                    rule("tdl::schema::node")))
          (rule_one("tdl::schema::named",
                    rule_attr(tag = "priority", outer = optional()),
                    rule("tdl::schema::node",
                      rule_attr(
                        tdl::schema::repatcher = new repatch_enumeration(
                          boost::assign::list_of
                          (string("high"))
                          (string("normal"))
                          (string("low"))
                          .operator std::vector<string>()))))))));

  struct multi_accepter : writable_structure {
    multi_accepter(string const &resource, transaction &tr) 
      : resource(resource), tr(tr), level(0), inner() {}

    string const &resource;
    transaction &tr;
    unsigned level;
    plugin_accepter inner;

    void begin(tdl::source_position const &w) {
      if (level == 0)
        inner = plugin_accepter();
      else
        inner.begin(w);
      ++level;
    }
    void end() {
      --level;
      if (level == 0)
        tr.add_plugin(
            inner.plugin_id,
            inner.interfaces,
            inner.enclosing_module_id,
            inner.symbol,
            inner.priority,
            resource);
      else
        inner.end();
    }

    void data(Xany const &x) { inner.data(x); }
    void add_tag(string const &s) { inner.add_tag(s); }
  };
  multi_accepter out(resource, tr);
  revocable_adapter adapter(out);
  match(
      rule_one("tdl::schema::document", rule_attr(coat = false),
        rule_one("tdl::schema::ordered", rule_attr(outer = list()),
          plugin_schema)),
      adapter).parse(stream);
}

namespace {
  class module_accepter : writable_structure {
  public:
    QID module_id;
    string file_path;
    module::module_type_t module_type;
    std::vector<QID> dependencies;

    string tag;
    Xany data_;

    void begin(tdl::source_position const &) {}
    void end() {
      if (tag == "module-id")
        module_id = Xany_cast<string>(data_);
      else if (tag == "file-path")
        file_path = Xany_cast<string>(data_);
      else if (tag == "module-type")
        module_type = Xany_cast<module::module_type_t>(data_);
      else if (tag == "depend-on")
        dependencies.push_back(Xany_cast<string>(data_));
      tag = string();
    }

    void add_tag(string const &t) { tag = t; }
    void data(Xany const &x) { data_ = x; }
  };
}

void gott::metadata::detail::update_module_resource(
    istream &stream,
    string const &resource,
    transaction &tr) {
  tr.remove_resource(resource);
  /*
  ordered
    named (module-id), node
    unordered
      named (file-path), node
      named (module-type), enumeration $ dynamic-native
   */
  rule_t const module_schema = 
    rule("tdl::schema::ordered", rule_attr(coat = false),
        boost::assign::list_of
        (rule_one("tdl::schema::named", rule_attr(tag = "module-id"),
                  rule("tdl::schema::node")))
        (rule("tdl::schema::unordered", rule_attr(coat = false),
          boost::assign::list_of
          (rule_one("tdl::schema::named", rule_attr(tag = "file-path"),
                    rule("tdl::schema::node")))
          (rule_one("tdl::schema::named", rule_attr(tag = "module-type"),
                    rule("tdl::schema::node", rule_attr(
                      tdl::schema::repatcher = new repatch_enumeration(
                        boost::assign::list_of
                        (string("dynamic-native"))
                        .operator std::vector<string>())))))
          (rule_one("tdl::schema::named",
                    rule_attr(tag = "depend-on", outer = list()),
                    rule("tdl::schema::node"))))));

  struct multi_accepter : writable_structure {
    multi_accepter(string const &resource, transaction &tr)
      : resource(resource), tr(tr), level(0) {}

    string const &resource;
    transaction &tr;
    unsigned level;
    module_accepter inner;

    void begin(tdl::source_position const &w) {
      if (level == 0)
        inner = module_accepter();
      else
        inner.begin(w);
      ++level;
    }
    void end() {
      --level;
      if (level == 0)
        tr.add_module(
            inner.module_id,
            inner.module_type,
            inner.file_path,
            inner.dependencies,
            resource);
      else
        inner.end();
    }

    void data(Xany const &x) { inner.data(x); }
    void add_tag(string const &s) { inner.add_tag(s); }
  };
  multi_accepter out(resource, tr);
  revocable_adapter adapter(out);
  match(
      rule_one("tdl::schema::document", rule_attr(coat = false),
        rule_one("tdl::schema::ordered", rule_attr(outer = list()),
          module_schema)),
      adapter).parse(stream);
}

namespace {
  class interface_accepter : public writable_structure {
  public:
    QID interface_id;

    string tag_;
    Xany data_;

    void begin(tdl::source_position const &) {}
    void end() {
      if (tag_ == "interface-id")
        interface_id = Xany_cast<string>(data_);
      tag_ = string();
    }
    void data(Xany const &x) { data_ = x; }
    void add_tag(string const &x) { tag_ = x; }
  };
}

void gott::metadata::detail::update_interface_resource(
    istream &stream,
    string const &resource,
    transaction &tr) {
  tr.remove_resource(resource);
  rule_t const interface_schema =
    rule_one("tdl::schema::named", rule_attr(tag = "interface-id"),
        rule("tdl::schema::node"));

  struct multi_accepter : writable_structure {
    multi_accepter(string const &resource, transaction &tr)
      : resource(resource), tr(tr), level(0) {}

    string const &resource;
    transaction &tr;
    unsigned level;
    interface_accepter inner;

    void begin(tdl::source_position const &w) {
      if (level == 0)
        inner = interface_accepter();
      else
        inner.begin(w);
      ++level;
    }
    void end() {
      --level;
      if (level == 0)
        tr.add_interface(inner.interface_id, resource);
      else
        inner.end();
    }

    void data(Xany const &x) { inner.data(x); }
    void add_tag(string const &s) { inner.add_tag(s); }
  };
  multi_accepter out(resource, tr);
  revocable_adapter adapter(out);
  match(
      rule_one("tdl::schema::document", rule_attr(coat = false),
        rule_one("tdl::schema::ordered", rule_attr(outer = list()),
          interface_schema)),
      adapter).parse(stream);
}
