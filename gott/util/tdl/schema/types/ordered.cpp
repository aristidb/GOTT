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

#include "ordered.hpp"

namespace schema = gott::tdl::schema;
namespace ev = gott::tdl::schema::ev;
using schema::item;
using schema::match_ordered;

match_ordered::match_ordered(Vector<rule_t const *> const &r, 
                             rule_attr const &a, match &m)
: happy_once(a, m), subrule_ts(r), pos(subrule_ts.begin()) {
  if (pos != subrule_ts.end())
    matcher().add(**pos);
  else
    be_happy();
}

match_ordered::~match_ordered() {}

bool match_ordered::play(ev::child_succeed const &) {
  if (++pos != subrule_ts.end()) 
    matcher().add(**pos);
  else 
    be_happy();
  return true;
}

bool match_ordered::accept_empty(Vector<rule_t const *> const &children) {
  bool accept = true;
  for (Vector<rule_t const *>::const_iterator it = children.begin(); 
       it != children.end(); ++it)
    ;//FIXME accept &= (*it)->accept_empty();
  return accept;
}

gott::string match_ordered::name() const {
  return "ordered";
}
