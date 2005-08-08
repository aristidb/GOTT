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

#ifndef GOTT_UTIL_NSTRING_NSTRING_HPP
#define GOTT_UTIL_NSTRING_NSTRING_HPP

#include "iterator.hpp"
#include "types.hpp"
#include <gott/util/visibility.hpp>
#define PARAM_HASH_NO_BASIC_STRING
#include <gott/util/my_hash_map.hpp>
#undef PARAM_HASH_NO_BASIC_STRING

namespace std {
  template<class> class allocator;
  template<class> class char_traits;
  template<class, class> class vector;
  template<class, class> class list;
  template<class, class> class basic_ostream;
}

namespace gott {

class nstring_buffer;
template<class> class range_t;

/**
 * UTF-8 string literals storage class.
 */
class nstring {
public:
  typedef utf8_iterator const_iterator;

  /**
   * Construct from encoded c-string.
   */
  nstring(range_t<char const *>, encoding = utf8) GOTT_EXPORT;
  
  /**
   * Construct from encoded wide c-string.
   */
  nstring(range_t<wchar_t const *>, encoding = utf32) GOTT_EXPORT;
  
  enum literal_tag { utf8_literal };

  /**
   * Construct from UTF8-c-literal. Shares memory.
   */
  nstring(range_t<utf8_t const *>, literal_tag) GOTT_EXPORT;

  /**
   * Construct from nstring_buffer.
   */
  nstring(nstring_buffer const &) GOTT_EXPORT;

  /**
   * Concatenate.
   */
  nstring(std::vector<nstring, std::allocator<nstring> > const &) GOTT_EXPORT;

  /**
   * Concatenate.
   */
  nstring(std::list<nstring, std::allocator<nstring> > const &) GOTT_EXPORT;

  /**
   * Concatenate.
   */
  nstring(range_t<nstring const *>) GOTT_EXPORT;

  /**
   * Construct from character range.
   */
  nstring(range_t<utf8_iterator> const &) GOTT_EXPORT;
  
  /**
   * Construct from character range.
   */
  nstring(range_t<utf8_t const *> const &) GOTT_EXPORT;

  /// Copy-Constructor.
  nstring(nstring const &) GOTT_EXPORT;

  /// Destructor.
  ~nstring() GOTT_EXPORT;

  /**
   * Swap with another nstring.
   */
  void swap(nstring &other) GOTT_EXPORT;

  /**
   * Assign from another nstring.
   */
  void operator =(nstring const &other) GOTT_EXPORT;

  /**
   * Get the internal data.
   */
  utf8_t const *data() const GOTT_EXPORT;

  utf8_t const *begin_raw() const GOTT_EXPORT;

  utf8_t const *end_raw() const GOTT_EXPORT;
 
  /**
   * Get an iterator pointing at the first character.
   */
  utf8_iterator begin() const GOTT_EXPORT;

  /**
   * Get an iterator pointing behind the last character.
   */
  utf8_iterator end() const GOTT_EXPORT;

  /**
   * Get the number of characters (not bytes) in the string.
   */
  std::size_t length() const GOTT_EXPORT;

  /**
   * Get the number of bytes the string needs as UTF8-encoded.
   */
  std::size_t size() const GOTT_EXPORT;

private:
  class representation;
  representation *p;

  GOTT_LOCAL nstring(); // unimplemented
};

/**
 * Print out a nstring.
 */
GOTT_EXPORT 
std::basic_ostream<wchar_t, std::char_traits<wchar_t> > &
operator<<(std::basic_ostream<wchar_t, std::char_traits<wchar_t> > &, 
           nstring const &);

/**
 * Concatenate two nstrings.
 */
GOTT_EXPORT nstring operator +(nstring const &, nstring const &);

/**
 * Compare two nstrings for equality.
 */
GOTT_EXPORT bool operator==(nstring const &, nstring const &);

/**
 * Compare two nstrings for inequality.
 */
inline bool operator!=(nstring const &a, nstring const &b) {
  return !(a == b);
}

/**
 * Compare two nstrings lexicographically.
 * \param a,b Strings to be compared.
 * \return 
 *   - == 0 : Both strings are equal.
 *   -  < 0 : @p a is lexicographically smaller than @p b.
 *   -  > 0 : @p a is lexicographically greater than @p b.
 */
GOTT_EXPORT int compare(nstring const &a, nstring const &b);

/**
 * Compare two nstrings lexicographically,
 * \param a,b Strings to be compared.
 * \return Whether @p a is smaller than @p b.
 */
inline bool operator<(nstring const &a, nstring const &b) {
  return compare(a, b) < 0;
}

/**
 * Compare two nstrings lexicographically,
 * \param a,b Strings to be compared.
 * \return Whether @p a is smaller than or equal to @p b.
 */
inline bool operator<=(nstring const &a, nstring const &b) {
  return compare(a, b) <= 0;
}

/**
 * Compare two nstrings lexicographically,
 * \param a,b Strings to be compared.
 * \return Whether @p a is greater than @p b.
 */
inline bool operator>(nstring const &a, nstring const &b) {
  return compare(a, b) > 0;
}

/**
 * Compare two nstrings lexicographically,
 * \param a,b Strings to be compared.
 * \return Whether @p a is greater than or equal to @p b.
 */
inline bool operator>=(nstring const &a, nstring const &b) {
  return compare(a, b) >= 0;
}

}

namespace HH_HASHD {
template<> struct hash<gott::nstring> {
  GOTT_EXPORT std::size_t operator() (gott::nstring const &) const;
};
}

#endif
