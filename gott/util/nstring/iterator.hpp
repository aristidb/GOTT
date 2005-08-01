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

#ifndef GOTT_UTIL_NSTRING_ITERATOR_HPP
#define GOTT_UTIL_NSTRING_ITERATOR_HPP

#include "types.hpp"
#include <iterator>

namespace gott {

class utf8_iterator {
public:
  typedef utf32_t value_type;
  typedef utf32_t *pointer;
  typedef utf32_t &reference;
  typedef std::ptrdiff_t difference_type;
  typedef std::input_iterator_tag iterator_category;

  utf32_t operator*() const;
  utf8_iterator &operator++();

  utf8_iterator(utf8_t const *c) : current(c) {}

private:
  utf8_t const *current;
};

bool operator==(utf8_iterator const &, utf8_iterator const &);
bool operator!=(utf8_iterator const &, utf8_iterator const &);

}
#endif
