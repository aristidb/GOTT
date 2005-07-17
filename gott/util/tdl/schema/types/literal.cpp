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

#include "literal.hpp"
#include "../event.hpp"
#include "../../exceptions.hpp"

using std::list;
using std::wstring;
using gott::util::xany::Xany;
using gott::util::xany::Xany_cast;

namespace schema = gott::util::tdl::schema;
namespace ev = gott::util::tdl::schema::ev;
using schema::rule;
using schema::match_literal;

rule::attributes match_literal::attributes(wstring const &w, bool cc) {
  return rule::attributes(std::list<wstring>(), cc, w);
}

match_literal::match_literal(rule::attributes const &a, match &m)
: rule(need, a, m) {
  if (get_attributes().user().type() != typeid(wstring))
    throw dont_accept(L"user-data other than wstring");
}

bool match_literal::play(ev::node const &n) {
  if (expects() == need) {
    wstring x = Xany_cast<wstring>(get_attributes().user());

    if (x != n.get_data())
      return false;
    
    matcher().structure().data(Xany(n.get_data()));
    expectation = nothing;
    return true;
  }

  return false;
}

wchar_t const *match_literal::name() const {
  return L"literal";
}
