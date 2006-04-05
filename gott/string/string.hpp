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

#ifndef GOTT_UTIL_NSTRING_NSTRING_HPP
#define GOTT_UTIL_NSTRING_NSTRING_HPP

#include "iterator.hpp"
#include "types.hpp"
#include "stl.hpp"
#include "convert.hpp"
//#include "buffer.hpp"
#include <gott/visibility.hpp>
#include <gott/range.hpp>
#include <boost/shared_ptr.hpp>

#ifndef NO_GOTT_THUNK
#include <gott/thunk.hpp>
#endif

#ifndef NO_STDLIB
#include <iosfwd>
#include <string>
#endif

namespace gott {

class string_buffer;
template<class> class range_t;

/**
 * UTF-8 string literals storage class.
 */
class GOTT_EXPORT string {
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
  GOTT_LOCAL string(range_t<char const *> in, encoding enc) {
    set_up(to_utf8_alloc(in, enc), true);
  }

  /**
   * Construct from encoded string.
   */
  GOTT_LOCAL string(range_t<char *> in, encoding enc) {
    set_up(to_utf8_alloc(in, enc), true);
  }

  /**
   * Construct from encoded c-string.
   */
  GOTT_LOCAL string(char const *in, encoding enc) {
    set_up(to_utf8_alloc(zero_terminated(in), enc), true);
  }

  /**
   * Construct from encoded wide string.
   */
  GOTT_LOCAL string(range_t<wchar_t const *> in, encoding enc) {
    set_up(to_utf8_alloc(in.cast<char const *>(), enc), true);
  }

  /**
   * Construct from encoded wide string.
   */
  GOTT_LOCAL string(range_t<wchar_t *> in, encoding enc) {
    set_up(to_utf8_alloc(in.cast<char const *>(), enc), true);
  }

  /**
   * Construct from encoded wide c-string.
   */
  GOTT_LOCAL string(wchar_t const *in, encoding enc) {
    set_up(to_utf8_alloc(zero_terminated(in).cast<char const *>(), enc), true);
  }

#ifndef NO_STDLIB
  /**
   * Construct string from std::string.
   */
  GOTT_LOCAL string(std::string const &s, encoding enc) {
    set_up(to_utf8_alloc(range(&s[0], s.length()), enc), true);
  }

#ifdef HAVE_WIDE_STDLIB
  /**
   * Construct string from std::wstring.
   */
  GOTT_LOCAL string(std::wstring const &s, encoding enc) {
    set_up(to_utf8_alloc(range(&s[0], s.length()).cast<char const *>(), enc), 
        true);
  }
#endif

  /**
   * Construct std::string from string using unicode encoding.
   * Use to_string() if you need a different encoding.
   */
  GOTT_LOCAL operator std::string() const {
    return to_string(*this);
  }

#ifdef HAVE_WIDE_STDLIB
  /**
   * Construct std::wstring from string using unicode encoding.
   * Use to_wstring() if you need a different encoding.
   */
  GOTT_LOCAL operator std::wstring() const {
    return to_wstring(*this);
  }
#endif
#endif

#ifndef GOTT_NO_THUNK
  GOTT_LOCAL string(thunk_t<char> &thk, encoding enc = utf8) {
    if (enc == utf8)
      set_up(thk.consume_alloc().cast<utf8_t const *>(), true);
    else {
      range_t<char *> rng = thk.consume_alloc();
      set_up(to_utf8_alloc(rng, enc), true);
      delete [] rng.begin();
    }
  }

  GOTT_LOCAL string(thunk_t<wchar_t> &thk, encoding enc = wide) {
    range_t<wchar_t *> rng = thk.consume_alloc();
    set_up(to_utf8_alloc(rng.cast<char const *>(), enc), true);
    delete [] rng.begin();
  }

   GOTT_LOCAL string(thunk_t<utf32_t> &thk) {
    range_t<utf32_t *> rng = thk.consume_alloc();
    set_up(to_utf8_alloc(rng.cast<char const *>(), utf32), true);
    delete [] rng.begin();
  }
#endif
  
  enum literal_tag { 
    literal ///< Designates that a UTF8 string is a literal.
  };

  /**
   * Construct from UTF8-c-literal. Shares memory.
   */
  GOTT_LOCAL string(range_t<utf8_t const *> in, literal_tag) {
    set_up(in, false);
  }

  /**
   * Construct from UTF8-c-literal. Shares memory.
   */
  GOTT_LOCAL string(range_t<utf8_t *> in, literal_tag) {
    set_up(in, false);
  }

  template<int N>
  GOTT_LOCAL string(utf8_t const (&in)[N]) {
    set_up(range_t<utf8_t const *>(in, in + N - 1), false);
  }

  /**
   * Construct from string_buffer.
   */
  string(string_buffer const &b);

  enum concat_tag { 
    concatenate  ///< Designates that many strings should be concatenated.
  };

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

  /**
   * Construct from character range.
   */
  GOTT_LOCAL string(range_t<utf8_t *> const &r) {
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
    p.swap(other.p);
  }

  /**
   * Assign from another string.
   */
  GOTT_LOCAL void operator=(string other) {
    other.swap(*this);
  }

  /**
   * Assign from iterator range.
   */
  template<class I>
  void assign(I a, I b) {
    operator=(string(range(a, b)));
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

  /**
   * Get a copy of the string with zero-termination in a freshly allocated
   * buffer.
   */
  char *c_string_alloc() const;

private:
  class representation;
  //TODO: short string optimization
  //like: variant<shared_ptr<representation>, char[16]> p;
  boost::shared_ptr<representation> p;

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

#endif
