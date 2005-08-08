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
#include <gott/util/range_algo.hpp>

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
    utf32_t const *us = uss.begin;
    std::size_t result = 0;
    for (; us < uss.end; ++us)
      result += utf8_len(*us);
    return result;
  }
  default: {
    char const *current = in.begin, *next;
    std::size_t result = 0;
    while (current < in.end) {
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
    copy(in, result);
    return range(result, result + len);
  }

  utf8_t *out = result;
  char const *current = in.begin, *next;
  while (current < in.end) {
    write_utf32_to_utf8(to_utf32_char(current, next, enc), out);
    current = next;
  }
  return range(result, out);
}

gott::range_t<char *>
gott::to_enc_alloc(range_t<utf8_t const *> const &in_, encoding enc) {
  range_t<utf8_iterator> in = in_.cast<utf8_iterator>();

  std::size_t len = 0;
  for (utf8_iterator it = in.begin; it != in.end; ++it)
    len += enc_len(*it, enc);

  char *result = new char[len];
  char *out = result;
  for (utf8_iterator it = in.begin; it != in.end; ++it)
    write_utf32_to_enc(*it, out, enc);
  return range(result, out);
}
