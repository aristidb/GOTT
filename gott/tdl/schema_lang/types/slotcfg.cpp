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
#include <iostream>

using namespace tdl::schema;
using tdl::structure::writable_structure;
using gott::string;
using gott::Xany;
using std::vector;

namespace {

slotcfg::simple_mode make_simple(long in);
slotcfg::sized_mode make_sized(long in);

struct rep_slot : tdl::structure::concrete_repatcher<rep_slot> {
  rep_slot(slotcfg *out) : out(out) {}
  slotcfg *out;

  writable_structure *deferred_write(writable_structure &) const {
    struct context : writable_structure {
      context(slotcfg *out) : out(out) {}
      slotcfg *out;

      enum { simple, sized, sized_param, range_param, range_param2 } state;
      slotcfg::sized_mode sized_mode;
      unsigned range_param1;

      void begin(tdl::source_position const &) {
      }

      void end() {
      }

      void data(Xany const &x) {
        long v = gott::Xany_cast<long>(x);
        switch (state) {
        case simple:
          *out = slotcfg(make_simple(v));
          break;
        case sized:
          sized_mode = make_sized(v);
          state = sized_param;
          break;
        case sized_param:
          *out = slotcfg(sized_mode, v);
          break;
        case range_param:
          range_param1 = v;
          state = range_param2;
          break;
        case range_param2:
          *out = slotcfg(slotcfg::range, range_param1, v);
          break;
        }
      }

      void add_tag(string const &s) {
        if (s == "simple")
          state = simple;
        else if (s == "sized")
          state = sized;
        else
          state = range_param;
      }
    };
    return new context(out);
  }
};

tdl::structure::repatcher *simple_modes() {
  boost::scoped_ptr<tdl::structure::repatcher_getter> g(
    tdl::structure::repatcher_by_name());
  g->begin();
    g->data(Xany("enumeration"));
    g->begin();
      g->begin(); g->data(Xany(":one")); g->end();
      g->begin(); g->data(Xany(":optional")); g->end();
      g->begin(); g->data(Xany(":some")); g->end();
      g->begin(); g->data(Xany(":list")); g->end();
    g->end();
  g->end();
  return g->result_alloc();
}

slotcfg::simple_mode make_simple(long in) {
  static slotcfg::simple_mode const table[] = {
    slotcfg::one,
    slotcfg::optional,
    slotcfg::some,
    slotcfg::list,
  };
  return table[in];
}

tdl::structure::repatcher *sized_modes() {
  boost::scoped_ptr<tdl::structure::repatcher_getter> g(
    tdl::structure::repatcher_by_name());
  g->begin();
    g->data(Xany("enumeration"));
    g->begin();
      g->begin(); g->data(Xany(":minimum")); g->end();
      g->begin(); g->data(Xany(":maximum")); g->end();
      g->begin(); g->data(Xany(":exactly")); g->end();
    g->end();
  g->end();
  return g->result_alloc();
}

slotcfg::sized_mode make_sized(long in) {
  static slotcfg::sized_mode const table[] = {
    slotcfg::minimum,
    slotcfg::maximum,
    slotcfg::exactly
  };
  return table[in];
}

tdl::structure::repatcher *range_mode() {
  boost::scoped_ptr<tdl::structure::repatcher_getter> g(
    tdl::structure::repatcher_by_name());
  g->begin();
    g->data(Xany("enumeration"));
    g->begin();
      g->begin(); g->data(Xany(":range")); g->end();
    g->end();
  g->end();
  g->begin();
    g->data(Xany("throw-away"));
  g->end();
  return g->result_alloc();
}

tdl::structure::repatcher *integer() {
  boost::scoped_ptr<tdl::structure::repatcher_getter> g(
    tdl::structure::repatcher_by_name());
  g->begin();
    g->data(Xany("integer"));
  g->end();
  return g->result_alloc();
}

class xmatcher : public happy_once {
public:
  xmatcher(rule_attr_t const &a, vector<rule_t> const &, match &m)
  : happy_once(a, m) {
    using boost::assign::list_of;
    m.add(
      rule("any", rule_attr(repatcher2 = new rep_slot(&out)), list_of
        (rule("node", rule_attr(tag = "simple", repatcher = simple_modes())))
        (rule("follow", rule_attr(tag = "sized"),
          list_of
          (rule("node", rule_attr(repatcher = sized_modes())))
          (rule("node", rule_attr(repatcher = integer())))
        ))
        (rule("follow", rule_attr(tag = "range"),
          list_of
          (rule("node", rule_attr(repatcher = range_mode())))
          (rule_one("list", rule_attr(),
            rule("node",
              rule_attr(repatcher = integer(), outer = exactly(2))
            )
          ))
        ))
      )
    );
  }

  static gott::atom const id;

  static bool accept_empty(rule_attr_t const &, std::vector<rule_t> const &)
  { return false; }

  string name() const { return id.get_string(); }

private:
  bool play(ev::child_succeed const &) {
    matcher().out_structure().begin();
      matcher().out_structure().data(Xany(out));
    matcher().out_structure().end();
    be_happy();
    return true;
  }

  slotcfg out;
};

}

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(plugin_lang_slotcfg, concrete_type<xmatcher>)

GOTT_PLUGIN_METADATA(
"plugin \"\"\n"
"interface tdl::resource\n"
"enclosing-module tdl::schema_lang\n"
"symbol plugin_lang_slotcfg\n\n"

"module tdl::schema_lang\n"
"file-path ??FILE??\n"
)

gott::atom const xmatcher::id("tdl::schema_lang::slotcfg");

