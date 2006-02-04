/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is A String and Text Storage Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

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
   * \param x The string to construct the buffer from.
   */
  string_buffer(string const &x) : data(0) {
    string_buffer empty;
    this->swap(empty);
    *this += x;
  }
  
  /**
   * Construct buffer from string.
   * \param x The utf8 character range to construct the buffer from.
   */
  string_buffer(range_t<utf32_t const *> const &x) : data(0) {
    string_buffer empty;
    this->swap(empty);
    insert(end(), x.begin(), x.end());
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
    iterator out = insert(pos, len).Begin;
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
    *append(1).Begin = ch;
  }

private:
  class representation;
  representation *data;
};

}

#endif
