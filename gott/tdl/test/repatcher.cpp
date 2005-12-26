// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Testing
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <gott/tdl/structure/repatch.hpp>
#include <gott/tdl/structure/repatcher_by_name.hpp>
#include <gott/tdl/structure/repatchers/find_literal.hpp>
#include <gott/tdl/structure/repatchers/substring.hpp>
#include <gott/tdl/structure/repatchers/integer.hpp>
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <gott/tdl/structure/print.hpp>
#include <gott/tdl/exceptions.hpp>
#include <gott/tut/tut.h>
#include <iostream>
#include <sstream>
#include <boost/scoped_ptr.hpp>

using namespace gott::tdl::structure;
using namespace gott::xany;
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
  direct_print<char> out(o);
  scoped_ptr<repatcher> re(strip_paren());
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin();
  ind->data(Xany("(hallo)"));
  ind->end();
  ensure_equals(o.str(), "hallo");
}

template<> template<>
void object::test<2>(int) {
  std::ostringstream o;
  direct_print<char> out(o);
  scoped_ptr<repatcher> re(strip_paren());
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin();
  try {
    ind->data(Xany("(hallo"));
  } catch (failed_repatch const &e) {
    ensure_equals(std::string(e.what()), 
        "repatch_find_literal: condition not true");
    ensure_equals(o.str(), "");
  }
  ind->end();
}

template<> template<>
void object::test<3>(int) {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name().get_alloc("integer"));
  scoped_ptr<repatcher> re(re_g->result_alloc());
  direct_print<char> out(std::cout);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin();
  ind->data(Xany("-44"));
  ind->end();
}

template<> template<>
void object::test<4>(int) {
  scoped_ptr<repatcher_getter> re_g(
      repatcher_by_name().get_alloc("enumeration"));
  re_g->begin(); re_g->data(Xany("v1")); re_g->end();
  re_g->begin(); re_g->data(Xany("v2")); re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());
  direct_print<char> out(std::cout);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin();
    ind->begin();
    ind->data(Xany("v1"));
    ind->end();
    ind->begin();
    ind->data(Xany("v2"));
    ind->end();
  ind->end();
}

template<> template<>
void object::test<5>(int) {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name().get_alloc("substring"));
  re_g->begin();
    re_g->add_tag("left");
    re_g->data(Xany(2));
  re_g->end();
  re_g->begin();
    re_g->data(Xany(3));
    re_g->add_tag("right");
  re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());
  direct_print<char> out(std::cout);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin();
    ind->data(Xany("123456789"));
  ind->end();
}

template<> template<>
void object::test<6>(int) {
  scoped_ptr<repatcher_getter> re_g(
      repatcher_by_name().get_alloc("find_literal"));
  re_g->begin(); re_g->data(Xany("foo")); re_g->end();
  re_g->begin(); re_g->data(Xany(repatch_find_literal::type::end)); re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());
  direct_print<char> out(std::cout);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin(); 
  ind->begin(); ind->data(Xany("barfoo")); ind->end();

  bool expected_failure = false;
  ind->begin();
      try {
      ind->data(Xany("foobar")); 
      } catch (failed_repatch const &) {
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
  re_g->begin();
    re_g->data(Xany("substring"));
    re_g->begin();
      re_g->begin();
        re_g->add_tag("left");
        re_g->data(Xany(1));
      re_g->end();
      re_g->begin();
        re_g->add_tag("right");
        re_g->data(Xany(0));
      re_g->end();
    re_g->end();
  re_g->end();
  re_g->begin();
    re_g->data(Xany("integer"));
    re_g->begin(); re_g->end();
  re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());
  direct_print<char> out(std::cout);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin();
    ind->data(Xany("x077"));
  ind->end();
}

template<> template<>
void object::test<8>(int) {
  no_test();
}

//...
}