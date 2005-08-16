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
#include <gott/util/range.hpp>
#include <ntl.h>

namespace std {
  template<class> class allocator;
  template<class> class char_traits;
  template<class, class> class vector;
  template<class, class> class list;
  template<class, class> class basic_ostream;
  template<class, class, class> class basic_string;
  typedef basic_string<char, char_traits<char>, allocator<char> > string;
  typedef basic_string<wchar_t,char_traits<wchar_t>,allocator<wchar_t> >wstring;
}

namespace gott {

template<class> struct thunk_t;

class string_buffer;
template<class> class range_t;

/**
 * UTF-8 string literals storage class.
 */
class GOTT_EXPORT string : Moveable<string> {
public:
  typedef utf8_iterator const_iterator;

  /**
   * Construct empty string
   */
  string();

  /**
   * Construct from encoded string.
   */
  string(range_t<char const *>, encoding = utf8);

  /**
   * Construct from encoded c-string.
   */
  string(char const *, encoding = utf8);

  /**
   * Construct from encoded wide string.
   */
  string(range_t<wchar_t const *>, encoding = utf32);

  /**
   * Construct from encoded wide c-string.
   */
  string(wchar_t const *, encoding = utf32);

  /**
   * Construct string from thunk.
   */
  string(thunk_t<utf8_t> &);

  /**
   * Construct string from std::string.
   */
  string(std::string const &, encoding = utf8);

  /**
   * Construct string from std::wstring.
   */
  string(std::wstring const &, encoding = utf32);

  /**
   * Construct std::string from string using unicode encoding.
   * Use to_string if you need a different encoding.
   */
  operator std::string() const;

  /**
   * Construct std::wstring from string using unicode encoding.
   * Use to_wstring if you need a different encoding.
   */
  operator std::wstring() const;
  
  enum literal_tag { utf8_literal };

  /**
   * Construct from UTF8-c-literal. Shares memory.
   */
  string(range_t<utf8_t const *>, literal_tag);

  /**
   * Construct from string_buffer.
   */
  string(string_buffer const &);

  /**
   * Concatenate.
   */
  string(std::vector<string, std::allocator<string> > const &);

  /**
   * Concatenate.
   */
  string(std::list<string, std::allocator<string> > const &);

  /**
   * Concatenate.
   */
  string(range_t<string const *>);

  /**
   * Construct from character range.
   */
  string(range_t<utf8_iterator> const &);
  
  /**
   * Construct from character range.
   */
  string(range_t<utf8_t const *> const &);

  /// Copy-Constructor.
  string(string const &);

  /// Destructor.
  ~string();

  /**
   * Swap with another string.
   */
  void swap(string &other);

  /**
   * Assign from another string.
   */
  void operator =(string const &other);

  /**
   * Get the internal data.
   */
  utf8_t const *data() const;

  utf8_t const *begin_raw() const;

  utf8_t const *end_raw() const;

  GOTT_LOCAL
  range_t<utf8_t const *> raw() const {
    return range(begin_raw(), end_raw());
  }
 
  /**
   * Get an iterator pointing at the first character.
   */
  utf8_iterator begin() const;

  /**
   * Get an iterator pointing behind the last character.
   */
  utf8_iterator end() const;

  /**
   * Get the number of characters (not bytes) in the string.
   */
  std::size_t length() const;

  /**
   * Get the number of bytes the string needs as UTF8-encoded.
   */
  std::size_t size() const;

private:
  class representation;
  representation *p;
};

/**
 * Print out a string.
 */
GOTT_EXPORT 
std::basic_ostream<char, std::char_traits<char> > &
operator<<(std::basic_ostream<char, std::char_traits<char> > &, 
           string const &);

/**
 * Print out a string.
 */
GOTT_EXPORT 
std::basic_ostream<wchar_t, std::char_traits<wchar_t> > &
operator<<(std::basic_ostream<wchar_t, std::char_traits<wchar_t> > &, 
           string const &);

/**
 * Concatenate two strings.
 */
GOTT_EXPORT string operator +(string const &, string const &);

/**
 * Compare two strings for equality.
 */
GOTT_EXPORT bool operator==(string const &, string const &);

/**
 * Compare two strings for inequality.
 */
inline bool operator!=(string const &a, string const &b) {
  return !(a == b);
}

/**
 * Compare two strings lexicographically.
 * \param a,b Strings to be compared.
 * \return 
 *   - == 0 : Both strings are equal.
 *   -  < 0 : @p a is lexicographically smaller than @p b.
 *   -  > 0 : @p a is lexicographically greater than @p b.
 */
GOTT_EXPORT int compare(string const &a, string const &b);

/**
 * Compare two strings lexicographically,
 * \param a,b Strings to be compared.
 * \return Whether @p a is smaller than @p b.
 */
inline bool operator<(string const &a, string const &b) {
  return compare(a, b) < 0;
}

/**
 * Compare two strings lexicographically,
 * \param a,b Strings to be compared.
 * \return Whether @p a is smaller than or equal to @p b.
 */
inline bool operator<=(string const &a, string const &b) {
  return compare(a, b) <= 0;
}

/**
 * Compare two strings lexicographically,
 * \param a,b Strings to be compared.
 * \return Whether @p a is greater than @p b.
 */
inline bool operator>(string const &a, string const &b) {
  return compare(a, b) > 0;
}

/**
 * Compare two strings lexicographically,
 * \param a,b Strings to be compared.
 * \return Whether @p a is greater than or equal to @p b.
 */
inline bool operator>=(string const &a, string const &b) {
  return compare(a, b) >= 0;
}

}

namespace HH_HASHD {
template<> struct hash<gott::string> {
  GOTT_EXPORT std::size_t operator() (gott::string const &) const;
};
}

#endif