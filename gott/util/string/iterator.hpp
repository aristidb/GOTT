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
#include <gott/util/visibility.hpp>

namespace gott {

/**
 * Iterator for full character steps over a (constant!) UTF8 string.
 */
class GOTT_EXPORT utf8_iterator {
public:
  typedef utf32_t value_type;
  typedef utf32_t *pointer;
  typedef utf32_t &reference;
  typedef std::ptrdiff_t difference_type;
  typedef std::bidirectional_iterator_tag iterator_category;

  /**
   * Return the character at the current position as UTF32.
   */
  utf32_t operator*() const;

  /**
   * Increment the position by a full character step. Prefix variant.
   */
  utf8_iterator &operator++();

  /**
   * Increment the position by a full character step. Postfix variant.
   * \param IGNORE Supplied by compiler.
   */
  utf8_iterator operator++(int IGNORE);

  /**
   * Decrement the position by a full character step. Prefix variant.
   */
  utf8_iterator &operator--();

  /**
   * Decrement the position by a full character step. Postfix variant.
   * \param IGNORE Supplied by compiler.
   */
  utf8_iterator operator--(int IGNORE); // postfix

  /**
   * Construct from pointer to UTF8-formatted memory.
   */
  utf8_iterator(utf8_t const *c) : current(c) {}

  /**
   * Retrieve the current memory position.
   */
  operator utf8_t const *() const;

private:
  utf8_t const *current;
};

/**
 * Compare two utf8_iterators for equality.
 */
GOTT_EXPORT bool operator==(utf8_iterator const &, utf8_iterator const &);

/**
 * Compare two utf8_iterators for inequality.
 */
GOTT_EXPORT bool operator!=(utf8_iterator const &, utf8_iterator const &);

GOTT_EXPORT bool operator<(utf8_iterator const &, utf8_iterator const &);

}
#endif
