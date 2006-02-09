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
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
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

#ifndef GOTT_UTIL_TDL_SimplE_PARSER_H
#define GOTT_UTIL_TDL_SimplE_PARSER_H

#include <gott/tdl/source_position.hpp>
#include <boost/scoped_ptr.hpp>
#include <iosfwd>

namespace gott {
namespace tdl {
namespace simple {

/**
 * The callback interface to parse.
 */
class GOTT_EXPORT parser {
public:
  /**
   * Parse a TDL document. Does not treat the meta-data section specifically.
   * \param s The stream to read a document from.
   */
  void parse(std::istream &s);

  /**
   * Returns the current position in the currently parsed TDL document.
   * Undefined result if not in parse().
   */
  source_position const &where() const;

public:
  // callbacks
  virtual void begin_parse() = 0;
    ///< Called as soon as the parse has begun.

  virtual void down() = 0;
    ///< Called when we open a new sub-layer.
 
  virtual void node(string const &content) = 0;
    ///< Called when a node has been read.

  virtual void comment(string const &content, bool on_new_line) = 0;
    ///< Called when a comment has been read.

  virtual void up() = 0;
    ///< Called as we close a sub-layer.

  virtual void end_parse() = 0;
    ///< Final call.

  virtual ~parser() = 0;

private:
  source_position where_;
};

}}}

#endif
