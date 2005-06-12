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

#ifndef GOTT_UTIL_TDL_SCHEMA_CONTEXT_TEMPLATE_HPP
#define GOTT_UTIL_TDL_SCHEMA_CONTEXT_TEMPLATE_HPP

#include <gott/util/misc/commonheaders.hpp>

#include "context.hpp"
#include "rule_attr.hpp"
#include "slot.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace schema {

/**
 * Chainable context producer. Stores the data necessary to construct a context
 * and can take contexts as "arguments", hence context_<b>template</b>.
 */
class context_template {
public:
  context_template() EXPORT;
  ~context_template() EXPORT;

  /**
   * Begins the declaration of a "rule".
   * @see context::begin
   * \param id The rule-factory builder's index.
   * \param ra The rule-factory's and later rule's attributes.
   */
  void begin(unsigned id, 
             rule::attributes const &ra = rule::attributes()) EXPORT;

  /**
   * Begins the declaration of a "rule".
   * @see context::begin
   * \param id The rule-factory builder's index.
   * \param ra The rule-factory's and later rule's attributes.
   * \param s Repetition definition.
   */
  void begin(unsigned id, rule::attributes const &ra, slotcfg const &s) EXPORT;

  /**
   * End the declaration of a "rule".
   */
  void end() EXPORT;

  typedef unsigned eID; ///< A parameter's ID.

  /**
   * Add a parameter to the template.
   * \param id The index of the parameter to add.
   */
  void param(eID id) EXPORT;

  /**
   * Instantiate to a context object.
   * \param params The parameters in a vector, position equals eID.
   * \param cont The context to fill.
   */
  void instantiate(std::vector<context*> const &params, context &cont) EXPORT;

private:
  struct entry_begin {
    entry_begin(unsigned i, rule::attributes const &a) : id(i), att(a) {}
    entry_begin(unsigned i, rule::attributes const &a, slotcfg const &s)
    : id(i), att(a), scfg(s) {}

    unsigned id;
    rule::attributes att;
    boost::optional<slotcfg> scfg;
  };

  struct entry_end {};
  typedef boost::variant<eID, entry_begin, entry_end> job;
  struct visitor;
  typedef std::vector<job> book;

  book var;
};

}}}}

#endif
