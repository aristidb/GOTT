// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Text storage and manipulation
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

#ifndef GOTT_UTIL_NSTRING_NSTRING_HPP
#define GOTT_UTIL_NSTRING_NSTRING_HPP

#include "iterator.hpp"

namespace gott {

class nstring {
public:
  nstring(char const *, encoding = utf8);
  nstring(std::string const &, encoding = utf8);
  nstring(wchar_t const *, encoding = utf32);
  nstring(std::wstring const &, encoding = utf32);

  operator std::string() const;
  operator std::wstring() const;
 
  nstring_iterator begin() const;
  nstring_iterator end() const;

  std::size_t length() const;

  std::size_t size() const;

private:
  class representation;
  representation *data;
};

nstring operator+(nstring const &, nstring const &);

}

#endif
