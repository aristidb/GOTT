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

#include "../repatch.hpp"
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/slot.hpp>
#include <gott/tdl/exceptions.hpp>
#include <gott/range_algo.hpp>
#include <gott/xany.hpp>
#include <gott/plugin/plugin_builder.hpp>

using namespace tdl::structure;
using tdl::tdl_error;
using tdl::source_position;
using gott::string;
using gott::Xany;
using gott::Xany_cast;

namespace {

class repatch_enumeration 
: public concrete_repatcher<repatch_enumeration> {
public:
  repatch_enumeration(std::vector<gott::string> const &);
  ~repatch_enumeration();
  writable_structure *deferred_write(writable_structure &) const;
  writable_structure *inverse(writable_structure &) const;
private:
  std::vector<gott::string> alternatives;
};
  
}

repatch_enumeration::repatch_enumeration(std::vector<string> const &x) 
: alternatives(x) {}

repatch_enumeration::~repatch_enumeration() {}

const char failure_message[] = 
    "repatch_enumeration: could not match enumeration";

writable_structure *
repatch_enumeration::deferred_write(writable_structure &s) const {
  struct context : simple_repatcher_context {
    std::vector<string> const &alternatives;

    context(writable_structure &s, std::vector<string> const &a) 
    : simple_repatcher_context(s), alternatives(a) {}

    void data(Xany const &x) {
      if (x.compatible<string>()) {
        string input = Xany_cast<string>(x);
        std::vector<string>::const_iterator it;
        if ((it = find(range(alternatives), input)) == alternatives.end())
          throw tdl_error("TDL Structure repatcher", "not in enumeration");
        target.data(Xany(long(it - alternatives.begin())));
      } else
        throw tdl_error("TDL Structure repatcher", 
            "enumeration needs input string");
    }
  };
  return new context(s, alternatives);
}

writable_structure *
repatch_enumeration::inverse(writable_structure &s) const {
  struct context : simple_repatcher_context {
    std::vector<string> const &alternatives;

    context(writable_structure &s, std::vector<string> const &a) 
    : simple_repatcher_context(s), alternatives(a) {}

    void data(Xany const &x) {
      if (x.compatible<long>()) {
        long input = Xany_cast<long>(x);
        if (input < 0 || input > long(alternatives.size()))
          throw tdl_error("TDL Structure repatcher", "not in enumeration");
        target.data(Xany(alternatives[input]));
      } else
        throw tdl_error("TDL Structure repatcher", 
            "enumeration (reverse) needs input ID");
    }
  };
  return new context(s, alternatives);
}

namespace {
class factory : public repatcher_getter_factory {
  struct getter : public repatcher_getter {
    getter() : inner(false) {}
    bool inner;
    std::vector<string> all_strings;
    void begin(source_position const &) {
      if (inner) fail();
      inner = true;
    }
    void end() {
      if (!inner) fail();
      inner = false;
    }
    void data(Xany const &x) {
      if (!inner) fail();
      all_strings.push_back(Xany_cast<string>(x));
    }
    void add_tag(string const &) {}
    void fail() {
      throw tdl_error("TDL Structure repatcher loader",
          "non-sensible arguments");
    }
    repatcher *result_alloc() const {
      return new repatch_enumeration(all_strings);
    }
  };
public:
  factory() {}
  gott::atom get_id() const { return gott::atom("enumeration"); }
  repatcher_getter *alloc() const {
    return new getter;
  }
  tdl::schema::rule_t parameter_schema() const {
    using namespace tdl::schema;
    return rule_one("list", rule_attr(),
        rule("node", rule_attr(outer = some())));
  }
};

}

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(
    plugin_repatcher_enumeration,
    factory)
