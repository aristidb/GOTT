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

#include "convert.hpp"
#include "iterator.hpp"
#include <iostream>

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
    end = (char const *) (utf8_t const *) ++next;
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

utf8_t *gott::to_utf8_alloc(char const *in, encoding enc) {
  std::size_t len = utf8_len(in, enc);
  utf8_t *result = new utf8_t[len];
  if (enc == utf8) {
    std::copy(in, in + len, result);
    return result;
  }
  utf8_t *out = result;
  char const *next;
  while (utf32_t ch = to_utf32_char(in, next, enc)) {
    write_utf32_to_utf8(ch, out);
    in = next;
  }
  return result;
}

std::size_t gott::utf8_len(char const *in, encoding enc) {
  switch (enc) {
  case ascii:
  case utf8:
    return std::strlen(in);
  case utf32: {
    utf32_t const *us = (utf32_t const *) in;
    std::size_t result = 0;
    for (; *us; ++us)
      result += utf8_len(*us);
    return result;
  }
  default: {
    char const *next;
    std::size_t result = 0;
    while (wchar_t ch = to_utf32_char(in, next, enc)) {
      result += utf8_len(ch);
      in = next;
    }
    return result;
  }
  }
}

std::size_t gott::utf32_len(char const *s, encoding enc) {
  switch (enc) {
  case ascii:
  case iso8859_1: case iso8859_2: case iso8859_3: case iso8859_4:
  case iso8859_5: case iso8859_6: case iso8859_7: case iso8859_8:
  case iso8859_9: case iso8859_10: case iso8859_11: case iso8859_12:
  case iso8859_13: case iso8859_14: case iso8859_15: case iso8859_16:
    return strlen(s);
  case utf32: case utf32le:
    return wcslen((wchar_t const *) s);
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
