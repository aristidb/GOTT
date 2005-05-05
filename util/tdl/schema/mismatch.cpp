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

#include "mismatch.hpp"
#include "position.hpp"

using gott::util::tdl::schema::mismatch;

mismatch::mismatch(detail::line_pos const &p) {  
  std::ostringstream o;
  o << p.line << ':' << p.pos+1 << " : mismatch ";
  if (p.line_new > p.line || p.current > p.native_end)
    o << "after";
  else
    o << "at";
  o << " token " << p.tok;
  w = o.str();
}

mismatch::~mismatch() throw() {}

char const *mismatch::what() const throw() {
  return w.c_str();
}
