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

#include "iterator.hpp"

using gott::utf8_iterator;

gott::utf32_t utf8_iterator::operator*() const {
  utf8_t const mask6 = ~(~utf8_t() << 6);
  utf8_t const mask5 = ~(~utf8_t() << 5);
  utf8_t const mask4 = ~(~utf8_t() << 4);
  if (current[0] < 0xC0)
    return current[0];
  else if (current[0] < 0xE0)
    return (utf32_t(current[0] & mask6) << 6) 
            | utf32_t(current[1] & mask6);
  else if (current[0] < 0xF0)
    return (utf32_t(current[0] & mask5) << 12) 
            | (utf32_t(current[1] & mask6) << 6)
            | utf32_t(current[2] & mask6);
  else if (current[0] < 0xF8)
    return utf32_t(current[0] & mask4) << 18 
            | (utf32_t(current[1] & mask6) << 12)
            | (utf32_t(current[2] & mask6) << 6)
            | utf32_t(current[3] & mask6);
  return 0;
}

utf8_iterator &utf8_iterator::operator++() {
  if (*current++ >= 0xC0)
    while (*current >= 0x80 && *current < 0xC0)
      ++current;
  return *this;
}

utf8_iterator utf8_iterator::operator++(int) {
  utf8_iterator tmp(*this);
  ++*this;
  return tmp;
}

utf8_iterator &utf8_iterator::operator--() {
  while (*current >= 0x80 && *current < 0xC0)
    --current;
  --current;
  return *this;
}

utf8_iterator utf8_iterator::operator--(int) {
  utf8_iterator tmp(*this);
  --*this;
  return tmp;
}

utf8_iterator::operator utf8_t const *() const {
  return current;
}

bool gott::operator==(utf8_iterator const &a, utf8_iterator const &b) {
  return (utf8_t const *) a == (utf8_t const *) b;
}

bool gott::operator!=(utf8_iterator const &a, utf8_iterator const &b) {
  return !(a == b);
}

bool gott::operator<(utf8_iterator const &a, utf8_iterator const &b) {
  return (utf8_t const *) a < (utf8_t const *) b;
}
