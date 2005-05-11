// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Simple SAX-like TDL parser
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

#include <gott/util/misc/commonheaders.hpp>

namespace gott {
namespace util {
namespace tdl {
namespace simple {

// Interface parser
// This is a callback interface to the simple tdl parser.
struct EXPORT parser {
  // callbacks
  virtual void begin_parse() = 0;
    // called as soon as the parse has begun

  virtual void down() = 0;
    // called when we open a new sub-layer
 
  virtual void node(std::wstring const &content) = 0;
    // called when a node has been read

  virtual void comment(std::wstring const &content, bool on_new_line) = 0;
    // called when a comment has been read

  virtual void up() = 0;
    // called as we close a sub-layer

  virtual void end_parse() = 0;
    // final call

  virtual ~parser() = 0;
};

struct line_logger {
  virtual void start_line() = 0;
  virtual void token(unsigned start, unsigned end, std::wstring const &nd) = 0;
  virtual void line_position(unsigned line_pos) = 0;
  EXPORT virtual ~line_logger() = 0;
};

// Class meta_parser
// This is an interface where you can specify meta-data call-backs and that you
// can pass the meta-data parser.
// Meta-data is seen in the start of the file as '#?...' lines. The first 
// non-empty line ends the meta-data section.
// If a command handler fails, the default command handler is called. 
// The default handler should always succeed (currently failure is ignored).
class meta_parser {
public:
  typedef 
    boost::function<bool (std::wstring const &, std::wstring const &)> 
    callback;
    // The function type for a command handler
    // First parameter: command name
    // Second parameter: rest of the command
    // Return: success

  EXPORT meta_parser();

  class store_one_param {
  public:
    bool operator() (std::wstring const &, std::wstring const &p) { 
      x = p; 
      return true;
    }
    std::wstring const &data() const { return x; }

  private:
    std::wstring x;
  };
  
  void set_default(callback const &f) EXPORT;
    // set the default command handler to f
  
  void set_specific(std::wstring const &s, callback const &f) EXPORT;
    // set the command handler for the command s to f

public:
  void exec(std::wstring const &line);

private:
  callback def;
  typedef hashd::hash_map<std::wstring, callback> cb_t;
  cb_t cb;
};

EXPORT void parse(std::wistream &s, parser &p, line_logger * = 0);
  // Parses the data section of a stream s (follows the meta-data section) 
  // and puts the results in p.

EXPORT void parse_meta(std::wistream &, meta_parser &, line_logger * = 0);
  // Parses the meta-data section of a stream to a meta_parser object.

}}}}
#endif
