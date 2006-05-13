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

#ifndef GOTT_UTIL_NSTRING_CONVERT_HPP
#define GOTT_UTIL_NSTRING_CONVERT_HPP

#include "types.hpp"
#include <gott/range.hpp>
#include <gott/visibility.hpp>

namespace gott {

/** 
 * \name Basic encoding handling functions
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
