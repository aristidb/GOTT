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
