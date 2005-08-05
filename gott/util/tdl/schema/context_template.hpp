// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
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

#ifndef GOTT_UTIL_TDL_SCHEMA_CONTEXT_TEMPLATE_HPP
#define GOTT_UTIL_TDL_SCHEMA_CONTEXT_TEMPLATE_HPP

#include <gott/util/commonheaders.hpp>

#include "context.hpp"
#include "rule_attr.hpp"
#include "slot.hpp"

namespace gott {
namespace tdl {
namespace schema {

/**
 * Chainable context producer. Stores the data necessary to construct a context
 * and can take contexts as "arguments", hence context_<b>template</b>.
 */
class context_template {
public:
  context_template() GOTT_EXPORT;
  ~context_template() GOTT_EXPORT;

  /**
   * Begins the declaration of a "rule".
   * @see context::begin
   * \param id The type's name.
   * \param ra The rule-factory's and later rule's attributes.
   */
  void begin(std::wstring const &id, 
             rule_attr const &ra = rule_attr()) GOTT_EXPORT;

  /**
   * Begins the declaration of a "rule".
   * @see context::begin
   * \param n The type's name.
   * \param ra The rule-factory's and later rule's attributes.
   * \param s Repetition definition.
   */
  void begin(std::wstring const &n, rule_attr const &ra, 
             slotcfg const &s) GOTT_EXPORT;

  /**
   * End the declaration of a "rule".
   */
  void end() GOTT_EXPORT;

  typedef unsigned eID; ///< A parameter's ID.

  /**
   * Add a parameter to the template.
   * \param id The index of the parameter to add.
   */
  void param(eID id) GOTT_EXPORT;

  /**
   * Instantiate to a context object.
   * \param params The parameters in a vector, position equals eID.
   * \param cont The context to fill.
   */
  void instantiate(std::vector<context*> const &params, context &cont) GOTT_EXPORT;

private:
  class IMPL;
  boost::scoped_ptr<IMPL> p;
};

}}}

#endif
