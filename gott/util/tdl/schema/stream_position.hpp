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

#ifndef GOTT_UTIL_TDL_SCHEMA_STREAM_POSITION_HPP
#define GOTT_UTIL_TDL_SCHEMA_STREAM_POSITION_HPP

#include <gott/util/tdl/simple/parse/parser.hpp>
#include <string>

namespace gott {
namespace tdl {
namespace schema {
namespace detail {

struct stream_position : public simple::line_logger {
  stream_position();
  void start_line();
  void token(unsigned start, unsigned end, std::wstring const &nd);
  void line_position(unsigned line_pos);

  unsigned line_new, line, pos;
  unsigned native_end, current;
  std::wstring tok;
  bool after;
};

}}}}

#endif
