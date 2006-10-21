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
#include <gott/tdl/exceptions.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/plugin/plugin_builder.hpp>

using gott::string;
using namespace tdl::structure;
using tdl::tdl_error;
using tdl::source_position;

namespace {

class repatch_throw_away 
  : public concrete_repatcher<repatch_throw_away> {
public:
  repatch_throw_away();
  ~repatch_throw_away();
  writable_structure *deferred_write(writable_structure &) const;
};
  
}

repatch_throw_away::repatch_throw_away() {}
repatch_throw_away::~repatch_throw_away() {}

writable_structure *
repatch_throw_away::deferred_write(writable_structure &) const {
  struct context : writable_structure {
    void data(gott::xany::Xany const &) {}
    void add_tag(gott::string const &) {}
    void begin(tdl::source_position const &) {}
    void end() {}
  };
  return new context;
}

namespace {
class factory : public repatcher_getter_factory {
  struct getter : public repatcher_getter {
    getter() {}
    void begin(source_position const &) { fail(); }
    void end() { fail(); }
    void data(gott::xany::Xany const &) { fail(); }
    void add_tag(string const &) { fail(); }
    void fail() {
      throw tdl_error("TDL Structure repatcher loader",
          "non-sensible arguments");
    }
    repatcher *result_alloc() const { return new repatch_throw_away(); }
    static repatcher_getter *alloc() { return new getter; }
  };
public:
  factory() {}
  gott::atom get_id() const { return gott::atom("throw-away"); }
  repatcher_getter *alloc() const { return new getter; }
  tdl::schema::rule_t parameter_schema() const {
    return tdl::schema::rule("ordered");
  }
};
}

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(
    plugin_repatcher_throw_away,
    factory)
