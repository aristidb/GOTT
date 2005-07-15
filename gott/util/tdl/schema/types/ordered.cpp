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

#include "ordered.hpp"

using std::vector;

namespace schema = gott::util::tdl::schema;
namespace ev = gott::util::tdl::schema::ev;
using schema::rule;
using schema::match_ordered;

match_ordered::match_ordered(vector<rule::factory const *> const &r, 
                             rule::attributes const &a, match &m)
: rule(need, a, m), subrules(r), pos(subrules.begin()) {
  if (pos != subrules.end())
    matcher().add(**pos);
  else
    expectation = nothing;  
}

bool match_ordered::play(ev::child_succeed const &) {
  if (++pos != subrules.end()) 
    matcher().add(**pos);
  else 
    expectation = nothing;
  return true;
}

bool match_ordered::accept_empty(vector<rule::factory const *> const &children) {
  bool accept = true;
  for (vector<rule::factory const *>::const_iterator it = children.begin(); 
       it != children.end(); ++it)
    accept &= (*it)->accept_empty();
  return accept;
}
