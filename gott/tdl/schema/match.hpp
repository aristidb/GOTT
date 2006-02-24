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

#ifndef GOTT_UTIL_TDL_SCHEMA_PARSE_HPP
#define GOTT_UTIL_TDL_SCHEMA_PARSE_HPP

#include <gott/tdl/parse/parser.hpp>
#include <gott/tdl/structure/structure.hpp>
#include <gott/tdl/schema/ev_fwd.hpp>
#include <boost/scoped_ptr.hpp>

namespace tdl {
namespace structure { class repatcher; }
  
namespace schema {

class positioning;
class rule_t;

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
 *
 * TODO: Split?!
 */
class match : public abstract_tdl_parser {
  class impl;

public:
  /**
   * Constructs a new match object writing to a tree/something.
   * \param tree The tree to write to.
   */
  match(structure::revocable_structure &tree) GOTT_EXPORT;

  /**
   * Constructs a new match object writing to a tree/something matching a 
   * document specified by a rule_t-factory.
   * \param rf The rule_t-factory specifying the document type.
   * \param tree The tree to write to.
   */
  GOTT_EXPORT
  match(rule_t const &rf, structure::revocable_structure &tree);

  ~match() GOTT_EXPORT;

  /**
   * Tells the engine to match next a item specified by a rule_t-factory.
   * \param rf The rule_t-factory specifying the to-be-added rule_t.
   */
  void add(rule_t const &rf) GOTT_EXPORT;

  /**
   * Return a reference to the tree the engine writes to.
   */
  structure::revocable_structure &revocable_structure() const GOTT_EXPORT;

  /**
   * Return a reference to the current wrapped structure the engine directly
   * writes to.
   */
  structure::writable_structure &direct_structure() const GOTT_EXPORT;

  /**
   * Return the schema parsers current source_position. If not backtracking
   * equivalent to simple::parser::where(), else equivalent to the result
   * of simple::parser::where() when the current memorized data was actually
   * read.
   */
  source_position const &where_out() const GOTT_EXPORT;

public:
  /**
   * Return a reference to the back-tracking and book-keeping sub-engine.
   */
  positioning &pos() const GOTT_EXPORT;

  /**
   * Tell the current rule_ts that tokens are still to be read.
   */
  void parental_requirement(ev::event const &, unsigned) GOTT_EXPORT;

private:
  void begin_parse();
  void down();
  void node(gott::string const &);
  void up();
  void end_parse();
  void comment(gott::string const &, bool);

private:
  class impl;
  boost::scoped_ptr<impl> p;

  match();
};

}}

#endif
