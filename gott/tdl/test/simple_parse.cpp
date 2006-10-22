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
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
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

#include <gott/tdl/parse/parser.hpp>
#include <gott/tdl/exceptions.hpp>
#include <ostream>
#include <gott/string/string.hpp>
#include <testsoon.hpp>

using gott::string;

namespace {
struct group_fixture_t : tdl::abstract_tdl_parser {
  enum event_id { b, e, d, u, n, c };
  typedef std::pair<event_id, string> event;
  std::vector<event> xp, ev;

  void begin_parse() { ev.push_back(event(b, "")); }
  void end_parse() { ev.push_back(event(e, "")); }
  void down() { ev.push_back(event(d, "")); }
  void up() { ev.push_back(event(u, "")); }
  void node(string const &w) { ev.push_back(event(n, w)); }
  void comment(string const &w, bool nl) { 
    ev.push_back(event(c, (nl ? gott::string("\n") : gott::string()) + w)); 
  }

  void B() { xp.push_back(event(b, "")); }
  void E() { xp.push_back(event(e, "")); }
  void D() { xp.push_back(event(d, "")); }
  void U() { xp.push_back(event(u, "")); }
  void N(string const &w) { xp.push_back(event(n, w)); }
  void C(string const &w, bool nl) { 
    xp.push_back(event(c, (nl ? gott::string("\n") : "") + w)); 
  }
};

std::ostream &operator<<(std::ostream &o, group_fixture_t::event const &e) {
  return o << "bedunc"[e.first] << e.second;
}
}

#define B() group_fixture.B()
#define E() group_fixture.E()
#define D() group_fixture.D()
#define U() group_fixture.U()
#define N(x) group_fixture.N(x)
#define C(x, y) group_fixture.C(x, y)
#define CMP() Equals(range(group_fixture.ev), range(group_fixture.xp))

GFTEST(Empty document) {
  std::istringstream data("");
  group_fixture.parse(data);
  B(); D(); U(); E();
  CMP();
}

GFTEST(Very simple document) {
  std::istringstream data("hallodu");
  group_fixture.parse(data);
  B(); D(); N("hallodu"); U(); E();
  CMP();
}

GFTEST(One-line document with more than one element) {
  std::istringstream data("  hallodu, foobar zulu");
  group_fixture.parse(data);
  B(); D(); N("hallodu"); N("foobar"); D(); N("zulu"); U(); U(); E();
  CMP();
}

GFTEST(Empty multi-line document with funny spaces) {
  std::istringstream data("\n\n          \n     \n\n\n");
  group_fixture.parse(data);
  B(); D(); U(); E();
  CMP();
}

GFTEST(Two simple documents) {
  std::istringstream data("\n\n\nfoobar      \n***\n  zumwinke");
  group_fixture.parse(data);
  group_fixture.parse(data);

  B(); D(); N("foobar"); U(); E();
  B(); D(); N("zumwinke"); U(); E();

  CMP();
}

GFTEST(Indentation) {
  std::istringstream data("\n\n  \n       fobar\n");
  group_fixture.parse(data);
  B(); D(); N("fobar"); U(); E();

  CMP();
}

GFTEST(Indentation #2 (reduced)) {
  std::istringstream data("\n  x");
  group_fixture.parse(data);
  B(); D(); N("x"); U(); E();

  CMP();
}

GFTEST(Simple comment) {
  std::istringstream data("\n\n#__ mycomment,\n");
  group_fixture.parse(data);
  B(); D(); C("__ mycomment,", true); U(); E();

  CMP();
}

GFTEST(Newlined char) {
  std::istringstream data("\na");
  group_fixture.parse(data);
  B(); D(); N("a"); U(); E();

  CMP();
}

GFTEST(Wiki test #1) {
  std::istringstream data("\
#?schema footype\n\
a\n\
  plugin\n\
    a\n\
    q\n\
  4\n\
  plugin\n\
    b\n\
  sum\n\
    7\n\
  -2\n"
  );
  group_fixture.parse(data);
  
  B();
    D();
      C("?schema footype", true);
      N("a");
      D();
        N("plugin");
        D(); N("a"); N("q"); U();
        N("4"); N("plugin");
        D(); N("b"); U();
        N("sum");
        D(); N("7"); U();
        N("-2");
      U();
    U();
  E();

  CMP();
}

GFTEST(Wiki test #2) {
  std::istringstream data("\
module foo\n\
\n\
schema footype\n\
\n\
type footype\n\
     named a\n\
           multi\n\
\n\
type multi\n\
     unordered\n\
       $ list named plugin, list string\n\
       named sum integer (> 0)\n\
       $ list integer"
  );
  group_fixture.parse(data);
  B(); D();
    N("module"); D(); N("foo"); U();
    N("schema"); D(); N("footype"); U();
    N("type"); D(); 
      N("footype");
      N("named"); D(); N("a"); N("multi"); U();
    U();
    N("type");
    D();
      N("multi");
      N("unordered");
      D();
        N("$"); D(); N("list"); D(); N("named"); D(); N("plugin"); N("list"); D(); N("string"); U(); U(); U(); U();
        N("named"); D(); N("sum"); D(); N("integer"); D(); N("(> 0)"); U(); U(); U();
        N("$"); D(); N("list"); D(); N("integer"); U(); U();
      U();
    U();
  U(); E();

  CMP();
}

GFTEST(Wiki test #3) {
  std::istringstream data("\
#?schema songDB\n\
TITLE:    Home\n\
ARTIST:   \"Dixie Chicks\"\n\
RATING:   9\n\
RIPPED:   T\n\
\n\
TITLE:    Fly\n\
ARTIST:   \"Dixie Chicks\"\n\
RATING:   8\n\
RIPPED:   T\n\
\n\
TITLE:    Roses\n\
ARTIST:   \"Kathy Mattea\"\n\
RATING:   7\n\
RIPPED:   T");
  group_fixture.parse(data);
  string dataset[3][4] = {
    {"Home",  "Dixie Chicks", "9", "T"},
    {"Fly",   "Dixie Chicks", "8", "T"},
    {"Roses", "Kathy Mattea", "7", "T"}
  };
  B(); D();
    C("?schema songDB", true);
    for (unsigned i = 0; i < 3; ++i) {
      N("TITLE:"); D(); N(dataset[i][0]); U();
      N("ARTIST:"); D(); N(dataset[i][1]); U();
      N("RATING:"); D(); N(dataset[i][2]); U();
      N("RIPPED:"); D(); N(dataset[i][3]); U();
    }
  U(); E();

  CMP();
}

GFTEST(Old CVS #1) {
  std::istringstream data("\
a, b c\n\
  d #comment\n\
 \"J\"\n\
e\n\
  f\n\
    g\n\
h\n\
  i\n\
    j\n\
 k\n\
l\n\
  i,j,k\n\
a `\n\
  ahllo\n\
   du\n\
 pferd\n"
  );
  group_fixture.parse(data);
  B(); D();
    N("a"); N("b"); D(); 
      N("c"); N("d"); C("comment", false); N("J");
    U();
    N("e"); D(); N("f"); D(); N("g"); U(); U();
    N("h"); D(); N("i"); D(); N("j"); U(); N("k"); U();
    N("l"); D(); N("i"); N("j"); N("k"); U();
    N("a"); D(); N("ahllo\n du\n"); N("pferd"); U();
  U(); E();

  CMP();
}

GFTEST(Simple block test) {
  std::istringstream data("\
a `\n\
  ahllo\n\
  du\n\
 pferd\n"
  );
  group_fixture.parse(data);
  B(); D();
    N("a"); 
    D(); 
      N("ahllo\ndu\n"); N("pferd"); 
    U();
  U(); E();

  CMP();
}

GFTEST(Simple block test Variant) {
  std::istringstream data("\
a `\n\
   ahllo\n\
   du\n\
  pferd\n"
  );
  group_fixture.parse(data);
  B(); D();
    N("a"); 
    D(); 
      N("ahllo\ndu\n"); N("pferd"); 
    U();
  U(); E();

  CMP();
}

GFTEST() {
  std::istringstream data(
    "a b `\n"
    "  cX\n"
    "  TT\n"
    "c\n");
  group_fixture.parse(data);
  B(); D();
    N("a");
    D();
      N("b");
      D();
        N("cX\nTT\n");
      U();
    U();
    N("c");
  U(); E();

  CMP();
}

GFTEST() {
  std::istringstream data("a, `\n  foo\n bar");
  group_fixture.parse(data);
  B(); D();
    N("a");
    N("foo\n");
    D(); N("bar"); U();
  U(); E();
  CMP();
}

GFTEST() {
  std::istringstream data("a `\n  foo\n bar");
  group_fixture.parse(data);
  B(); D();
    N("a");
    D();
      N("foo\n");
      N("bar");
    U();
  U(); E();
  CMP();
}

GFTEST() {
  std::istringstream data("`\n  foo\n bar\n qux");
  group_fixture.parse(data);
  B(); D();
    N("foo\n");
    D();
      N("bar");
      N("qux");
    U();
  U(); E();
  CMP();
}

GFTEST() {
  std::istringstream data("foo\n   #x\n bar");
  group_fixture.parse(data);
  B(); D();
    N("foo");
    C("x", true);
    D();
      N("bar");
    U();
  U(); E();
  CMP();
}

GFTEST() {
  std::istringstream data("\"");
  try {
    group_fixture.parse(data);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL parser");
    return;
  }
  Check(!"not throwed");
}

GFTEST() {
  std::istringstream data("x (");
  try {
    group_fixture.parse(data);
  } catch (tdl::tdl_error const &m) {
    Equals(m.module(), "TDL parser");
    return;
  }
  Check(!"not throwed");
}

