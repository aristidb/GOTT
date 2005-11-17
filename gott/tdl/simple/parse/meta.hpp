// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Simple SAX-like TDL parser
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

#ifndef GOTT_UTIL_TDL_SIMPLE_META_HPP
#define GOTT_UTIL_TDL_SIMPLE_META_HPP

#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <gott/visibility.hpp>

namespace gott {
class string;

namespace tdl {
namespace simple {

class line_logger;

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
    boost::function<bool (string const &cmd, string const &param)> 
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
  void set_specific(string const &s, callback const &f) GOTT_EXPORT;

  /**
   * Parses the meta-data section of a TDL document to a special parser.
   * \param s The stream to read from.
   * \param l (optional) The line-logger callback.
   */
  void parse(std::wistream &s, line_logger *l = 0) GOTT_EXPORT;

private:
  class IMPL;
  boost::scoped_ptr<IMPL> p;
};

}}}

#endif
