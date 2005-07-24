// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
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
#include "../exceptions.hpp"

namespace structure = gott::util::tdl::structure;
using structure::repatcher;
using structure::repatch_context;
using structure::failed_repatch;

repatcher::repatcher() {}
repatcher::~repatcher() {}

repatch_context::~repatch_context() {}

failed_repatch::failed_repatch(std::wstring const &s) : tdl_exception(s) {}
failed_repatch::~failed_repatch() throw() {}
