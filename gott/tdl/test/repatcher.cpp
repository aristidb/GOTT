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

#include <gott/tdl/structure/repatch.hpp>
#include <gott/tdl/structure/print.hpp>
#include <gott/tdl/exceptions.hpp>
#include <sstream>
#include <boost/scoped_ptr.hpp>
#include <testsoon.hpp>

using namespace tdl::structure;
using gott::Xany;

using boost::scoped_ptr;

namespace {
repatcher *strip_paren() {
  boost::scoped_ptr<repatcher_getter> rg(repatcher_by_name());
  rg->begin();
    rg->data(Xany("find-literal"));
    rg->begin();
      rg->begin(); rg->data(Xany(0)); rg->end(); // start
      rg->begin(); rg->data(Xany("(")); rg->end();
    rg->end();
  rg->end();
  rg->begin();
    rg->data(Xany("find-literal"));
    rg->begin();
      rg->begin(); rg->data(Xany(1)); rg->end(); // end
      rg->begin(); rg->data(Xany(")")); rg->end();
    rg->end();
  rg->end();
  rg->begin();
    rg->data(Xany("substring"));
    rg->begin();
      rg->begin();
        rg->add_tag("left");
        rg->data(Xany(1));
      rg->end();
      rg->begin();
        rg->add_tag("right");
        rg->data(Xany(-1));
      rg->end();
    rg->end();
  rg->end();
  
  return rg->result_alloc();
}
}

TEST() {
  std::ostringstream o;
  direct_print out(o);
  scoped_ptr<repatcher> re(strip_paren());
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin();
  ind->data(Xany("(hallo)"));
  ind->end();
  Equals(o.str(), "hallo");
}

TEST() {
  std::ostringstream o;
  direct_print out(o);
  scoped_ptr<repatcher> re(strip_paren());
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin();
  try {
    ind->data(Xany("(hallo"));
  } catch (tdl::tdl_error const &e) {
    Equals(e.module(), "TDL Structure repatcher");
    Equals(o.str(), "");
  }
  ind->end();
}

TEST() {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name());
  re_g->begin();
  re_g->data(Xany("integer"));
  re_g->begin(); re_g->end();
  re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());

  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));

  ind->begin();
  ind->data(Xany("-000044"));
  ind->end();

  Equals(stream.str(), "-44");
}

TEST() {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name());
  re_g->begin();
  re_g->data(Xany("enumeration"));
  re_g->begin();
  re_g->begin(); re_g->data(Xany("v1")); re_g->end();
  re_g->begin(); re_g->data(Xany("v2")); re_g->end();
  re_g->end();
  re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());

  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));

  ind->begin();
    ind->begin();
      ind->data(Xany("v1"));
    ind->end();
    ind->begin();
      ind->data(Xany("v2"));
    ind->end();
  ind->end();

  Equals(stream.str(), "    0\n    1");
}

TEST() {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name());
  re_g->begin();
    re_g->data(Xany("substring"));
    re_g->begin();
      re_g->begin();
        re_g->add_tag("left");
        re_g->data(Xany(2));
      re_g->end();
      re_g->begin();
        re_g->data(Xany(3));
        re_g->add_tag("right");
      re_g->end();
    re_g->end();
  re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());

  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin();
    ind->data(Xany("123456789"));
  ind->end();

  Equals(stream.str(), "3");
}

TEST() {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name());
  re_g->begin();
    re_g->data(Xany("find-literal"));
    re_g->begin();
      re_g->begin();
        re_g->data(Xany("foo"));
      re_g->end();
      re_g->begin();
        re_g->data(Xany(1)); // end
      re_g->end();
    re_g->end();
  re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());

  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin(); 
  ind->begin(); ind->data(Xany("barfoo")); ind->end();
  ind->end();
  Equals(stream.str(), "    barfoo");

  bool expected_failure = false;
  ind->begin();
    ind->begin();
      try {
        ind->data(Xany("foobar")); 
      } catch (tdl::tdl_error const &e) {
        Equals(e.module(), "TDL Structure repatcher");
        expected_failure = true;
      }
    ind->end();
  ind->end();

  if (!expected_failure)
    Check(false);
}

TEST() {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name());
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

  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  ind->begin();
    ind->data(Xany("x077"));
  ind->end();

  Equals(stream.str(), "77");
}

TEST_GROUP(integer_ext) {
namespace {
struct helper : writable_structure {
  helper(long val) : val(val), found(false) {}
  ~helper() {}

  long const val;
  bool found;

  void begin(tdl::source_position const &) {}
  void end() {}
  void add_tag(gott::string const &) {}
  void data(Xany const &x) {
    Check(!found);
    Equals(gott::Xany_cast<long>(x), val);
    found = true;
  }
};

void check(gott::string const &in, long val) {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name());
  re_g->begin();
    re_g->data(Xany("integer"));
  re_g->begin(); re_g->end();
  re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());

  helper out(val);

  scoped_ptr<writable_structure> ind(re->deferred_write(out));
  Nothrows(ind->data(Xany(in)), tdl::tdl_error&);

  Check(out.found);
}

void yeah(std::ostringstream &stream, long value) {
  if (value < 0)
    stream << -value;
  else
    stream << value;
  Check(stream);
  check(gott::string(stream.str(), gott::ascii), value);
}

typedef testsoon::range_generator<long> rng;
}

XTEST((name, "simple")(generator, (rng)(-20)(20))) {
  std::ostringstream stream;
  if (value < 0)
    stream << '-';
  yeah(stream, value);
}

XTEST((name, "dec")(generator, (rng)(-20)(20))) {
  std::ostringstream stream;
  if (value < 0)
    stream << '-';
  stream << "0d";
  yeah(stream, value);
}

XTEST((name, "hex")(generator, (rng)(-20)(20))) {
  std::ostringstream stream;
  if (value < 0)
    stream << '-';
  stream << "0x" << std::hex;
  yeah(stream, value);
}

XTEST((name, "oct")(generator, (rng)(-20)(20))) {
  std::ostringstream stream;
  if (value < 0)
    stream << '-';
  stream << "0c" << std::oct;
  yeah(stream, value);
}

XTEST((name, "bin")(generator, (rng)(-20)(20))) {
  std::ostringstream stream;
  unsigned long val = value;
  if (value < 0) {
    stream << '-';
    val = -value;
  }
  stream << "0b";
  for (int i = 31; i >= 0; --i)
    stream << ((val >> i) & 1);
  Check(stream);
  check(gott::string(stream.str(), gott::ascii), value);
}

XTEST((name, "bin2")(generator, (rng)(-20)(20))) {
  std::ostringstream stream;
  unsigned long val = value;
  if (value < 0) {
    stream << '-';
    val = -value;
  }
  stream << "0b";
  bool lead = true;
  for (int i = 31; i >= 0; --i) {
    bool x = (val >> i) & 1;
    if (x)
      lead = false;
    if (!lead)
      stream << x;
  }
  if (lead)
    stream << '0';
  Check(stream);
  check(gott::string(stream.str(), gott::ascii), value);
}

}

TEST_GROUP(inverse) {

TEST(nothing) {
  repatch_nothing re;
  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re.inverse(out));
  ind->begin();
    ind->data(Xany("test"));
  ind->end();

  Equals(stream.str(), "test");
}

TEST(empty chain) {
  repatcher_chain rec;
  rec.push_back_alloc(new repatch_nothing);
  rec.push_back_alloc(new repatch_nothing);
  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(rec.inverse(out));
  ind->begin();
    ind->data(Xany("test"));
  ind->end();

  Equals(stream.str(), "test");
}

TEST(integer (poor)) {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name());
  re_g->begin();
    re_g->data(Xany("integer"));
  re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());
  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re->inverse(out));
  Check(ind);
  ind->begin();
    ind->data(Xany(-4));
  ind->end();

  Equals(stream.str(), "-4");
}

TEST(strip paren) {
  std::ostringstream o;
  direct_print out(o);
  scoped_ptr<repatcher> re(strip_paren());
  scoped_ptr<writable_structure> ind(re->inverse(out));
  Check(ind);
  ind->begin();
    ind->data(Xany("hallo"));
  ind->end();
  Equals(o.str(), "(hallo)");
}

TEST(enumeration) {
  scoped_ptr<repatcher_getter> re_g(repatcher_by_name());
  re_g->begin();
  re_g->data(Xany("enumeration"));
  re_g->begin();
  re_g->begin(); re_g->data(Xany("v1")); re_g->end();
  re_g->begin(); re_g->data(Xany("v2")); re_g->end();
  re_g->end();
  re_g->end();
  scoped_ptr<repatcher> re(re_g->result_alloc());

  std::ostringstream stream;
  direct_print out(stream);
  scoped_ptr<writable_structure> ind(re->inverse(out));
  Check(ind);

  ind->begin();
    ind->data(Xany(0));
  ind->end();
  ind->begin();
    ind->data(Xany(1));
  ind->end();

  Equals(stream.str(), "v1\nv2");
}

}

