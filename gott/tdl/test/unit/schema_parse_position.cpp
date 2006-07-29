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

#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/schema/parse_position.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <gott/tdl/structure/structure.hpp>
#include <gott/tdl/parse/parser.hpp>
#include <gott/tut/tut.h>
#include <gott/string/string.hpp>

namespace {
struct ignore_struct : tdl::structure::revocable_structure {
  void data(gott::xany::Xany const &) {}
  void begin(tdl::source_position const &) {}
  void end() {}
  void add_tag(gott::string const &) {}

  void add_repatcher(
      boost::shared_ptr<tdl::structure::repatcher const> const &) {}
  void remove_repatcher(
      boost::shared_ptr<tdl::structure::repatcher const> const &) {}

  pth point() { return pth(); }
  void revert(pth) {}
  void get_rid_of(pth) {}
};

struct test_base {
  ignore_struct no_stru;
  tdl::schema::match match;
  tdl::schema::positioning &pos;
  tdl::abstract_tdl_parser &parse;

  test_base() 
  : match(no_stru), pos(match.pos()), parse(match) {
    match.add(tdl::schema::rule_one("tdl::schema::document", tdl::schema::rule_attr_t(), 
     tdl::schema::rule("tdl::schema::node", tdl::schema::rule_attr_t())));
  }
};

typedef tut::test_group<test_base> tf;
typedef tf::object object;

tf X("schema::parse_position");
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  parse.begin_parse();
  ensure_equals("first token index", pos.current().first, 0);
}

template<> template<>
void object::test<2>(int) {
  no_test();
}
}
