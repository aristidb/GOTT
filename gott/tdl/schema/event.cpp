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

#include "event.hpp"
#include "match.hpp"
#include "item.hpp"

namespace ev = tdl::schema::ev;
using tdl::schema::item;
using std::ostream;
using boost::static_visitor;

ev::event::~event() {}

bool ev::begin_parse::play(item &r) const {
  return r.play(*this);
}

bool ev::node::play(item &r) const {
  return r.play(*this);
}

bool ev::down::play(item &r) const {
  return r.play(*this);
}

bool ev::up::play(item &r) const { 
  return r.play(*this);
}

bool ev::end_parse::play(item &r) const {
  return r.play(*this);
}

bool ev::child_succeed::play(item &r) const {
  return r.play(*this);
}

bool ev::child_fail::play(item &r) const {
  return r.play(*this);
}

void ev::begin_parse::print(ostream &s) const {
  s << "token:begin_parse";
}

void ev::node::print(ostream &s) const {
  s << "token:node{" << data << "}";
}

void ev::down::print(ostream &s) const {
  s << "token:down";
}

void ev::up::print(ostream &s) const {
  s << "token:up";
}

void ev::end_parse::print(ostream &s) const {
  s << "token:end_parse";
}

void ev::child_succeed::print(ostream &s) const {
  s << "notify:child_succeed";
}

void ev::child_fail::print(ostream &s) const {
  s << "notify:child_fail";
}

ev::node::node(gott::string const &s) : data(s) {}

gott::string const &ev::node::get_data() const { return data; }

namespace {
  struct token_visitor : public static_visitor<ev::token const &> {
    ev::token const &operator() (ev::token const &t) const {
      return t;
    }
  };
}

ev::token const &ev::get(ev::token_t const &t) {
  return apply_visitor(token_visitor(), t);
}
