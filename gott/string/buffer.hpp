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

#include "types.hpp"
#include <gott/visibility.hpp>
#include <gott/range.hpp>
#ifndef NO_STDLIB
#include <stdexcept>
#endif

namespace gott {
class string;
template<class> class range_t;

/**
 * Buffer for editable UTF32-strings.
 */
class string_buffer {
public:
  typedef utf32_t value_type;
  typedef utf32_t *iterator;
  typedef utf32_t const *const_iterator;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  
  /**
   * Construct empty buffer.
   */
  string_buffer() GOTT_EXPORT;

  /// Copy-Constructor.
  string_buffer(string_buffer const &b) : data(0) {
    string_buffer empty;
    this->swap(empty);
    *this += b;
  }

  /**
   * Construct buffer from string.
   */
  string_buffer(string const &x) : data(0) {
    string_buffer empty;
    this->swap(empty);
    *this += x;
  }
  
  /**
   * Construct buffer from string.
   */
  string_buffer(range_t<utf32_t const *> const &x) : data(0) {
    string_buffer empty;
    this->swap(empty);
    insert(end(), x.begin, x.end);
  }

  /// Destructor.
  ~string_buffer() GOTT_EXPORT;

  /**
   * Return an iterator pointing to the beginning of the represented string.
   */
  iterator begin() { 
    return const_cast<iterator>(((string_buffer const *)this)->begin());
  }

  /**
   * Return an iterator pointing after the end of the represented string.
   */
  iterator end() {
    return const_cast<iterator>(((string_buffer const *)this)->end());
  }
  
  /**
   * Return an iterator pointing to the beginning of the represented string.
   */
  const_iterator begin() const GOTT_EXPORT;

  /**
   * Return an iterator pointing after the end of the represented string.
   */ 
  const_iterator end() const GOTT_EXPORT;

  /**
   * Return the size (and length) of the represented string.
   */
  size_t size() const GOTT_EXPORT;
  
  /**
   * Swap contents with another buffer.
   */
  void swap(string_buffer &o) {
    representation *tmp = data;
    data = o.data;
    o.data = tmp;
  }

  /**
   * Assign another buffers contents to this buffer.
   */
  void operator=(string_buffer const &b) {
    string_buffer(b).swap(*this);
  }

  /**
   * Return the character at a certain position of the buffer.
   */
  utf32_t &operator[](std::size_t i) {
    return begin()[i];
  }

#ifndef NO_STDLIB
  /**
   * Return the character at a certain position of the buffer.
   */
  utf32_t &at(std::size_t i) {
    if (i > size())
      throw std::out_of_range("string_buffer::at: index out of bounds");
  }
#endif
  
  /**
   * Erase a range from the buffer.
   * \param r The range to erase.
   * \return Iterator pointing just after the erased range.
   */
  iterator erase(range_t<iterator> const &r) GOTT_EXPORT;

  iterator erase(iterator i1, iterator i2) {
    return erase(range(i1, i2));
  }

  /**
   * Insert space of specified length before a specified iterator.
   * \param pos The iterator to insert before.
   * \param len The size of the inserted chunk.
   * \return The inserted chunk.
   */
  range_t<iterator> insert(iterator pos, std::size_t len) GOTT_EXPORT;

  template<class I>
  void insert(iterator pos, I a, I b) {
#ifndef NO_STDLIB
    std::size_t len = std::distance(a, b);
#else
    std::size_t len = b - a;
#endif
    iterator out = insert(pos, len).begin;
    while (a != b)
      *out++ = *a++;
  }

  /**
   * Append space of specified length.
   * \param len The size of the appended chunk.
   * \return The appended chunk.
   */
  range_t<iterator> append(std::size_t len) {
    return insert(end(), len);
  }

  /**
   * Append a string.
   */
  void operator+=(string const &s) GOTT_EXPORT;

  /**
   * Append a string_buffer.
   */
  void operator+=(string_buffer const &sb) GOTT_EXPORT;

  void operator+=(utf32_t ch) GOTT_LOCAL {
    *append(1).begin = ch;
  }

private:
  class representation;
  representation *data;
};

}

#endif
