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

#include "nstring.hpp"
#include "convert.hpp"

#include <cstdlib>
#include <cwchar>

using gott::nstring;

class nstring::representation {
public:
  representation(utf8_t *d, std::size_t s) 
  : ref_count(1), size(s), length(0), data(d) {}


  std::size_t ref_count;
  std::size_t size;
  std::size_t length;
  utf8_t *data;
};

nstring::nstring(char const *in, encoding enc)
: data(new representation(to_utf8_alloc(in, enc), std::strlen(in))) {}

nstring::nstring(wchar_t const *in, encoding enc)
: data(new representation(to_utf8_alloc(in, enc), std::wcslen(in))) {}
