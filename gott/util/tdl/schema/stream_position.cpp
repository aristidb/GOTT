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

#include "stream_position.hpp"

using gott::util::tdl::schema::detail::line_pos;

line_pos::line_pos() 
  : line_new(0), line(0), pos(0), native_end(0), current(0), after(false) {}

void line_pos::start_line() { ++line_new; }

void line_pos::token(unsigned p, unsigned e, std::wstring const &w) {
  pos = p;
  native_end = e;
  tok = w;
  line = line_new;
}

void line_pos::line_position(unsigned x) {
  current = x;
}
