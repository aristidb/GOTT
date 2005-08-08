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
#include "nstring.hpp"
#include "convert.hpp"
#include <gott/util/range.hpp>

using gott::nstring;
using std::string;
using std::wstring;

string gott::to_string(nstring const &str, encoding enc) {
  range_t<char const *> out = to_enc_alloc(range(str), enc);
  string result(out.begin, out.end);
  delete [] out.begin;
  return result;
}

wstring gott::to_wstring(nstring const &str, encoding enc) {
  range_t<char const *> precast_out = to_enc_alloc(range(str), enc);
  range_t<wchar_t const *> out = precast_out.cast<wchar_t const *>();
  wstring result(out.begin, out.end);
  delete [] out.begin;
  return result;
}

nstring gott::to_nstring(string const &str, encoding enc) {
  return nstring(range(&str[0], &str[str.length()]), enc);
}

nstring gott::to_nstring(wstring const &str, encoding enc) {
  return nstring(range(&str[0], &str[str.length()]), enc);
}
