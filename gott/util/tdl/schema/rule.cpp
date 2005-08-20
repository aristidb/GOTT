// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
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

#include "rule.hpp"
#include "rule_attr.hpp"
#include "match.hpp"
#include "event.hpp"
#include <gott/util/range_algo.hpp>
#include <boost/bind.hpp>
#include <gott/util/string/stl.hpp>

namespace schema = gott::tdl::schema;
using schema::match;
using schema::rule;
using schema::rule_attr;
namespace ev = schema::ev;
using gott::tdl::structure::writable_structure;

class rule::IMPL {
public:
  match *cont;
  rule_attr attrib;

  IMPL(match &m, rule_attr const &a) 
  : cont(&m), attrib(a) {
    start_structure();
    add_tags();
  }

  void end_structure() {
    if (attrib.coat())
      cont->direct_structure().end();
  }

private:
  void add_tags() {
    for (Vector<string>::const_iterator it = attrib.tags().begin();
         it != attrib.tags().end(); ++it)
      cont->direct_structure().add_tag(*it);
  }

  void start_structure() {
    if (attrib.coat())
      cont->direct_structure().begin();
  }
};

rule::rule(rule_attr const &a, match &m) 
: pIMPL(new IMPL(m, a)) {
}

void rule::finish() {
  pIMPL->end_structure();
}

rule::~rule() {}

rule_attr const &rule::attributes() const { return pIMPL->attrib; }
match &rule::matcher() { return *pIMPL->cont; }

bool rule::play(ev::begin_parse const &) { return play_other(); }
bool rule::play(ev::down const &) { return play_other(); }
bool rule::play(ev::up const &) { return play_other(); }
bool rule::play(ev::end_parse const &) { return play_other(); }
bool rule::play(ev::node const &) { return play_other(); }

bool rule::play(ev::child_succeed const &) { return play_other(); }
bool rule::play(ev::child_fail const &) { return play_other(); }

bool rule::play_other() { return false; }

std::ostream &schema::operator<<(std::ostream &s, rule_attr const &a) {
  s << '(' << (a.coat() ? "coat" : "flat");
  s << ",tags:" << range(a.tags());
  if (dynamic_cast<xany::printable const *>(&a.user().get_operations()))
    s << ",user:" << a.user();
  s << ",repatcher:" << a.repatcher() << ')';
  return s;
}
