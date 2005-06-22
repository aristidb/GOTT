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

#include "../by_name.hpp"
#include "any.hpp"
#include "document.hpp"
#include "enumeration.hpp"
#include "follow.hpp"
#include "integer.hpp"
#include "list.hpp"
#include "literal.hpp"
#include "named.hpp"
#include "ordered.hpp"
#include "string.hpp"
#include "unordered.hpp"

namespace {
struct builtin_types {
  builtin_types();
} auto_reg;
}

namespace s = gott::util::tdl::schema;

builtin_types::builtin_types() {
  s::name_manager().add(L"any",         s::match_any::factory::index());
  s::name_manager().add(L"document",    s::match_document::factory::index());
  s::name_manager().add(L"enumeration", s::match_enumeration::factory::index());
  s::name_manager().add(L"follow",      s::match_follow::factory::index());
  s::name_manager().add(L"integer",     s::match_integer::factory::index());
  s::name_manager().add(L"list",        s::match_list::factory::index());
  s::name_manager().add(L"literal",     s::match_literal::factory::index());
  s::name_manager().add(L"named",       s::match_named::factory::index());
  s::name_manager().add(L"ordered",     s::match_ordered::factory::index());
  s::name_manager().add(L"string",      s::match_string::factory::index());
  s::name_manager().add(L"unordered",   s::match_unordered::factory::index());
}
