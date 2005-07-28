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

#include "repatch.hpp"
#include "../event.hpp"
#include "../rule_attr.hpp"

namespace schema = gott::util::tdl::schema;

using schema::match_repatch;

match_repatch::match_repatch(rule::attributes const &a, match &m)
: happy_once(a, m) {
  if (a.user().type() != typeid(structure::repatcher *))
    throw dont_accept(L"non-repatch user attribute");

  repatcher = xany::Xany_cast<structure::repatcher *>(a.user());
}

bool match_repatch::play(ev::node const &n) {
  if (expectation() != need)
    return false;

  boost::scoped_ptr<structure::writable_structure> 
    con(repatcher->deferred_write(matcher().direct_structure()));
  con->data(gott::util::xany::Xany(n.get_data()));
  
  be_happy();
  return true;
}

wchar_t const *match_repatch::name() const {
  return L"repatch";
}
