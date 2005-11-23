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

#ifndef GOTT_UTIL_TDL_SIMPLE_PARSER_H
#define GOTT_UTIL_TDL_SIMPLE_PARSER_H

#include <gott/visibility.hpp>
#include <boost/scoped_ptr.hpp>
#include <iosfwd>

namespace gott {
class string;

namespace tdl {
namespace simple {

class line_logger;

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

protected:
  GOTT_LOCAL parser(line_logger *l) : ll(l) {}
  GOTT_LOCAL void set_line_logger(line_logger *l) { ll = l; }
  
private:
  line_logger *ll;
};

}}}
#endif
