// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
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

#include "rule.hpp"
#include "rule_attr.hpp"
#include "parse.hpp"
#include "event.hpp"

using std::wstring;

namespace schema = gott::util::tdl::schema;
using schema::match;
using schema::rule;
namespace ev = gott::util::tdl::schema::ev;
using gott::util::tdl::structure::writable_structure;

class rule::IMPL {
public:
  match *cont;
  attributes attrib;

  IMPL(match &m, attributes const &a) : cont(&m), attrib(a) {
    start_structure();
    add_tags();
  }

  void end_structure() {
    if (attrib.coat())
      cont->structure().end();
  }

private:
  void add_tags() {
    void (writable_structure::*add)(wstring const &) = &writable_structure::add_tag;
    for_each(range(attrib.tags()), bind(add, &cont->structure(), _1));
  }

  void start_structure() {
    if (attrib.coat())
      cont->structure().begin();
  }
};

rule::rule(expect e, attributes a, match &m) 
: expectation(e), pIMPL(new IMPL(m, a)) {
}

void rule::finish() {
  pIMPL->end_structure();
}

rule::~rule() { delete pIMPL; }

rule::attributes const &rule::get_attributes() const { return pIMPL->attrib; }
match &rule::matcher() { return *pIMPL->cont; }

bool rule::play(ev::begin_parse const &) { return false; }
bool rule::play(ev::down const &) { return false; }
bool rule::play(ev::up const &) { return false; }
bool rule::play(ev::end_parse const &) { return false; }
bool rule::play(ev::node const &) { return false; }

bool rule::play(ev::child_succeed const &) { return false; }
bool rule::play(ev::child_fail const &) { return false; }
