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

#include "../exceptions.hpp"
#include <gott/util/nstring/nstring.hpp>

using gott::tdl::schema::dont_accept;
using gott::nstring;

static nstring pizza(nstring const &x) {
  nstring const components[] = { "Don't accept", (x == "") ? "" : ": ", x };
  return nstring(gott::range(components));
}

dont_accept::dont_accept(nstring const &desc) 
  : tdl_exception(pizza(desc)) {}

dont_accept::~dont_accept() throw() {}
