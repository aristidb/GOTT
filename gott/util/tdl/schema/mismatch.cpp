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

#include "../exceptions.hpp"
#include "stream_position.hpp"

namespace schema = gott::util::tdl::schema;
using schema::mismatch;
using gott::util::range;

static std::wstring build_string(schema::detail::stream_position const &p,
                                 std::list<std::wstring> const &t) {
  std::wostringstream o;
  o << p.line << L':' << p.pos+1 << L" : mismatch in ";
  print_separated(o, range(t), L">");
  if (p.line_new > p.line || p.current > p.native_end)
    o << L" after token ";
  else
    o << L" at token ";
  o << p.tok;
  return o.str();
}

mismatch::mismatch(detail::stream_position const &p, 
                   std::list<std::wstring> const &t)
: tdl_exception(build_string(p, t)) {}

mismatch::~mismatch() throw() {}
