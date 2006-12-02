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

#include <testsoon.hpp>
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <gott/tdl/structure/revocable_adapter.hpp>
#include <gott/tdl/structure/repatchable_adapter.hpp>
#include <gott/tdl/structure/structure.hpp>
#include <gott/tdl/exceptions.hpp>
#include <boost/shared_ptr.hpp>
#include <sstream>

using tdl::structure::writable_structure;
using tdl::structure::repatcher;
using gott::string;
using boost::shared_ptr;

namespace {

struct acceptor : writable_structure {
  void begin(tdl::source_position const &) {
  }

  void end() {
  }

  void add_tag(gott::string const &) {
  }

  void data(gott::Xany const &x) {
    rep = gott::Xany_cast<shared_ptr<repatcher> >(x);
  }

  shared_ptr<repatcher> rep;
};

shared_ptr<repatcher> get_repatcher(string const &rep) {
  acceptor acc_r;
  tdl::structure::repatchable_adapter2 helper3(acc_r);
  tdl::structure::revocable_adapter helper2(helper3);
  tdl::structure::repatchable_adapter helper(helper2);
  using namespace tdl::schema;
  match m_r(
      rule_one("document", rule_attr(tag = "doc"),
        rule("tdl::schema_lang::repatcher", rule_attr(tag = "R"))),
      helper);
  std::stringstream rep_stream;
  rep_stream << rep;
  m_r.parse(rep_stream);  
  return acc_r.rep;
}

void perform(string const &rep, string const &data, writable_structure &out) {
  shared_ptr<repatcher> got_rep = get_repatcher(rep);
  Check(got_rep);

  tdl::structure::repatchable_adapter2 helper6(out);
  tdl::structure::revocable_adapter helper5(helper6);
  tdl::structure::repatchable_adapter helper4(helper5);
  using namespace tdl::schema;
  match m(
      rule_one("document",
        rule_attr(tag = "doc", tdl::schema::repatcher = got_rep),
        rule("node", rule_attr(tag = "E"))),
      helper4);
  std::stringstream data_stream;
  data_stream << data;
  m.parse(data_stream);
}

}

TEST_GROUP(init) {
  void check(string const &rep) {
    shared_ptr<repatcher> got_rep;
    Nothrows(got_rep = get_repatcher(rep), tdl::tdl_error&);
    Check(got_rep);
  }

  TEST(empty) {
    check("");
  }

  TEST(integer) {
    check("integer");
  }

  TEST(substring) {
    check("substring 1,1");
  }

  TEST(throw-away) {
    check("throw-away");
  }

  TEST(find-literal) {
    check("find-literal substring, foo");
  }
}

TEST_GROUP(bad) {
  void check(string const &rep) {
    Throws(get_repatcher(rep), tdl::tdl_error, "");
  }

  TEST(intxeger) {
    check("intxeger");
  }

  TEST(integer params) {
    check("integer 4");
  }
}
