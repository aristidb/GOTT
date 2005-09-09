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
#include "stl.hpp"
#include <gott/util/visibility.hpp>
#include <gott/util/range.hpp>

#ifndef NO_NTL
#include <ntl.h>
#endif

#ifndef NO_STDLIB
#include <iosfwd>

#ifdef _MSC_VER
#include <string>
#include <vector>
#include <list>
#include <ostream>
#include <gott/util/thunk.hpp>
#else
namespace std {
  template<class> class allocator;
  template<class> class char_traits;
  template<class, class> class vector;
  template<class, class> class list;
  template<class, class, class> class basic_string;
  typedef basic_string<char, char_traits<char>, allocator<char> > string;
  typedef basic_string<wchar_t,char_traits<wchar_t>,allocator<wchar_t> >wstring;
}
#endif
#endif

namespace gott {

template<class> struct thunk_t;

class string_buffer;
template<class> class range_t;

/**
 * UTF-8 string literals storage class.
 */
class GOTT_EXPORT string : Moveable<string> {
public:
  typedef range_t<utf8_t const *> utf8_range;
  typedef range_t<utf8_iterator> utf32_range;

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

#ifndef NO_STDLIB
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
  operator std::string() const {
    return to_string(*this);
  }

  /**
   * Construct std::wstring from string using unicode encoding.
   * Use to_wstring if you need a different encoding.
   */
  operator std::wstring() const {
    return to_wstring(*this);
  }
#endif
  
  enum literal_tag { utf8_literal };

  /**
   * Construct from UTF8-c-literal. Shares memory.
   */
  string(range_t<utf8_t const *>, literal_tag);

  /**
   * Construct from string_buffer.
   */
  string(string_buffer const &);

#ifndef NO_STDLIB
  /**
   * Concatenate.
   */
  string(std::vector<string, std::allocator<string> > const &);

  /**
   * Concatenate.
   */
  string(std::list<string, std::allocator<string> > const &);
#endif

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
  void operator=(string other) GOTT_LOCAL {
    other.swap(*this);
  }

  /**
   * Get the internally used UTF8 string.
   */
  range_t<utf8_t const *> as_utf8() const;

  /**
   * Access the string as UTF32.
   */
  range_t<utf8_iterator> as_utf32() const GOTT_LOCAL {
    return as_utf8().cast<utf8_iterator>();
  }

  /**
   * Get the number of characters (not bytes) in the string.
   */
  std::size_t length() const;

  /**
   * Get the number of bytes the string needs as UTF8-encoded.
   */
  std::size_t size() const GOTT_LOCAL {
    return as_utf8().size();
  }

private:
  class representation;
  representation *p;
};

#ifndef NO_STDLIB
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
#endif

/**
 * Concatenate two strings.
 */
GOTT_LOCAL inline string operator +(string const &a, string const &b) {
  string const arr[2] = {a, b};
  return range(arr);
}

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

#ifndef NO_NTL
template<>
inline unsigned GetHashValue(gott::string const &s) {
  gott::range_t<gott::utf8_t const *> r = s.as_utf8();
  return memhash(r.begin, r.size());
}
#endif

#endif
