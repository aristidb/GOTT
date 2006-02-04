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

#include "convert.hpp"
#include "iterator.hpp"
#include <gott/range_algo.hpp>

using gott::utf32_t;
using gott::utf8_t;

utf32_t gott::to_utf32_char(char const *mbc, char const *&end, encoding enc) {
  switch (enc) {
  case ascii:
    end = mbc + 1;
    return utf32_t(*mbc);
  case utf8: {
    utf8_iterator it((utf8_t const *) mbc);
    utf8_iterator next = it;
    end = (char const *) (++next).ptr();
    return *it;
  }
  case utf32:
    end = mbc + 4;
    return * (utf32_t *) mbc;
  default:
    throw 0;
  }
}

void gott::write_utf32_to_utf8(utf32_t ch, utf8_t *&out) {
  utf8_t const mask6 = ~(~utf8_t() << 6);
  if (ch < 0x80) {
    *out++ = utf8_t(ch);
  } else if (ch < 0x800) {
    *out++ = 0xC0 | utf8_t(ch >> 6);
    *out++ = 0x80 | (utf8_t(ch) & mask6);
  } else if (ch < 0x10000) {
    *out++ = 0xE0 | utf8_t(ch >> 12);
    *out++ = 0x80 | (utf8_t(ch >> 6) & mask6);
    *out++ = 0x80 | (utf8_t(ch) & mask6);
  } else {
    *out++ = 0xF0 | utf8_t(ch >> 18);
    *out++ = 0x80 | (utf8_t(ch >> 12) & mask6);
    *out++ = 0x80 | (utf8_t(ch >> 6) & mask6);
    *out++ = 0x80 | (utf8_t(ch) & mask6);
  }
}

void gott::write_utf32_to_enc(utf32_t ch, char *&out, encoding enc) {
  switch (enc) {
  case utf32:
    *(utf32_t *)out = ch;
    out += 4;
    break;
  case utf8:
    write_utf32_to_utf8(ch, (utf8_t *&)out);
    break;
  case ascii:
    *out++ = char(ch);
    break;
  default:
    throw 0;
  }
}

std::size_t gott::utf8_len(range_t<char const *> const &in, encoding enc) {
  switch (enc) {
  case ascii:
  case utf8:
    return in.size();
  case utf32: {
    range_t<utf32_t const *> uss = in.cast<utf32_t const *>();
    utf32_t const *us = uss.begin();
    std::size_t result = 0;
    for (; us < uss.end(); ++us)
      result += utf8_len(*us);
    return result;
  }
  default: {
    char const *current = in.begin(), *next;
    std::size_t result = 0;
    while (current < in.end()) {
      wchar_t ch = to_utf32_char(current, next, enc);
      result += utf8_len(ch);
      current = next;
    }
    return result;
  }
  }
}

std::size_t gott::utf32_len(range_t<char const *> const &s, encoding enc) {
  switch (enc) {
  case ascii:
  case iso8859_1: case iso8859_2: case iso8859_3: case iso8859_4:
  case iso8859_5: case iso8859_6: case iso8859_7: case iso8859_8:
  case iso8859_9: case iso8859_10: case iso8859_11: case iso8859_12:
  case iso8859_13: case iso8859_14: case iso8859_15: case iso8859_16:
    return s.size();
  case utf32be: case utf32le:
    return s.cast<wchar_t const *>().size();
  default:
    throw 0;
  }
}

std::size_t gott::utf8_len(utf32_t ch) {
  if (ch < 0x80)
    return 1;
  if (ch < 0x800)
    return 2;
  else if (ch < 0x10000)
    return 3;
  else
    return 4;
}

std::size_t gott::enc_len(utf32_t ch, encoding enc) {
  switch (enc) {
  case utf32be: case utf32le:
    return 4;
  case utf8:
    return utf8_len(ch);
  case ascii:
  case iso8859_1: case iso8859_2: case iso8859_3: case iso8859_4:
  case iso8859_5: case iso8859_6: case iso8859_7: case iso8859_8:
  case iso8859_9: case iso8859_10: case iso8859_11: case iso8859_12:
  case iso8859_13: case iso8859_14: case iso8859_15: case iso8859_16:
    return 1;
  default:
    throw 0;
  }
}

gott::range_t<utf8_t *> 
gott::to_utf8_alloc(range_t<char const *> const &in, encoding enc) {
  std::size_t len = utf8_len(in, enc);

  utf8_t *result = new utf8_t[len];

  if (enc == utf8) {
    utf8_t *out = result;
    for (char const *it = in.begin(); it != in.end(); ++it)
      *out++ = *it;
    return range(result, result + len);
  }

  utf8_t *out = result;
  char const *current = in.begin(), *next;
  while (current < in.end()) {
    write_utf32_to_utf8(to_utf32_char(current, next, enc), out);
    current = next;
  }
  return range(result, out);
}

gott::range_t<char *>
gott::to_enc_alloc(range_t<utf8_t const *> const &in_, encoding enc) {
  if (enc == utf8) {
    utf8_t *result = new utf8_t[in_.size()], *out = result;
    for (utf8_t const *it = in_.begin(); it != in_.end(); ++it)
      *out++ = *it;
    return range(result, in_.size()).cast<char *>();
  }

  range_t<utf8_iterator> in = in_.cast<utf8_iterator>();

  std::size_t len = 0;
  for (utf8_iterator it = in.begin(); it < in.end(); ++it)
    len += enc_len(*it, enc);

  char *result = new char[len];
  char *out = result;
  for (utf8_iterator it = in.begin(); it < in.end(); ++it)
    write_utf32_to_enc(*it, out, enc);
  return range(result, out);
}
