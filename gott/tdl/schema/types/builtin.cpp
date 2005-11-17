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

#include "../by_name.hpp"
#include "any.hpp"
#include "document.hpp"
#include "follow_ordered.hpp"
#include "list.hpp"
#include "named.hpp"
#include "ordered.hpp"
#include "node.hpp"
#include "unordered.hpp"
#include <gott/string/string.hpp>

namespace {
struct builtin_types {
  builtin_types();
} auto_reg;
}

using namespace gott::tdl::schema;

#define REG(t) by_name().add<match_##t>(#t)

builtin_types::builtin_types() {
  REG(any);
  REG(document);
  by_name().add<match_follow_ordered>("follow");
  REG(list);
  REG(named);
  REG(ordered);
  REG(node);
  REG(unordered);
}
