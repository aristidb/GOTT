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
#include <gott/tdl/schema/item.hpp>
#include <gott/tdl/schema/happy_once.hpp>
#include <gott/tdl/schema/type.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <gott/tdl/structure/repatch.hpp>
#include <gott/tdl/structure/repatcher_by_name.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>//FIXME

using namespace tdl::schema;
namespace structure = tdl::structure;
using structure::writable_structure;

namespace {
class repatcher_maker : public structure::concrete_repatcher<repatcher_maker> {
  writable_structure *deferred_write(writable_structure &target) const {
    class context : public writable_structure {
    public:
      context(writable_structure &target)
      : target(target),
        getter(structure::repatcher_by_name().chain_alloc()),
        level(0) {}

    private:
      void begin(tdl::source_position const &w) {
        last_position = w;
        if (level++ > 0)
          getter->begin(w);
        std::cout << "repatcher_maker/begin:" << level << std::endl;
      }

      void end() {
        std::cout << "repatcher_maker/end:" << level << std::endl;
        if (--level > 0)
          getter->end();
        else {
          target.begin(last_position);
            target.add_tag("repatcher");
            target.data(gott::Xany(boost::shared_ptr<structure::repatcher>(
                    getter->result_alloc())));
          target.end();
          getter.reset(structure::repatcher_by_name().chain_alloc());
        }
      }

      void data(gott::xany::Xany const &x) {
        getter->data(x);
      }

      void add_tag(gott::string const &x) {
        getter->add_tag(x);
      }

      writable_structure &target;
      boost::scoped_ptr<structure::repatcher_getter> getter;
      unsigned long level;
      tdl::source_position last_position;
    };
    return new context(target);
  }
};

class matcher : public happy_once {
public:
  matcher(
      rule_attr_t const &attr,
      std::vector<rule_t> const &children,
      match &ref)
  : happy_once(attr, ref) {
    ref.add(
      rule_one("tdl::schema::list",
        rule_attr(repatcher = new repatcher_maker),
        rule("tdl::schema::tree", rule_attr(outer = list()))));
    (void)children; //TODO: check that there are _no_ children
  }

  static bool accept_empty(rule_attr_t const &, std::vector<rule_t> const &) 
  { return true; }

  bool play(ev::child_succeed const &) {
    be_happy();
    return true;
  }

  gott::string name() const {
    return gott::string("tdl::schema_lang::repatcher");
  }
};
}

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(plugin_lang_repatcher, concrete_type<matcher>)
