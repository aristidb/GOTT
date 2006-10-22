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

#ifndef GOTT_UTIL_TDL_SimplE_META_HPP
#define GOTT_UTIL_TDL_SimplE_META_HPP

#include "../source_position.hpp"
#include <boost/function.hpp>
#include <gott/visibility.hpp>

namespace gott {
class string;
}

namespace tdl {
class source_position;

/**
 * TDL Meta-data callback.
 * This is an interface where you can specify meta-data call-backs and that you
 * can pass to parse_meta.
 * Meta-data is the section of the file containing '#?...' (and empty) lines
 * only. A non-empty line ends this section.
 * If a specified handler fails, the default handler is called.
 * The default handler should always succeed (currently failure is ignored).
 */
class meta_parser {
public:
  /**
   * The function type for a command handler.
   * \param cmd The command name.
   * \param param The rest of the command.
   * \return The success of the handler.
   */
  typedef 
    boost::function<bool (gott::string const &cmd, gott::string const &param)> 
    callback;

  GOTT_EXPORT meta_parser();
  GOTT_EXPORT ~meta_parser();

  /**
   * Set the default command handler.
   * \param f The handler to add.
   */
  void set_default(callback const &f) GOTT_EXPORT;
  
  /**
   * Set a specific command handler.
   * \param s The command to invocate this handler on.
   * \param f The handler to add.
   */
  void set_specific(gott::string const &s, callback const &f) GOTT_EXPORT;

  /**
   * Parses the meta-data section of a TDL document to a special parser.
   * \param s The stream to read from.
   * \param w The current source position in the stream.
   */
  source_position parse(std::istream &s, 
      source_position const &w = source_position()) GOTT_EXPORT;

  /**
   * Returns the current position in the stream.
   */
  source_position const &where() const GOTT_EXPORT;

private:
  class impl;
  impl *p;
};

}

#endif
