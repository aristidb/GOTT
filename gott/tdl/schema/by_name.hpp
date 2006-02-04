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

#ifndef GOTT_UTIL_TDL_SCHEMA_BY_NAME_HPP
#define GOTT_UTIL_TDL_SCHEMA_BY_NAME_HPP

#include <boost/scoped_ptr.hpp>
#include <gott/visibility.hpp>
#include <gott/tdl/schema/rule.hpp>

namespace gott {
class string;

namespace tdl {
namespace schema {

/**
 * Manages all named (native) schema types.
 */
class by_name_t {
  by_name_t();
  ~by_name_t();
  friend by_name_t &by_name();

public:
  /**
   * Add a type to the database.
   * \param name The name to find the type with.
   * \param type The type.
   */
  GOTT_EXPORT void add(string const &name, abstract_rule const &type);

  template<class T>
  void add(string const &name) {
    item_constructor type_con = &construct_item<T>;
    item_check accept_empty = &T::accept_empty;
    add(name, abstract_rule(type_con, accept_empty));
  }

  /**
   * Get a type from the database.
   * \param name The type's name.
   * \param att The attributes to supply to the type.
   * \param children The children to supply to the type.
   * \return A rule_t for the wanted type.
   */
  GOTT_EXPORT 
  rule_t get(string const &name, rule_attr_t const &att, 
             std::vector<rule_t> const &children) const;

  class impl;
  boost::scoped_ptr<impl> p;
};

GOTT_EXPORT by_name_t &by_name();

}}}

#endif
