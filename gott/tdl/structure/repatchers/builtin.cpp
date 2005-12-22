// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
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

#include "integer.hpp"
#include "enumeration.hpp"
#include "substring.hpp"
#include "find_literal.hpp"
#include "../repatcher_by_name.hpp"
#include "../repatch.hpp"

namespace {
struct builtin_repatchers {
  builtin_repatchers();
} auto_reg;
}

using namespace gott::tdl::structure;

builtin_repatchers::builtin_repatchers() {
  repatch_integer::reg();
  repatch_enumeration::reg();
  repatch_substring::reg();
  repatch_find_literal::reg();
}
