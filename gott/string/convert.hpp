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

#ifndef GOTT_UTIL_NSTRING_CONVERT_HPP
#define GOTT_UTIL_NSTRING_CONVERT_HPP

#include "types.hpp"
#include <gott/range.hpp>
#include <gott/visibility.hpp>

namespace gott {

/** \defgroup encoding Basic encoding handling functions
 * Those are various functions for conveniently handling encoding.
 * @{
 */

/**
 * Allocate and fill a buffer containing the UTF8 representation of the
 * encoded input string.
 * \param in The encoded input string.
 * \param enc The encoding of @p in.
 * \return The newly allocated UTF8 buffer.
 */
GOTT_EXPORT
range_t<utf8_t *> to_utf8_alloc(range_t<char const *> const &in, encoding enc);

/**
 * Allocate and fill a buffer containing the encoded representation of the
 * UTF8 input string.
 * \param in The UTF8 input string.
 * \param enc The encoding of the output string.
 * \return The newly allocated buffer.
 */
GOTT_EXPORT
range_t<char *> to_enc_alloc(range_t<utf8_t const *> const &in, encoding enc);

/**
 * Convert a single encoded character to UTF32.
 * \param mbc The encoded character.
 * \param[out] end Pointer to after the encoded character.
 * \param enc The encoding of the input character.
 * \return The decoded character.
 */
GOTT_EXPORT
utf32_t to_utf32_char(char const *mbc, char const *&end, encoding enc);

/**
 * Convert a single encoded character to UTF32.
 * \param mbc The encoded character.
 * \param enc The encoding of the input character.
 * \return The decoded character.
 */
inline utf32_t to_utf32_char(char const *mbc, encoding enc) {
  char const *ignore;
  return to_utf32_char(mbc, ignore, enc);
}

/**
 * Write a UTF32 character to a UTF8 buffer.
 * \param ch The input UTF8 character.
 * \param out The pointer to the memory to be written to. 
 *            Will point to after the character.
 */
GOTT_EXPORT
void write_utf32_to_utf8(utf32_t ch, utf8_t *&out);

/**
 * Write a UTF32 character to a buffer of arbitrary encoding.
 * \param ch The input UTF8 character.
 * \param out The pointer to the memory to be written to. 
 *            Will point to after the character.
 * \param enc The encoding of @p out.
 */
GOTT_EXPORT
void write_utf32_to_enc(utf32_t ch, char *&out, encoding enc);

/**
 * Calculate the size of an encoded string if written as UTF8.
 * \param str The input encoded string.
 * \param enc The encoding of @p str.
 * \return The length of the hypothetical UTF8 string.
 */
GOTT_EXPORT
std::size_t utf8_len(range_t<char const *> const &str, encoding enc);

/**
 * Calculate the size of an encoded string if written as UTF32.
 * \param str The input encoded string.
 * \param enc The encoding of @p str.
 * \return The length of the hypothetical UTF32 string.
 */
GOTT_EXPORT
std::size_t utf32_len(range_t<char const *> const &str, encoding enc);

/**
 * Calculate the size of a single UTF32 character if written as UTF8.
 * \param ch The UTF32 character.
 * \return The size of the UTF8 character.
 */
GOTT_EXPORT
std::size_t utf8_len(utf32_t ch);

/**
 * Calculate the size of a single UTF32 character if written in an
 * arbitrary encoding.
 * \param ch The UTF32 character.
 * \param enc The desired encoding.
 * \return The size of the encoded character.
 */
GOTT_EXPORT
std::size_t enc_len(utf32_t ch, encoding enc);

//@}

}

#endif
