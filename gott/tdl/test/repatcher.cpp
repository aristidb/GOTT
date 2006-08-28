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

#include <gott/tdl/source_position.hpp>
#include <gott/tdl/structure/repatch.hpp>
#include <gott/tdl/structure/repatchers/find_literal.hpp>
#include <gott/tdl/structure/repatchers/substring.hpp>
#include <gott/tdl/structure/repatchers/integer.hpp>
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <gott/tdl/structure/print.hpp>
#include <gott/tdl/exceptions.hpp>
#include <gott/tut/tut.h>
#include <sstream>
#include <boost/scoped_ptr.hpp>

using namespace tdl::structure;
using tdl::source_position;
using gott::Xany;

using boost::scoped_ptr;

namespace tut {
struct repatcher_test { };

typedef test_group<repatcher_test> tf;
typedef tf::object object;
}

namespace {
tut::tf stru("repatcher");

repatcher *strip_paren() {
  repatcher_chain *result = new repatcher_chain;
  result->push_back(
      repatch_find_literal(repatch_find_literal::type::start, "("));
  result->push_back(
      repatch_find_literal(repatch_find_literal::type::end, ")"));
  result->push_back(repatch_substring(1, -1));
  return result;
}
}

namespace tut {
template<> template<>
void object::test<1>(int) {
  std::ostringstream o;
  direct_print out(o);
  scoped_ptr<repatcher> re(strip_paren());
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin(source_position());
  ind->data(Xany("(hallo)"));
  ind->end();
  ensure_equals(o.str(), "hallo");
}

template<> template<>
void object::test<2>(int) {
  std::ostringstream o;
  direct_print out(o);
  scoped_ptr<repatcher> re(strip_paren());
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin(source_position());
  try {
    ind->data(Xany("(hallo"));
  } catch (tdl::tdl_error const &e) {
    ensure_equals(e.module(), "TDL Structure repatcher");
    ensure_equals(o.str(), "");
  }
  ind->end();
}

template<> template<>
void object::test<3>(int) {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name().get_alloc("integer"));
  scoped_ptr<repatcher> re(re_g->result_alloc());

  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));

  ind->begin(source_position());
  ind->data(Xany("-000044"));
  ind->end();

  ensure_equals(stream.str(), "-44");
}

template<> template<>
void object::test<4>(int) {
  scoped_ptr<repatcher_getter> re_g(
      repatcher_by_name().get_alloc("enumeration"));
  re_g->begin(source_position()); re_g->data(Xany("v1")); re_g->end();
  re_g->begin(source_position()); re_g->data(Xany("v2")); re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());

  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));

  ind->begin(source_position());
    ind->begin(source_position());
      ind->data(Xany("v1"));
    ind->end();
    ind->begin(source_position());
      ind->data(Xany("v2"));
    ind->end();
  ind->end();

  ensure_equals(stream.str(), "    0\n    1");
}

template<> template<>
void object::test<5>(int) {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name().get_alloc("substring"));
  re_g->begin(source_position());
    re_g->add_tag("left");
    re_g->data(Xany(2));
  re_g->end();
  re_g->begin(source_position());
    re_g->data(Xany(3));
    re_g->add_tag("right");
  re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());

  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin(source_position());
    ind->data(Xany("123456789"));
  ind->end();

  ensure_equals(stream.str(), "3");
}

template<> template<>
void object::test<6>(int) {
  scoped_ptr<repatcher_getter> re_g(
      repatcher_by_name().get_alloc("find-literal"));
  re_g->begin(source_position()); re_g->data(Xany("foo")); re_g->end();
  re_g->begin(source_position()); re_g->data(Xany(repatch_find_literal::type::end)); re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());

  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin(source_position()); 
  ind->begin(source_position()); ind->data(Xany("barfoo")); ind->end();
  ind->end();
  ensure_equals(stream.str(), "    barfoo");

  bool expected_failure = false;
  ind->begin(source_position());
    ind->begin(source_position());
      try {
        ind->data(Xany("foobar")); 
      } catch (tdl::tdl_error const &e) {
        ensure_equals(e.module(), "TDL Structure repatcher");
        expected_failure = true;
      }
    ind->end();
  ind->end();

  if (!expected_failure)
    fail();
}

template<> template<>
void object::test<7>(int) {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name().chain_alloc());
  re_g->begin(source_position());
    re_g->data(Xany("substring"));
    re_g->begin(source_position());
      re_g->begin(source_position());
        re_g->add_tag("left");
        re_g->data(Xany(1));
      re_g->end();
      re_g->begin(source_position());
        re_g->add_tag("right");
        re_g->data(Xany(0));
      re_g->end();
    re_g->end();
  re_g->end();
  re_g->begin(source_position());
    re_g->data(Xany("integer"));
    re_g->begin(source_position()); re_g->end();
  re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());

  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin(source_position());
    ind->data(Xany("x077"));
  ind->end();

  ensure_equals(stream.str(), "77");
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

//...
}
