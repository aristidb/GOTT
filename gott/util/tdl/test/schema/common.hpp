// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Testing
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

#include <gott/util/tdl/schema/parse.hpp>
#include <gott/util/tdl/schema/context.hpp>
#include <gott/util/tdl/exceptions.hpp>
#include <gott/util/tdl/structure/tree.hpp>
#include <gott/util/tdl/structure/comfort.hpp>
#include <gott/util/tdl/structure/print.hpp>
#include <gott/util/string/string.hpp>
#include <gott/util/tut/tut.h>

namespace tut {
struct schema_basic {
  gott::tdl::structure::tree tree, xp;
  gott::tdl::schema::match match;
  gott::tdl::schema::context context;
  std::wstring rest;
  schema_basic() : match(tree) {}
  void run_test(std::wstring const &input) {
    match.add(context.get());
    std::wistringstream stream(input);
    match.parse(stream);
    rest = stream.str().c_str() + stream.tellg();
  }
};
}

