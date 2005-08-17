// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Text storage and manipulation
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

#include "stl.hpp"
#include "string.hpp"
#include "convert.hpp"
#include <gott/util/range.hpp>

using gott::string;

std::string gott::to_string(string const &str, encoding enc) {
  range_t<char const *> out = to_enc_alloc(str.as_utf8(), enc);
  std::string result(out.begin, out.end);
  delete [] out.begin;
  return result;
}

std::wstring gott::to_wstring(string const &str, encoding enc) {
  range_t<char const *> precast_out = to_enc_alloc(str.as_utf8(), enc);
  range_t<wchar_t const *> out = precast_out.cast<wchar_t const *>();
  std::wstring result(out.begin, out.end);
  delete [] out.begin;
  return result;
}
