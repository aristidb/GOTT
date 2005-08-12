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

#include <gott/util/visibility.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

namespace gott {
class nstring;

namespace tdl {
namespace simple {

/**
 * The callback interface to parse.
 */
struct GOTT_EXPORT parser {
  // callbacks
  virtual void begin_parse() = 0;
    ///< Called as soon as the parse has begun.

  virtual void down() = 0;
    ///< Called when we open a new sub-layer.
 
  virtual void node(nstring const &content) = 0;
    ///< Called when a node has been read.

  virtual void comment(nstring const &content, bool on_new_line) = 0;
    ///< Called when a comment has been read.

  virtual void up() = 0;
    ///< Called as we close a sub-layer.

  virtual void end_parse() = 0;
    ///< Final call.

  virtual ~parser() = 0;
};

/**
 * The callback interface notified by parse() of line and cursor changes.
 */
struct line_logger {
  virtual void start_line() = 0;
    ///< A new line was started.

  /**
   * A new token has appeared.
   * \param start The starting line position of the token.
   * \param end The first line position after the token.
   * \param nd The token itself.
   */
  virtual void token(unsigned start, unsigned end, nstring const &nd) = 0;

  /**
   * Called whenever a different line position is reached.
   * \param line_pos The line position.
   */
  virtual void line_position(unsigned line_pos) = 0;

  GOTT_EXPORT virtual ~line_logger() = 0;
};

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
    boost::function<bool (nstring const &cmd, nstring const &param)> 
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
  void set_specific(nstring const &s, callback const &f) GOTT_EXPORT;

public:
  /// @internal
  void exec(nstring const &line);

private:
  class IMPL;
  boost::scoped_ptr<IMPL> p;
};

/**
 * Parse a TDL document. Does not treat the meta-data section specifically.
 * \param s The stream to read a document from.
 * \param p The parser callback to notify of tokens.
 * \param l (optional) The line-logger callback.
 */
GOTT_EXPORT 
void parse(std::wistream &s, parser &p, line_logger *l = 0);

/**
 * Parses the meta-data section of a TDL document to a special parser.
 * \param s The stream to read from.
 * \param p The meta-data callback to notify.
 * \param l (optional) The line-logger callback.
 */
GOTT_EXPORT 
void parse_meta(std::wistream &s, meta_parser &p, line_logger *l = 0);

}}}
#endif
