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
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#ifndef GOTT_UTIL_NSTRING_TYPES_HPP
#define GOTT_UTIL_NSTRING_TYPES_HPP

#include <boost/cstdint.hpp>
#include <cstddef>

namespace gott {

/**
 * The storage type of UTF8 characters.
 */
typedef char utf8_t;
typedef boost::uint8_t uutf8_t;

/**
 * The storage type of UTF32 characters.
 */
typedef boost::int32_t utf32_t;
typedef boost::uint32_t uutf32_t;

/**
 * ID for encodings.
 */
enum encoding {
  // Full Unicode
  utf8,
  utf16be, utf16le,
  utf32be, utf32le,
  utf7,
  c99,java,
  // Proprietary (TODO)
  utf16 = utf16le,
  utf32 = utf32le,
  wide = utf32,
  // European
  ascii,
  iso8859_1, iso8859_2, iso8859_3, iso8859_4, iso8859_5, iso8859_6, iso8859_7,
  iso8859_8, iso8859_9, iso8859_10, iso8859_11, iso8859_12, iso8859_13, 
  iso8859_14, iso8859_15, iso8859_16,
  NO_ENCODING
};

}

#endif
