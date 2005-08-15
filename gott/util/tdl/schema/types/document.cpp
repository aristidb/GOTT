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

#include "document.hpp"

namespace schema = gott::tdl::schema;
namespace ev = gott::tdl::schema::ev;
using schema::rule;
using schema::match_document;

match_document::match_document(rule_factory const &sr, 
                               rule_attr const &a, match &m)
: happy_once(a, m), sub(sr), state(first) {}

bool match_document::play(ev::begin_parse const &) {
  state = begun_parse;
  return true;
}

bool match_document::play(ev::down const &) { 
  if (state != begun_parse)
    return false;

  state = opened;
  matcher().add(sub);
  return true;
}

bool match_document::play(ev::up const &) {
  if (state != opened)
    return false;

  state = closed;
  return true;
}

bool match_document::play(ev::end_parse const &) {
  if (state == closed) {
    be_happy();
    return true;
  }

  return false;
}

bool match_document::play(ev::child_succeed const &) {
  return true;
}

gott::nstring match_document::name() const {
  return "document";
}
