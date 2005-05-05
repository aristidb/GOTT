// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Testing
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

#include <gott/util/misc/commonheaders.hpp>

#include "../structure/structure.hpp"
#include "../structure/tree.hpp"
#include "../structure/print.hpp"
#include "../structure/comfort.hpp"
#include "../simple/parse/parser.hpp"
#include <iostream>

using namespace gott::util;
using namespace std;
using namespace tdl::structure;
using namespace boost;
using xany::Xany;

static void indent(unsigned l) {
  while (l--)
    wcout << L' ';
}

static void print_tags(tree::iterator const &x) {
  list<wstring> const &l = x.get_tags();
  for (list<wstring>::const_iterator it = l.begin(); it != l.end(); ++it) {
    if (it != l.begin())
      wcout << L',';
    else
      wcout << L" : ";
    wcout << *it;
  }
}

static void print(tree::iterator const &x, unsigned l = 0) {
  indent(l);
  if (!x.get_data().empty())
    wcout << x.get_data();
  else
    wcout << L'-';
  
  print_tags(x);
  
  //wcout << endl;
  wcout << L" {\n";
  
  for (tree::iterator i = x.first_child(); i; i = i.next())
    print(i, l + 4);

  indent(l); wcout << L"}\n";
}

void structure_test() {
  tree trr;
  revocable_structure &tr = trr;
#if 1
  tr.begin();
    tr.begin();
      tr.data(Xany(L"hallo"));
      tr.begin();
        revocable_structure::pth x = tr.point();
        // VOID (
        tr.data(Xany(L"foo"));
        tr.begin();
          tr.data(Xany(L"bar"));
        tr.end();
        // )
        tr.revert(x);
      
        tr.data(Xany(L"string"));
        tr.add_tag(L"one");
      tr.end();
      tr.begin();
        tr.data(Xany(L"foo"));
        tr.add_tag(L"other");
      tr.end();
      tr.begin();
        tr.data(Xany(L"bar"));
        tr.add_tag(L"other");
      tr.end();
    tr.end();
  tr.end();
  
  wcout << L"root:\n"; print(trr.get_root());
  wcout << L"root[0]:\n"; print(trr.get_root()[0]);
  wcout << L"root[one]:\n"; print(trr.get_root()[0].with_tag(L"one").get());
  wcout << L"root[other]:\n";
  for (tree::tagged_iterator it = trr.get_root()[0].with_tag(L"other"); it; ++it)
    print(it.get());

  tree o;
  cf::nd_list children = boost::assign::list_of
    (cf::S(Xany(L"string"), L"one"))
    (cf::S(Xany(L"foo"), L"other"))
    (cf::S(Xany(L"bar"), L"other"));
 cf::C(cf::MD(Xany(L"hallo"), children)).write_to(o);
 wcout << L"other:\n"; print(o.get_root());

 wcout << L"equal?:" << (o==trr) << endl;

#else
  direct_struc(wcin, tr);
  direct_print p(wcout);
  trr.copy_to(p);
  wcout << endl;
#endif
}

#include "../schema/parse.hpp"
#include "../schema/context.hpp"

#include "../schema/types/optional.hpp"
#include "../schema/types/string.hpp"
#include "../schema/types/document.hpp"
#include "../schema/types/integer.hpp"
#include "../schema/types/list.hpp"
#include "../schema/types/ordered.hpp"
#include "../schema/types/follow.hpp"
#include "../schema/types/any.hpp"
#include "../schema/types/named.hpp"
#include "../schema/types/enumeration.hpp"
#include "../schema/types/literal.hpp"
#include "../schema/types/unordered.hpp"
#include "../schema/context_template.hpp"

#include "../structure/tree.hpp"
#include "../structure/print.hpp"

using namespace gott::util::tdl;
using namespace gott::util::xany;
using namespace schema;

static void printx(Xany const &a) {
  if (!a.empty()) 
    wcout << a;
  else
    wcout << L'-';
}

#define YY wcout << L'$';

#if DEBUG
struct debug_tree : public structure::revocable_structure {
  unsigned long tag, level;
  structure::revocable_structure &o;

  void begin() {YY
    indent(level);
    wcout << tag << L"begin" << endl;
    level += 4;
    o.begin();
  }

  void data(Xany const &x) {YY
    indent(level);
    wcout << tag << L':';
    printx(x);
    wcout << endl;
    o.data(x);
  }

  void end() {YY
    indent(level);
    wcout << tag << L"end" << endl;
    level -= 4;
    o.end();
  }

  void add_tag(wstring const &s) {YY o.add_tag(s); }
  void set_tags(list<wstring> const &l) {YY o.set_tags(l); }

  structure::revocable_structure::pth point() {YY
    structure::revocable_structure::pth mp = o.point();
    indent(level); wcout << L'<' << mp << endl;
    mp = o.point();
    return mp;
  }

  void revert(structure::revocable_structure::pth const &x) {YY
    indent(level); wcout << L'>' << x << endl;
    YY; indent(level); wcout << L"OLD TREE:\n";
    ((structure::tree&)o).dump(wcout);
    o.revert(x);
    YY; indent(level); wcout << L"NEW TREE:\n";
    ((structure::tree&)o).dump(wcout);
  }

  void get_rid_of(structure::revocable_structure::pth const &x) {YY
    indent(level); wcout << L"get rid of:: " <<  x << endl;
  }

  debug_tree(structure::revocable_structure &x) : tag(0), level(0), o(x) {}
};
#endif

void schema_test() {
#ifdef TRYCATCH
  try { //////////////////////////////////////////////////////////////
#endif

  structure::tree rt;
#if DEBUG
  debug_tree dt(rt);
#endif
  
  structure::revocable_structure &t = rt;

#if 0
  context_template document, footype, multi;
  
  ////////////////////////////////////////////////////////////////////
  
  document.begin(match_document::factory::index(), rule::attributes(Xany(L"--doc--")));
    document.param(0);
  document.end();

  footype.begin(match_named::factory::index(), match_named::attributes(L"a"));
    footype.param(1);
  footype.end();

  multi.begin(match_unordered::factory::index(), rule::attributes(Xany(L"--unordered--")));
    multi.begin(match_named::factory::index(), match_named::attributes(L"plugin"));
      multi.begin(match_string::factory::index(), rule::attributes(Xany(L"plugin-data")), slotcfg(slotcfg::list));
      multi.end();
    multi.end();
    multi.begin(match_named::factory::index(), match_named::attributes(L"sum"));
      multi.begin(match_integer::factory::index(), rule::attributes(Xany(L"sum-data"))); // > 0
      multi.end();
    multi.end();
    multi.begin(match_integer::factory::index(), rule::attributes(Xany(L"--other--")) );//, slotcfg(slotcfg::some));
    multi.end();
  multi.end();
  
/*o*/
  ////////////////////////////////////////////////////////////////////

  context c[2];
  vector<context*> vx(2);
  vx[0] = c;
  vx[1] = c + 1;
  
  multi.instantiate(vx, c[1]);
  footype.instantiate(vx, c[0]);

  context a;
  document.instantiate(vx, a);
#elif 0
  context a;
  a.begin(match_document::factory::index(), rule::attributes(Xany(L"d")));
    a.begin(match_ordered::factory::index(), rule::attributes(Xany(L"o")));
      a.begin(match_list::factory::index(), rule::attributes(Xany(L"s")));
        a.begin(match_list::factory::index(), rule::attributes(Xany(L"t")));
          a.begin(match_integer::factory::index(), rule::attributes(Xany(L"ii")));
          ;a.end();
        ;a.end();
      ;a.end();
      a.begin(match_string::factory::index(), rule::attributes(Xany(L"xx")));
    ;a.end();
  ;a.end();
#else
  context a;
  a.begin(match_document::factory::index());
    a.begin(match_list::factory::index());
      a.begin(match_integer::factory::index(), rule::attributes(wstring(L"el")), slotcfg(slotcfg::minimum, 3));
      a.end();
    a.end();
  a.end();
#endif

  ////////////////////////////////////////////////////////////////////
  
  match m(t);
  m.add(a.get());

  simple::meta_parser rec;
  simple::meta_parser::store_one_param schema;
  rec.set_specific(L"schema", ref(schema));
  
  simple::parse_meta(wcin, rec, m.get_debug());

  wcout << L"Schema: " << schema.data() << endl;

  simple::parse(wcin, m, m.get_debug());

#ifdef DEBUG
  rt.dump(wcout);
#else
  structure::direct_print p(wcout);
  rt.copy_to(p);
  wcout << endl;
#endif

#ifdef TRYCATCH
  } catch (exception const &e) { /////////////////////////////////////
    wcout << e.what() << endl;
  } //////////////////////////////////////////////////////////////////
#endif
}

#include "../query/query.hpp"
#include "../structure/structure.hpp"
#include "../structure/print.hpp"

using namespace std;
using namespace boost;
using namespace gott::util::tdl::structure;
using namespace gott::util::tdl::query;

void dump(tree::iterator const &i, unsigned ind = 0) {
  indent(ind);
  if (i.get_data().type() == typeid(wstring))
    wcout << i.get_data();
  wcout << L"{\n";
  
  for (tree::iterator it = i.first_child(); it; it = it.next())
    dump(it, ind + 4);

  indent(ind);
  wcout << L"}\n" << flush;
}

void dump(selection<tree::iterator> const &s) {
  for (selection<tree::iterator>::const_iterator it = s.begin(); it != s.end(); ++it) {
    wcout << it - s.begin() << L':' << flush;
    
    dump(*it);
  }
  wcout << endl;
}

void query_test() {
  tree t;
  writable_structure &tr = t;
  tr.begin();
    tr.data(Xany(L"root"));
    tr.begin();
      tr.data(Xany(L"first"));
      tr.begin();
        tr.data(Xany(L"foo"));
        tr.add_tag(L"foo");
      tr.end();
      tr.begin();
        tr.data(Xany(L"bar"));
        tr.add_tag(L"bar");
      tr.end();
    tr.end();
    tr.begin();
      tr.data(Xany(L"qoo"));
    tr.end();
  tr.end();

  selection<tree::iterator> a(t.get_root());
  dump(a);
  
  selection<tree::iterator> b(a.find(1));
  dump(b);
  
  selection<tree::iterator> c(a.find(0));
  dump(c);
  
  selection<tree::iterator> d(c.find_tag(L"bar"));
  dump(d);
}

#include "../simple/write/writer.hpp"

void simple_writer_test() {
  gott::util::tdl::simple::writer w(std::wcout, 5);
  w.down();
    w.node(L"a");
    w.node(L"b\n q\nJ");
    w.down();
      w.node(L"c");
      w.node(L"c");
      w.down();
        w.node(L"d c");
        w.down();
          w.node(L"!");
          w.up();
        w.node(L"!!");
        w.up();
      w.node(L"-");
      w.comment(L"lalalaLA", false);
      w.up();
    w.comment(L"lalala", false);
    w.node(L"final");
    w.up();
  w.end_document();
}

#include "../simple/parse/parser.hpp"

using namespace std;
using namespace gott::util::tdl;

class MyParser : public simple::parser {
  int level;
public:
  MyParser() : level(0) {}

  void lv() {
    for (int i = 0; i < level; ++i)
      wcout << L' ';
    wcout << level / 4;
  }

  void down() {
    lv(); wcout << L'{' << endl;
    level += 4;
  }
  
  void node(wstring const &s) {
    lv(); wcout << '(' << s << ')' << endl;
  }

  void up() {
    level -= 4;
    lv(); wcout << L'}' << endl;
  }

  void comment(wstring const &s, bool nl) {
#if 1
    lv();
    if (nl) wcout << L'\n';
    wcout << L"COMMENT#" << s << endl;
#endif
  }

  void begin_parse() {}

  void end_parse() {
    if (level)
      wcout << "Ha! Der Parser schliesst nicht alles was er oeffnet, so ein Doofer: " << level / 4 << endl;
  }
};

struct my_line : public simple::line_logger {
  unsigned l;
  my_line() : l(0) {}
  void start_line() { wcout << L"Line: " << ++l << endl; }
  void token(unsigned, unsigned x, wstring const &w) { wcout << L"Token " << x + 1 << L':' << w << endl; }
  void line_position(unsigned) {}
};

bool meta_all(wstring const &cmd, wstring const &param) {
  wcout << L"META$" << cmd << L'$' << param << endl;
  return true;
}

bool meta_schema(wstring const &cmd, wstring const &param) {
  wcout << L"SCHEMA$" << cmd << L'$' << param << endl;
  return true;
}

void simple_parse_test() {
  my_line l;
  wcout << L"StartMeta" << endl;
  simple::meta_parser m;
  m.set_default(meta_all);
  m.set_specific(L"schema", meta_schema);
  parse_meta(wcin, m, &l);
  wcout << L"Meta done" << endl;
  MyParser p;
  parse(wcin, p, &l);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    wcerr << L"Specify the test to be run\n";
    return 1;
  }

  hashd::hash_map<string, void (*)()> tests;

#define TT(t) tests[ #t ] = t ## _test
  TT(structure); TT(schema); TT(query); TT(simple_parse); TT(simple_writer);

  wcout << L"Testing: " << argv[1] << endl;
  (tests[argv[1]])();
}
