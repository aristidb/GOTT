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

#ifndef GOTT_UTIL_NSTRING_BUFFER_HPP
#define GOTT_UTIL_NSTRING_BUFFER_HPP

#include "nstring.hpp"
#include "types.hpp"

namespace gott {

template<class> class range_t;

/**
 * Buffer for editable UTF32-strings.
 */
class GOTT_EXPORT nstring_buffer {
public:
  typedef utf32_t value_type;
  typedef utf32_t *iterator;
  typedef utf32_t const *const_iterator;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  
  /**
   * Construct empty buffer.
   */
  nstring_buffer();

  /// Copy-Constructor.
  nstring_buffer(nstring_buffer const &);

  /**
   * Construct buffer from nstring.
   */
  nstring_buffer(nstring const &);
  
  /**
   * Construct buffer from string.
   */
  nstring_buffer(range_t<utf32_t const *> const &);

  /// Destructor.
  ~nstring_buffer();

  /**
   * Return an iterator pointing to the beginning of the represented string.
   */
  iterator begin();

  /**
   * Return an iterator pointing after the end of the represented string.
   */
  iterator end();
  
  /**
   * Return an iterator pointing to the beginning of the represented string.
   */
  const_iterator begin() const;

  /**
   * Return an iterator pointing after the end of the represented string.
   */ 
  const_iterator end() const;

  /**
   * Return the size (and length) of the represented string.
   */
  size_t size() const;
  
  /**
   * Swap contents with another buffer.
   */
  void swap(nstring_buffer &);

  /**
   * Assign another buffers contents to this buffer.
   */
  void operator=(nstring_buffer const &b);

  /**
   * Return the character at a certain position of the buffer.
   */
  utf32_t &operator[](std::size_t);

  /**
   * Return the character at a certain position of the buffer.
   */
  utf32_t &at(std::size_t);
  
  /**
   * Erase a range from the buffer.
   * \param r The range to erase.
   */
  void erase(range_t<iterator> const &r);

  /**
   * Insert space of specified length before a specified iterator.
   * \param pos The iterator to insert before.
   * \param len The size of the inserted chunk.
   * \return The inserted chunk.
   */
  range_t<iterator> insert(iterator pos, std::size_t len);
  
private:
  class representation;
  representation *data;
};

}

#endif
