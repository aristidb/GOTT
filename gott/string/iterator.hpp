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
#ifndef NO_STDLIB
#include <iterator>
#endif
#include <gott/visibility.hpp>

namespace gott {

/**
 * Iterator for full character steps over a (constant!) UTF8 string.
 */
class utf8_iterator {
public:
  typedef utf32_t value_type;
  typedef utf32_t *pointer;
  typedef utf32_t &reference;
  typedef std::ptrdiff_t difference_type;
#ifndef NO_STDLIB
  typedef std::bidirectional_iterator_tag iterator_category;
#endif

  /**
   * Return the character at the current position as UTF32.
   */
  utf32_t operator*() const {
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

  /**
   * Increment the position by a full character step. Prefix variant.
   */
  utf8_iterator &operator++() {
    if (*current++ >= 0xC0)
      while (*current >= 0x80 && *current < 0xC0)
        ++current;
    return *this;
  }

  /**
   * Increment the position by a full character step. Postfix variant.
   * \param IGNORE Supplied by compiler.
   */
  utf8_iterator operator++(int IGNORE) { (void) IGNORE;
    utf8_iterator tmp(*this);
    ++*this;
    return tmp;
  }

  /**
   * Decrement the position by a full character step. Prefix variant.
   */
  utf8_iterator &operator--() {
    while (*current >= 0x80 && *current < 0xC0)
      --current;
    --current;
    return *this;
  }

  /**
   * Decrement the position by a full character step. Postfix variant.
   * \param IGNORE Supplied by compiler.
   */
  utf8_iterator operator--(int IGNORE) { (void) IGNORE;
    utf8_iterator tmp(*this);
    --*this;
    return tmp;
  }

  utf8_iterator &operator+=(std::size_t len) {
    while (len--)
      ++*this;
    return *this;
  }

  utf8_iterator &operator+=(long len) {
    if (len > 0)
      *this += std::size_t(len);
    else
      *this -= -len;
    return *this;
  }

  template<class I>
  utf8_iterator &operator+=(I len) {
    return *this += long(len);
  }

  template<class I>
  utf8_iterator operator+(I len) const {
    utf8_iterator tmp(*this);
    tmp += len;
    return tmp;
  }

  utf8_iterator &operator-=(std::size_t len) {
    while (len--)
      --*this;
    return *this;
  }

  utf8_iterator operator-(std::size_t len) const {
    utf8_iterator tmp(*this);
    tmp -= len;
    return tmp;
  }

  /**
   * Construct from pointer to UTF8-formatted memory.
   */
  utf8_iterator(utf8_t const *c = 0) : current(c) {}

  /**
   * Retrieve the current memory position.
   */
  utf8_t const *ptr() const { return current; }

private:
  utf8_t const *current;
};

/**
 * Compare two utf8_iterators for equality.
 * \relates utf8_iterator
 */
inline bool operator==(utf8_iterator a, utf8_iterator b) {
  return a.ptr() == b.ptr();
}

/**
 * Compare two utf8_iterator objects for inequality.
 * \relates utf8_iterator
 */
inline bool operator!=(utf8_iterator a, utf8_iterator b) {
  return !(a == b);
}

/**
 * Compares two utf8_iterator objects.
 * \relates utf8_iterator
 */
inline bool operator<(utf8_iterator a, utf8_iterator b) {
  return a.ptr() < b.ptr();
}

/**
 * Compares two utf8_iterator objects.
 * \relates utf8_iterator
 */
inline bool operator>(utf8_iterator a, utf8_iterator b) {
  return a.ptr() > b.ptr();
}

/**
 * Substract two utf8_iterator objects.
 * \param a The utf8_iterator to substract from.
 * \param b The utf8_iterator to substract.
 * \return The difference in complete characters.
 * \relates utf8_iterator
 */
inline std::ptrdiff_t operator-(utf8_iterator a, utf8_iterator b) {
  if (a < b)
    return -(b - a);
  std::ptrdiff_t result = 0;
  while (a > b) {
    --a;
    ++result;
  }
  return result;
}

}
#endif
