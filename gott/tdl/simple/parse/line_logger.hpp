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

#ifndef GOTT_UTIL_TDL_SIMPLE_LINE_LOGGER_HPP
#define GOTT_UTIL_TDL_SIMPLE_LINE_LOGGER_HPP

#include <gott/visibility.hpp>

namespace gott {
class string;

namespace tdl {
namespace simple {

/**
 * The callback interface notified by parse() of line and cursor changes.
 */
class line_logger {
public:
  virtual void start_line() = 0;
    ///< A new line was started.

  /**
   * A new token has appeared.
   * \param start The starting line position of the token.
   * \param end The first line position after the token.
   * \param nd The token itself.
   */
  virtual void token(unsigned start, unsigned end, string const &nd) = 0;

  /**
   * Called whenever a different line position is reached.
   * \param line_pos The line position.
   */
  virtual void line_position(unsigned line_pos) = 0;

  GOTT_EXPORT virtual ~line_logger() = 0;
};

}}}

#endif
