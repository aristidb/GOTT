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

#include "enumeration.hpp"
#include "../event.hpp"

using std::list;
using std::vector;
using std::wstring;
using gott::util::xany::Xany;
using gott::util::xany::Xany_cast;

namespace schema = gott::util::tdl::schema;
namespace ev = gott::util::tdl::schema::ev;
using schema::rule;
using schema::match_enumeration;

rule::attributes match_enumeration::attributes(vector<wstring> const &v, 
                                               bool cc) {
  return rule::attributes(std::list<wstring>(), cc, v);
}

match_enumeration::match_enumeration(rule::attributes const &a, match &m) 
: rule(need, a, m) {
  if (get_attributes().user().type() != typeid(vector<wstring>))
    throw std::bad_exception();
}

bool match_enumeration::play(ev::node const &n) {
  if (expects() == need) {
    vector<wstring> v = Xany_cast<vector<wstring> >(get_attributes().user());

    vector<wstring>::const_iterator it = 
      find(v.begin(), v.end(), n.get_data());
    
    if (it == v.end())
      return false;
    
    matcher().structure().data(Xany(it - v.begin()));
    expectation = nothing;
    return true;
  }

  return false;
}
