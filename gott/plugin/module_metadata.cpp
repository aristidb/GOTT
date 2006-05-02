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

#include "module_metadata.hpp"
#include "metadata.hpp"
#include "validate.hpp"
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/by_name.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <gott/tdl/schema/slot.hpp>
#include <gott/tdl/structure/revocable_adapter.hpp>
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <gott/tdl/write/writer.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/thread.hpp>
#include <vector>

using namespace tdl::schema;
using namespace tdl::structure;
using namespace gott::xany;
using namespace gott::plugin;
using namespace gott;
using std::istream;
using std::ostream;

bool module_metadata::is_valid() const {
  if (!validation)
    validation = detail::validate_metadata(*this);
  return *validation;
}

namespace {
  static boost::recursive_mutex metadata_biglock;
  #define BIGLOCK boost::recursive_mutex::scoped_lock B_lock(metadata_biglock)

  typedef std::vector<module_metadata> module_metadata_list_t;
  static module_metadata_list_t known_module_metadata;

  /*
  ordered
    named (module-id), node
    unordered
      named (file-path), node
      named (module-type), enumeration $ dynamic-native
   */
  static rule_t metadata_schema = 
    rule("ordered", rule_attr(coat = false),
        boost::assign::list_of
        (rule_one("named", rule_attr(tag = "module-id"), rule("node")))
        (rule("unordered", rule_attr(coat = false),
          boost::assign::list_of
          (rule_one("named", rule_attr(tag = "file-path"), rule("node")))
          (rule_one("named", rule_attr(tag = "module-type"),
                    rule("node", rule_attr(
                      tdl::schema::repatcher = new repatch_enumeration(
                        std::vector<string>(1, "dynamic-native")))))))));

  struct accepter : writable_structure {
    accepter(module_metadata &ref) : ref(ref) {}

    module_metadata &ref;

    string tag;
    Xany data_;

    void begin(tdl::source_position const &) {}
    void end() {
      if (tag == "module-id")
        ref.module_id = Xany_cast<string>(data_);
      else if (tag == "file-path")
        ref.file_path = Xany_cast<string>(data_);
      else if (tag == "module-type")
        ref.module_type = Xany_cast<module_metadata::module_type_t>(data_);
      tag = string();
    }

    void add_tag(string const &t) { tag = t; }
    void data(Xany const &x) { data_ = x; }
  };
}

void gott::plugin::enumerate_module_metadata_p(
    boost::function<void (module_metadata const &)> const &callback,
    boost::optional<QID const &> const &module_id,
    bool cancel_early,
    bool do_load_standard_metadata,
    bool validate) {
  if (do_load_standard_metadata)
    load_standard_metadata();
  BIGLOCK;
  for (module_metadata_list_t::iterator it = known_module_metadata.begin();
      it != known_module_metadata.end();
      ++it) {
    if (module_id && it->module_id != *module_id)
      continue;
    if (validate && !it->is_valid())
      continue;
    callback(*it);
    if (cancel_early)
      return;
  }
}

void gott::plugin::add_module_metadata(module_metadata const &metadata) {
  BIGLOCK;
  known_module_metadata.push_back(metadata);
}

void gott::plugin::clear_module_metadata() {
  BIGLOCK;
  module_metadata_list_t().swap(known_module_metadata);
}

void gott::plugin::extract_module_metadata(istream &stream) {
  struct multi_accepter : writable_structure {
    multi_accepter() : level(0), inner(current) {}

    unsigned level;
    module_metadata current;
    accepter inner;

    void begin(tdl::source_position const &w) {
      if (level == 0) {
        current = module_metadata();
      } else
        inner.begin(w);
      ++level;
    }
    void end() {
      --level;
      if (level == 0) {
        add_module_metadata(current);
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

istream &gott::plugin::operator>>(istream &stream, module_metadata &out_value) {
  out_value = module_metadata();
  accepter out(out_value);
  revocable_adapter adapter(out);
  match(
      rule_one("document", rule_attr(coat = false), metadata_schema),
      adapter)
    .parse(stream);
  return stream;
}

ostream &gott::plugin::operator<<(ostream &stream, module_metadata const &val) {
  tdl::tdl_writer w(stream, 2);
  w.down();
  {
    w.node("module-id"); 
    w.down();
      w.node(val.module_id);
    w.up();
  }
  {
    w.node("file-path");
    w.down();
      w.node(val.file_path);
    w.up();
  }
  {
    w.node("module-type");
    w.down();
      switch (val.module_type) {
      case module_metadata::dynamic_native:
        w.node("dynamic-native");
        break;
      }
    w.up();
  }
  w.up();
  return stream;
}
