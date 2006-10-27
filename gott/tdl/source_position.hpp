// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#ifndef GOTT_TDL_SOURCE_POSITION_HPP
#define GOTT_TDL_SOURCE_POSITION_HPP

#include <gott/string/string.hpp>
#include <ostream>

namespace tdl {

class source_position {
public:
  source_position(
      gott::string const &file = gott::string(),
      unsigned line = 0,
      unsigned column = 0,
      gott::string const &token = gott::string(),
      unsigned token_line = 0,
      unsigned token_column = 0
      )
    : file(file), line(line), column(column), token(token), 
    token_line(token_line), token_column(token_column) {}

  gott::string file;
  unsigned line;
  unsigned column;
  gott::string token;
  unsigned token_line;
  unsigned token_column;

  source_position token_position() const {
    return source_position(file, token_line, token_column, token, token_line,
        token_column);
  }
};

inline std::ostream &operator<<(std::ostream &out, source_position const &obj) {
  return out << obj.file << " : " << obj.line << ':' << obj.column << '/' << 
    obj.token_line << ':' << obj.token_column << " (" << obj.token << ')';
}

inline bool operator==(source_position const &a, source_position const &b) {
  return a.line == b.line && a.column == b.column 
    && a.token_line == b.token_line && a.file == b.file && a.token == b.token;
}

inline bool operator!=(source_position const &a, source_position const &b) {
  return !(a == b);
}

}

#endif
