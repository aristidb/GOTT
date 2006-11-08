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
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/happy_once.hpp>
#include <gott/tdl/schema/type.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <gott/tdl/schema/event.hpp>
#include <gott/tdl/structure/repatch.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream> //FIXME

using namespace tdl::schema;
namespace structure = tdl::structure;
using structure::writable_structure;
using structure::repatcher_getter_factory;
using structure::repatcher_getter;

namespace {

struct make_rep_schema {
  std::vector<rule_t> &out;

  void operator() (repatcher_getter_factory const *rgf) const {
    out.push_back(
        rule_one("named",
          rule_attr(tag = rgf->get_id().get_string(), outer = list()),
          rgf->parameter_schema()));
  }
};

struct rep_rep : structure::concrete_repatcher<rep_rep> {
  rep_rep(repatcher_getter *helper) : helper(helper) {}

  repatcher_getter *helper;

  writable_structure *deferred_write(writable_structure &) const {
    struct context : writable_structure {
      context(repatcher_getter *helper) : helper(helper) {}

      repatcher_getter *helper;

      void begin(tdl::source_position const &w) {
        helper->begin(w);
      }

      void end() {
        helper->end();
      }

      void add_tag(gott::string const &s) {
        helper->add_tag(s);
      }

      void data(gott::Xany const &d) {
        helper->data(d);
      }
    };

    return new context(helper);
  }
};

class xmatcher : public happy_once {
public:
  xmatcher(
      rule_attr_t const &attr,
      std::vector<rule_t> const &children,
      match &ref)
  :     happy_once(attr, ref),
        getter(structure::repatcher_by_name()) {
    (void)children; //TODO: check that there are _no_ children

    std::vector<rule_t> inner;
    make_rep_schema helper = { inner };
    tdl::resource::list<repatcher_getter_factory>(helper);
    ref.add(rule("unordered",
          rule_attr(coat = false, repatcher2 = new rep_rep(getter.get())),
          inner));
  }

  static bool accept_empty(rule_attr_t const &, std::vector<rule_t> const &) 
  { return true; }

  static gott::atom const id;

private:
  bool play(ev::child_succeed const &) {
    matcher().out_structure().begin(matcher().where_out());
    matcher().out_structure().data(gott::Xany(
          boost::shared_ptr<structure::repatcher>(            getter->result_alloc())));
    matcher().out_structure().end();
    be_happy();
    return true;
  }

  gott::string name() const {
    return id.get_string();
  }

  rule_t param;
  boost::scoped_ptr<repatcher_getter> getter;
};
}

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(plugin_lang_repatcher, concrete_type<xmatcher>)

gott::atom const xmatcher::id("tdl::schema_lang::repatcher");

