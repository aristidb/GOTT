// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
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

#ifndef GOTT_UTIL_TDL_SCHEMA_PARSE_HPP
#define GOTT_UTIL_TDL_SCHEMA_PARSE_HPP

#include <gott/util/misc/commonheaders.hpp>
#include <gott/util/tdl/simple/parse/parser.hpp>
#include <gott/util/tdl/structure/structure.hpp>

#include "rule.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace schema {

class positioning;

// Class match
// Tries to fit a given tdl document (by simple::parse) into a schema
class match : public simple::parser {
  class IMPL;

public:
  match(structure::revocable_structure &tree) EXPORT;

  match(rule::factory const &rf, structure::revocable_structure &tree) EXPORT;
    // initialize a match-object that writes to tree and tries to
    // match rf

  ~match() EXPORT;

  void add(rule::factory const &) EXPORT;
    // parse the new rule (instanciated from the given factory) before
    // continueing with the current rule

  structure::revocable_structure &structure() EXPORT;
    // return a reference to tree (defined in constructor)

public:
  positioning &pos() EXPORT;

public:
  simple::line_logger *get_debug() EXPORT;

private:
  void begin_parse();
  void down();
  void node(std::wstring const &);
  void up();
  void end_parse();
  void comment(std::wstring const &, bool);

private:
  class IMPL;
  IMPL *pIMPL;
};

}}}}
#endif
