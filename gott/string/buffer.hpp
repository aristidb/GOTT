// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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
#include <gott/range_algo.hpp>

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
  string_buffer() : begin_(0), end_(0), storage_end_(0) {}

  /// Copy-Constructor.
  string_buffer(string_buffer const &b) : begin_(0), end_(0), storage_end_(0) {
    *this += b;
  }

  /**
   * Construct buffer from string.
   * \param x The string to construct the buffer from.
   */
  string_buffer(string const &x) : begin_(0), end_(0), storage_end_(0) {
    *this += x;
  }
  
  /**
   * Construct buffer from string.
   * \param x The utf8 character range to construct the buffer from.
   */
  string_buffer(range_t<utf32_t const *> const &x) 
  : begin_(0), end_(0), storage_end_(0) {
    insert(end(), x.begin(), x.end());
  }

  /// Destructor.
  ~string_buffer() { if (begin_) delete [] begin_; }

  /**
   * Return an iterator pointing to the beginning of the represented string.
   */
  iterator begin() { 
    return begin_;
  }

  /**
   * Return an iterator pointing after the end of the represented string.
   */
  iterator end() {
    return end_;
  }
  
  /**
   * Return an iterator pointing to the beginning of the represented string.
   */
  const_iterator begin() const {
    return begin_;
  }

  /**
   * Return an iterator pointing after the end of the represented string.
   */ 
  const_iterator end() const {
    return end_;
  }

  /**
   * Return the size (and length) of the represented string.
   */
  size_t size() const {
    return end_ - begin_;
  }

  /**
   * Swap contents with another buffer.
   */
  void swap(string_buffer &o) {
    std::swap(begin_, o.begin_);
    std::swap(end_, o.end_);
    std::swap(storage_end_, o.storage_end_);
  }

  /**
   * Assign another buffers contents to this buffer.
   */
  string_buffer &operator=(string_buffer const &b) {
    string_buffer(b).swap(*this);
    return *this;
  }

  /**
   * Return the character at a certain position of the buffer.
   */
  utf32_t &operator[](std::size_t i) {
    return begin_[i];
  }

  /**
   * Return the character at a certain position of the buffer.
   */
  utf32_t operator[](std::size_t i) const {
    return begin_[i];
  }

  /**
   * Erase a range from the buffer.
   * \param r The range to erase.
   * \return Iterator pointing just after the erased range.
   */
  iterator erase(range_t<iterator> const &r) {
    iterator a = r.begin();
    iterator b = r.end();

    while (b != end_)
      *a++ = *b++;
    end_ = a;
    return r.begin();
  }

  iterator erase(iterator i1, iterator i2) {
    return erase(range(i1, i2));
  }

  /**
   * Insert space of specified length before a specified iterator.
   * \param pos The iterator to insert before.
   * \param len The size of the inserted chunk.
   * \return The inserted chunk.
   */
  range_t<iterator> insert(iterator pos, std::size_t len) {
    if (len != 0) {
      if (std::size_t(storage_end_ - end_) < len) {
        std::size_t pp = pos - begin_;
        ensure(len);
        pos = begin_ + pp;
      } else
        end_ += len;
      for (iterator it = end_ - 1; it >= pos + len; --it)
        *it = *(it - len);
    }
    return range(pos, len);
  }

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
  void operator+=(string const &s) {
    gott::copy(s.as_utf32(), append(s.length()));
  }

  /**
   * Append a string_buffer.
   */
  void operator+=(string_buffer const &b) {
    gott::copy(range(b), append(b.size()));
  }

  void operator+=(utf32_t ch) {
    *append(1).Begin = ch;
  }

private:
  utf32_t *begin_;
  utf32_t *end_;
  utf32_t *storage_end_;

  void ensure(std::size_t add_len) {
    std::size_t old_len = end_ - begin_;
    if (std::size_t(storage_end_ - end_) < add_len)
      grow(old_len + add_len);
    end_ = begin_ + old_len + add_len;
  }

  void grow(std::size_t length) {
    std::size_t new_size = 1;
    while (new_size < length) 
      new_size <<= 1;
    utf32_t *old = begin_;
    begin_ = new utf32_t[new_size];
    utf32_t *out = begin_;
    for (utf32_t *pos = old; pos != end_; ++pos)
      *out++ = *pos;
    delete [] old;
    storage_end_ = begin_ + new_size;
  }
};

}

#endif
