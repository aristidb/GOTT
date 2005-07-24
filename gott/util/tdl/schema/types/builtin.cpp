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
#include "follow_ordered.hpp"
#include "integer.hpp"
#include "list.hpp"
#include "literal.hpp"
#include "named.hpp"
#include "ordered.hpp"
#include "string.hpp"
#include "unordered.hpp"
#include "repatch.hpp"

namespace {
struct builtin_types {
  builtin_types();
} auto_reg;
}

using namespace gott::util::tdl::schema;

#define FOLLOW_ORDERED_AS_FOLLOW

builtin_types::builtin_types() {
  name_manager().add(L"any",            match_any::factory::index());
  name_manager().add(L"document",       match_document::factory::index());
  name_manager().add(L"enumeration",    match_enumeration::factory::index());
#ifndef FOLLOW_ORDERED_AS_FOLLOW
  name_manager().add(L"follow",         match_follow::factory::index());
  name_manager().add(L"follow_ordered", match_follow_ordered::factory::index());
#else
  name_manager().add(L"follow",         match_follow_ordered::factory::index());
#endif
  name_manager().add(L"integer",        match_integer::factory::index());
  name_manager().add(L"list",           match_list::factory::index());
  name_manager().add(L"literal",        match_literal::factory::index());
  name_manager().add(L"named",          match_named::factory::index());
  name_manager().add(L"ordered",        match_ordered::factory::index());
  name_manager().add(L"string",         match_string::factory::index());
  name_manager().add(L"unordered",      match_unordered::factory::index());
  name_manager().add(L"repatch",        match_repatch::factory::index());
}
