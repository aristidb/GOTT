
// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
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

#include <gott/tdl/simple/parse/parser.hpp>
#include <gott/tut/tut.h>
#include <ostream>
#include <gott/string/string.hpp>

using gott::string;

namespace tut {
struct spb : gott::tdl::simple::parser {
  spb() : parser(0) {}

  enum event_id { b, e, d, u, n, c };
  typedef std::pair<event_id, string> event;
  Vector<event> xp, ev;

  void begin_parse() { ev.push_back(event(b, "")); }
  void end_parse() { ev.push_back(event(e, "")); }
  void down() { ev.push_back(event(d, "")); }
  void up() { ev.push_back(event(u, "")); }
  void node(string const &w) { ev.push_back(event(n, w)); }
  void comment(string const &w, bool nl) { 
    ev.push_back(event(c, (nl ? "\n" : "") + w)); 
  }

  void B() { xp.push_back(event(b, "")); }
  void E() { xp.push_back(event(e, "")); }
  void D() { xp.push_back(event(d, "")); }
  void U() { xp.push_back(event(u, "")); }
  void N(string const &w) { xp.push_back(event(n, w)); }
  void C(string const &w, bool nl) { 
    xp.push_back(event(c, (nl ? "\n" : "") + w)); 
  }
};

typedef test_group<spb> tf;
typedef tf::object object;
}

NTL_MOVEABLE(tut::spb::event);

namespace {
tut::tf spp("simple::parse");
}

std::ostream &operator<<(std::ostream &o, tut::spb::event const &e) {
  return o << "bedunc"[e.first] << e.second;
}

namespace tut {

// Empty document
template<> template<>
void object::test<1>(int) {
  std::istringstream data("");
  parse(data);
  B(); D(); U(); E();
  ensure_equals("empty", range(ev), range(xp));
}

// Very simple document
template<> template<>
void object::test<2>(int) {
  std::istringstream data("hallodu");
  parse(data);
  B(); D(); N("hallodu"); U(); E();
  ensure_equals("single-word", range(ev), range(xp));
}

// One-line document with more than one element
template<> template<>
void object::test<3>(int) {
  std::istringstream data("  hallodu, foobar zulu");
  parse(data);
  B(); D(); N("hallodu"); N("foobar"); D(); N("zulu"); U(); U(); E();
  ensure_equals("multiple-word", range(ev), range(xp));
}

// Empty multi-line document with funny spaces
template<> template<>
void object::test<4>(int) {
  std::istringstream data("\n\n          \n     \n\n\n");
  parse(data);
  B(); D(); U(); E();
  ensure_equals("quasi-empty", range(ev), range(xp));
}

// Two simple documents
template<> template<>
void object::test<5>(int) {
  std::istringstream data("\n\n\nfoobar      \n***\n  zumwinke");
  parse(data);
  parse(data);

  B(); D(); N("foobar"); U(); E();
  B(); D(); N("zumwinke"); U(); E();

  ensure_equals("dual stream #1", range(ev), range(xp));
}

// Indentation
template<> template<>
void object::test<6>(int) {
  std::istringstream data("\n\n  \n       fobar\n");
  parse(data);
  B(); D(); N("fobar"); U(); E();

  ensure_equals("indented #1", range(ev), range(xp));
}

// Indentation #2 (reduced)
template<> template<>
void object::test<7>(int) {
  std::istringstream data("\n  x");
  parse(data);
  B(); D(); N("x"); U(); E();

  ensure_equals("indented #2", range(ev), range(xp));
}


// Simple comment
template<> template<>
void object::test<8>(int) {
  std::istringstream data("\n\n#__ mycomment,\n");
  parse(data);
  B(); D(); C("__ mycomment,", true); U(); E();

  ensure_equals("own-line comment", range(ev), range(xp));
}

// Newlined char
template<> template<>
void object::test<9>(int) {
  std::istringstream data("\na");
  parse(data);
  B(); D(); N("a"); U(); E();

  ensure_equals("newlined char", range(ev), range(xp));
}

// Wiki test #1
template<> template<>
void object::test<10>(int) {
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
  parse(data);
  
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

  ensure_equals("wiki test #1", range(ev), range(xp));
}

// Wiki test #2
template<> template<>
void object::test<11>(int) {
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
  parse(data);
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

  ensure_equals("wiki test #2", range(ev), range(xp));
}

// Wiki test #3
template<> template<>
void object::test<12>(int) {
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
  parse(data);
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

  ensure_equals("wiki test #3", range(ev), range(xp));
}

// Old CVS #1
template<> template<>
void object::test<13>(int) {
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
  parse(data);
  B(); D();
    N("a"); N("b"); D(); 
      N("c"); N("d"); C("comment", false); N("J");
    U();
    N("e"); D(); N("f"); D(); N("g"); U(); U();
    N("h"); D(); N("i"); D(); N("j"); U(); N("k"); U();
    N("l"); D(); N("i"); N("j"); N("k"); U();
    N("a"); D(); N("ahllo\n du\n"); N("pferd"); U();
  U(); E();

  ensure_equals("Old CVS test #1", range(ev), range(xp));
}

// Simple block test
template<> template<>
void object::test<14>(int) {
  std::istringstream data("\
a `\n\
  ahllo\n\
  du\n\
 pferd\n"
  );
  parse(data);
  B(); D();
    N("a"); 
    D(); 
      N("ahllo\ndu\n"); N("pferd"); 
    U();
  U(); E();

  ensure_equals("simple block", range(ev), range(xp));
}

// Simple block test Variant
template<> template<>
void object::test<15>(int) {
  std::istringstream data("\
a `\n\
   ahllo\n\
   du\n\
  pferd\n"
  );
  parse(data);
  B(); D();
    N("a"); 
    D(); 
      N("ahllo\ndu\n"); N("pferd"); 
    U();
  U(); E();

  ensure_equals("simple block var", range(ev), range(xp));
}

template<> template<>
void object::test<16>(int) {
  std::istringstream data(
    "a b `\n"
    "  cX\n"
    "  TT\n"
    "c\n");
  parse(data);
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

  ensure_equals("second block", range(ev), range(xp));
}

template<> template<>
void object::test<17>(int) {
  no_test();
}

// further tests
}

