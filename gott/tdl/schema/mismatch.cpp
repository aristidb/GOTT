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
#include "stream_position.hpp"
#include <gott/range.hpp>
#include <gott/string/stl.hpp>
#include <gott/thunk.hpp>
#include <boost/scoped_ptr.hpp>

namespace schema = gott::tdl::schema;
using schema::mismatch;
using gott::range;
using gott::string;
using gott::integer_to_string;

static string build_string(schema::detail::stream_position const &p,
                            std::vector<string> const &t) {
  std::vector<string> out;
  out.push_back(string(*integer_to_string<gott::utf8_t>(p.line)));
  out.push_back(":");
  out.push_back(string(*integer_to_string<gott::utf8_t>(p.pos + 1)));
  out.push_back(" : mismatch in ");
  gott::range_t<std::vector<string>::const_iterator> tg = range(t);
  if (tg.begin() != tg.end())
    out.push_back(*tg.Begin++);
  for (; tg.begin() != tg.end(); ++tg.Begin) {
    out.push_back(">");
    out.push_back(*tg.begin());
  }
  if (p.line_new > p.line || p.current > p.native_end)
    out.push_back(" after token ");
  else
    out.push_back(" at token ");
  out.push_back(p.tok);
  return string(range(out), string::concatenate);
}

mismatch::mismatch(detail::stream_position const &p, 
                   std::vector<string> const &t)
: tdl_exception(build_string(p, t)) {}

mismatch::~mismatch() throw() {}
