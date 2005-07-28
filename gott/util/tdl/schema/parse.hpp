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

/**
 * TDL schema matching engine.
 * Use this as the first parameter to simple::parse in order to match a schema.
 * If you use the result of match::get_debug as the second parameter, better
 * errors can be produced.
 *
 * Example usage:
 * @code
 * try {
 *   schema::context cont;
 *   // ...fill cont...
 *   structure::tree tr;
 *   schema::match m(cont.get());
 *   simple::parse(m, m.get_debug());
 * } catch (schema::mismatch &m) {
 *   std::wcout << m.what() << std::endl;
 * }
 * @endcode
 */
class match : public simple::parser {
  class IMPL;

public:
  /**
   * Constructs a new match object writing to a tree/something.
   * \param tree The tree to write to.
   */
  match(structure::revocable_structure &tree) EXPORT;

  /**
   * Constructs a new match object writing to a tree/something matching a 
   * document specified by a rule-factory.
   * \param rf The rule-factory specifying the document type.
   * \param tree The tree to write to.
   */
  match(rule::factory const &rf, structure::revocable_structure &tree) EXPORT;

  ~match() EXPORT;

  /**
   * Tells the engine to match next a rule specified by a rule-factory.
   * \param rf The rule-factory specifying the to-be-added rule.
   */
  void add(rule::factory const &rf) EXPORT;

  /**
   * Return a reference to the tree the engine writes to.
   */
  structure::revocable_structure &revocable_structure() const EXPORT;

  /**
   * Return a reference to the current wrapped structure the engine directly
   * writes to.
   */
  structure::writable_structure &direct_structure() const EXPORT;

public:
  /**
   * Return a reference to the back-tracking and book-keeping sub-engine.
   */
  positioning &pos() const EXPORT;

public:
  /**
   * Return a pointer to the internal line-logger.
   * If this is passed to simple::parse, we can better report errors.
   */
  simple::line_logger *get_debug() const EXPORT;

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
