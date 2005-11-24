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
#include "convert.hpp"
#include "buffer.hpp"
#include <gott/visibility.hpp>
#include <gott/range.hpp>

#ifndef NO_NTL
#include <ntl.h>
#endif

#ifndef NO_STDLIB
#include <iosfwd>
#include <string>

#ifdef _MSC_VER
#include <gott/thunk.hpp>
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
  GOTT_LOCAL string() { 
    set_up(offset(range("").cast<utf8_t const*>(), 0, -1), false);
  }

  /**
   * Construct from encoded string.
   */
  GOTT_LOCAL string(range_t<char const *> in, encoding enc = utf8) {
    set_up(to_utf8_alloc(in, enc), true);
  }

  /**
   * Construct from encoded c-string.
   */
  GOTT_LOCAL string(char const *in, encoding enc = utf8) {
    set_up(to_utf8_alloc(zero_terminated(in), enc), true);
  }

  /**
   * Construct from encoded wide string.
   */
  GOTT_LOCAL string(range_t<wchar_t const *> in, encoding enc = utf32) {
    set_up(to_utf8_alloc(in.cast<char const *>(), enc), true);
  }

  /**
   * Construct from encoded wide c-string.
   */
  GOTT_LOCAL string(wchar_t const *in, encoding enc = utf32) {
    set_up(to_utf8_alloc(zero_terminated(in).cast<char const *>(), enc), true);
  }

  /**
   * Construct string from thunk.
   */
  string(thunk_t<utf8_t> &);

#ifndef NO_STDLIB
  /**
   * Construct string from std::string.
   */
  GOTT_LOCAL string(std::string const &s, encoding enc = utf8) {
    set_up(to_utf8_alloc(range(&s[0], s.length()), enc), true);
  }

  /**
   * Construct string from std::wstring.
   */
  GOTT_LOCAL string(std::wstring const &s, encoding enc = utf32) {
    set_up(to_utf8_alloc(range(&s[0], s.length()).cast<char const *>(), enc), 
        true);
  }

  /**
   * Construct std::string from string using unicode encoding.
   * Use to_string() if you need a different encoding.
   */
  GOTT_LOCAL operator std::string() const {
    return to_string(*this);
  }

  /**
   * Construct std::wstring from string using unicode encoding.
   * Use to_wstring() if you need a different encoding.
   */
  GOTT_LOCAL operator std::wstring() const {
    return to_wstring(*this);
  }
#endif
  
  enum literal_tag { utf8_literal };

  /**
   * Construct from UTF8-c-literal. Shares memory.
   */
  GOTT_LOCAL string(range_t<utf8_t const *> in, literal_tag) {
    set_up(in, false);
  }

  /**
   * Construct from string_buffer.
   */
  GOTT_LOCAL string(string_buffer const &b) {
    set_up(to_utf8_alloc(range(b).cast<char const *>(), utf32), true);
  }

  enum concat_tag { concatenate };

  /**
   * Concatenate.
   */
  template<class I>
  GOTT_LOCAL string(range_t<I> const &r, concat_tag) {
    std::size_t size = 0;
    for (I it = r.Begin; it != r.End; ++it)
      size += it->size();
    utf8_t *current = new utf8_t[size];
    set_up(range(current, size), true);
    for (I it = r.Begin; it != r.End; ++it) {
      utf8_range r = it->as_utf8();
      while (!r.empty())
        *current++ = *r.Begin++;
    }
  }

  /**
   * Construct from character range.
   */
  GOTT_LOCAL string(range_t<utf8_iterator> const &r) {
    foreign(r.call<utf8_t const *>(&utf8_iterator::ptr));
  }
  
  /**
   * Construct from character range.
   */
  GOTT_LOCAL string(range_t<utf8_t const *> const &r) {
    foreign(r);
  }

  /// Copy-Constructor.
  string(string const &);

  /// Destructor.
  ~string();

  /**
   * Swap with another string.
   */
  GOTT_LOCAL void swap(string &other) {
    class representation *tmp = p;
    p = other.p;
    other.p = tmp;
  }

  /**
   * Assign from another string.
   */
  GOTT_LOCAL void operator=(string other) {
    other.swap(*this);
  }

  /**
   * Get the internally used UTF8 string.
   */
  range_t<utf8_t const *> as_utf8() const;

  /**
   * Access the string as UTF32.
   */
  GOTT_LOCAL range_t<utf8_iterator> as_utf32() const {
    return as_utf8().cast<utf8_iterator>();
  }

  /**
   * Get the number of characters (not bytes) in the string.
   */
  std::size_t length() const;

  /**
   * Get the number of bytes the string needs as UTF8-encoded.
   */
  GOTT_LOCAL std::size_t size() const {
    return as_utf8().size();
  }

private:
  class representation;
  representation *p;

  void set_up(range_t<utf8_t const *> const &d, bool o);
  void foreign(range_t<utf8_t const *> const &x);
};

#ifndef NO_STDLIB
/**
 * Print out a string.
 * @relates string
 */
GOTT_EXPORT 
std::basic_ostream<char, std::char_traits<char> > &
operator<<(std::basic_ostream<char, std::char_traits<char> > &, 
           string const &);

/**
 * Print out a string.
 * @relates string
 */
GOTT_EXPORT 
std::basic_ostream<wchar_t, std::char_traits<wchar_t> > &
operator<<(std::basic_ostream<wchar_t, std::char_traits<wchar_t> > &, 
           string const &);
#endif

/**
 * Concatenate two strings.
 * @relates string
 */
GOTT_LOCAL inline string operator +(string const &a, string const &b) {
  string const arr[2] = {a, b};
  return string(range(arr), string::concatenate);
}

/**
 * Compare two strings for equality.
 * @relates string
 */
GOTT_EXPORT bool operator==(string const &, string const &);

/**
 * Compare two strings for inequality.
 * @relates string
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
 * @relates string
 */
GOTT_EXPORT int compare(string const &a, string const &b);

/**
 * Compare two strings lexicographically,
 * \param a,b Strings to be compared.
 * \return Whether @p a is smaller than @p b.
 * @relates string
 */
inline bool operator<(string const &a, string const &b) {
  return compare(a, b) < 0;
}

/**
 * Compare two strings lexicographically,
 * \param a,b Strings to be compared.
 * \return Whether @p a is smaller than or equal to @p b.
 * @relates string
 */
inline bool operator<=(string const &a, string const &b) {
  return compare(a, b) <= 0;
}

/**
 * Compare two strings lexicographically,
 * \param a,b Strings to be compared.
 * \return Whether @p a is greater than @p b.
 * @relates string
 */
inline bool operator>(string const &a, string const &b) {
  return compare(a, b) > 0;
}

/**
 * Compare two strings lexicographically,
 * \param a,b Strings to be compared.
 * \return Whether @p a is greater than or equal to @p b.
 * @relates string
 */
inline bool operator>=(string const &a, string const &b) {
  return compare(a, b) >= 0;
}

}

#ifndef NO_NTL
template<>
inline unsigned GetHashValue(gott::string const &s) {
  gott::range_t<gott::utf8_t const *> r = s.as_utf8();
  return memhash(r.Begin, r.size());
}
#endif

#endif
