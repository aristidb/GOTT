
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

#include <gott/util/tdl/simple/parse/parser.hpp>
#include <gott/util/tut/tut.h>
#include <ostream>
#include <gott/util/nstring/nstring.hpp>

using gott::nstring;

namespace tut {
struct spb : gott::tdl::simple::parser {
  enum event_id { b, e, d, u, n, c };
  typedef std::pair<event_id, nstring> event;
  Vector<event> xp, ev;

  void begin_parse() { ev.push_back(event(b, L"")); }
  void end_parse() { ev.push_back(event(e, L"")); }
  void down() { ev.push_back(event(d, L"")); }
  void up() { ev.push_back(event(u, L"")); }
  void node(nstring const &w) { ev.push_back(event(n, w)); }
  void comment(nstring const &w, bool nl) { 
    ev.push_back(event(c, (nl ? L"\n" : L"") + w)); 
  }

  void B() { xp.push_back(event(b, L"")); }
  void E() { xp.push_back(event(e, L"")); }
  void D() { xp.push_back(event(d, L"")); }
  void U() { xp.push_back(event(u, L"")); }
  void N(nstring const &w) { xp.push_back(event(n, w)); }
  void C(nstring const &w, bool nl) { 
    xp.push_back(event(c, (nl ? L"\n" : L"") + w)); 
  }
};

typedef test_group<spb> tf;
typedef tf::object object;
}

NTL_MOVEABLE(tut::spb::event);

namespace {
tut::tf spp("simple::parse");
}

using gott::tdl::simple::parse;

std::ostream &operator<<(std::ostream &o, 
                         Vector<tut::spb::event> const &e) {
  o << '\n';
  for (Vector<tut::spb::event>::const_iterator it = e.begin(); 
       it != e.end(); ++it) 
    o << "bedunc"[it->first] << it->second << ' ';
  o << '\n';
  return o;
}

namespace tut {

// Empty document
template<> template<>
void object::test<1>(int) {
  std::wistringstream data(L"");
  parse(data, *this);
  B(); D(); U(); E();
  ensure_equals("empty", ev, xp);
}

// Very simple document
template<> template<>
void object::test<2>(int) {
  std::wistringstream data(L"hallodu");
  parse(data, *this);
  B(); D(); N(L"hallodu"); U(); E();
  ensure_equals("single-word", ev, xp);
}

// One-line document with more than one element
template<> template<>
void object::test<3>(int) {
  std::wistringstream data(L"  hallodu, foobar zulu");
  parse(data, *this);
  B(); D(); N(L"hallodu"); N(L"foobar"); D(); N(L"zulu"); U(); U(); E();
  ensure_equals("multiple-word", ev, xp);
}

// Empty multi-line document with funny spaces
template<> template<>
void object::test<4>(int) {
  std::wistringstream data(L"\n\n          \n     \n\n\n");
  parse(data, *this);
  B(); D(); U(); E();
  ensure_equals("quasi-empty", ev, xp);
}

// Two simple documents
template<> template<>
void object::test<5>(int) {
  std::wistringstream data(L"\n\n\nfoobar      \n***\n  zumwinkel");
  parse(data, *this);
  parse(data, *this);

  B(); D(); N(L"foobar"); U(); E();
  B(); D(); N(L"zumwinkel"); U(); E();

  ensure_equals("dual stream #1", ev, xp);
}

// Indentation
template<> template<>
void object::test<6>(int) {
  std::wistringstream data(L"\n\n  \n       fobar\n");
  parse(data, *this);
  B(); D(); N(L"fobar"); U(); E();

  ensure_equals("indented #1", ev, xp);
}

// Indentation #2 (reduced)
template<> template<>
void object::test<7>(int) {
  std::wistringstream data(L"\n  x");
  parse(data, *this);
  B(); D(); N(L"x"); U(); E();

  ensure_equals("indented #2", ev, xp);
}


// Simple comment
template<> template<>
void object::test<8>(int) {
  std::wistringstream data(L"\n\n#__ mycomment,\n");
  parse(data, *this);
  B(); D(); C(L"__ mycomment,", true); U(); E();

  ensure_equals("own-line comment", ev, xp);
}

// Newlined char
template<> template<>
void object::test<9>(int) {
  std::wistringstream data(L"\na");
  parse(data, *this);
  B(); D(); N(L"a"); U(); E();

  ensure_equals("newlined char", ev, xp);
}

// Wiki test #1
template<> template<>
void object::test<10>(int) {
  std::wistringstream data(L"\
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
  parse(data, *this);
  
  B();
    D();
      C(L"?schema footype", true);
      N(L"a");
      D();
        N(L"plugin");
        D(); N(L"a"); N(L"q"); U();
        N(L"4"); N(L"plugin");
        D(); N(L"b"); U();
        N(L"sum");
        D(); N(L"7"); U();
        N(L"-2");
      U();
    U();
  E();

  ensure_equals("wiki test #1", ev, xp);
}

// Wiki test #2
template<> template<>
void object::test<11>(int) {
  std::wistringstream data(L"\
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
  parse(data, *this);
  B(); D();
    N(L"module"); D(); N(L"foo"); U();
    N(L"schema"); D(); N(L"footype"); U();
    N(L"type"); D(); 
      N(L"footype");
      N(L"named"); D(); N(L"a"); N(L"multi"); U();
    U();
    N(L"type");
    D();
      N(L"multi");
      N(L"unordered");
      D();
        N(L"$"); D(); N(L"list"); D(); N(L"named"); D(); N(L"plugin"); N(L"list"); D(); N(L"string"); U(); U(); U(); U();
        N(L"named"); D(); N(L"sum"); D(); N(L"integer"); D(); N(L"(> 0)"); U(); U(); U();
        N(L"$"); D(); N(L"list"); D(); N(L"integer"); U(); U();
      U();
    U();
  U(); E();

  ensure_equals("wiki test #2", ev, xp);
}

// Wiki test #3
template<> template<>
void object::test<12>(int) {
  std::wistringstream data(L"\
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
  parse(data, *this);
  nstring dataset[3][4] = {
    {L"Home",  L"Dixie Chicks", L"9", L"T"},
    {L"Fly",   L"Dixie Chicks", L"8", L"T"},
    {L"Roses", L"Kathy Mattea", L"7", L"T"}
  };
  B(); D();
    C(L"?schema songDB", true);
    for (unsigned i = 0; i < 3; ++i) {
      N(L"TITLE:"); D(); N(dataset[i][0]); U();
      N(L"ARTIST:"); D(); N(dataset[i][1]); U();
      N(L"RATING:"); D(); N(dataset[i][2]); U();
      N(L"RIPPED:"); D(); N(dataset[i][3]); U();
    }
  U(); E();

  ensure_equals("wiki test #3", ev, xp);
}

// Old CVS #1
template<> template<>
void object::test<13>(int) {
  std::wistringstream data(L"\
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
  parse(data, *this);
  B(); D();
    N(L"a"); N(L"b"); D(); 
      N(L"c"); N(L"d"); C(L"comment", false); N(L"J");
    U();
    N(L"e"); D(); N(L"f"); D(); N(L"g"); U(); U();
    N(L"h"); D(); N(L"i"); D(); N(L"j"); U(); N(L"k"); U();
    N(L"l"); D(); N(L"i"); N(L"j"); N(L"k"); U();
    N(L"a"); D(); N(L"ahllo\n du\n"); N(L"pferd"); U();
  U(); E();

  ensure_equals("Old CVS test #1", ev, xp);
}

// Simple block test
template<> template<>
void object::test<14>(int) {
  std::wistringstream data(L"\
a `\n\
  ahllo\n\
  du\n\
 pferd\n"
  );
  parse(data, *this);
  B(); D();
    N(L"a"); 
    D(); 
      N(L"ahllo\ndu\n"); N(L"pferd"); 
    U();
  U(); E();

  ensure_equals("simple block", ev, xp);
}

template<> template<>
void object::test<15>(int) {
  no_test();
}

// further tests
}

