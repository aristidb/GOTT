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

using gott::nstring;
using gott::tdl::tdl_exception;
using gott::tdl::schema::unregistered_type;

static nstring pizza(nstring const &type) {
  nstring const components[] = { "Type ", type, " not found in database" };
  return nstring(range(components));
}

unregistered_type::unregistered_type(nstring const &type)
  : tdl_exception(pizza(type)) {}

unregistered_type::~unregistered_type() throw() {}
