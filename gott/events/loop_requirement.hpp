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
 * The Original Code is An Event Handling Class Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#ifndef GOTT_BASE_EVENTS_LOOP_REQUIREMENT_HPP
#define GOTT_BASE_EVENTS_LOOP_REQUIREMENT_HPP

#include <gott/visibility.hpp>
#include <gott/string/qid.hpp>
#include <boost/shared_ptr.hpp>

namespace gott {
namespace events {
class main_loop_factory;

/**
 * A formulated requirement for a main_loop.
 */
class loop_requirement {
public:
  enum combiner_t { combine_and, combine_or };

  GOTT_EXPORT
  loop_requirement(
      loop_requirement const &, 
      loop_requirement const &,
      combiner_t);

  enum feature_flag_t { feature };
  GOTT_EXPORT
  loop_requirement(QID const &, feature_flag_t);

public: // internal
  bool do_try(main_loop_factory &) const;

private:
  class impl;
  boost::shared_ptr<impl const> p;

  loop_requirement();
};

/**
 * Requirement for a main_loop to fulfill two other requirements.
 * \param lhs First checked requirement.
 * \param rhs Second checked requirement.
 * \return A requirement object.
 * \relates loop_requirement
 */
inline loop_requirement operator&&(
    loop_requirement const &lhs, 
    loop_requirement const &rhs) {
  return loop_requirement(lhs, rhs, loop_requirement::combine_and);
}

/**
 * Requirement for a main_loop to fulfill at least one of two other 
 * requirements.
 * \param lhs First checked requirement.
 * \param rhs Second checked requirement.
 * \return A requirement object.
 * \relates loop_requirement
 */
inline loop_requirement operator||(
    loop_requirement const &lhs,
    loop_requirement const &rhs) {
  return loop_requirement(lhs, rhs, loop_requirement::combine_or);
}

/**
 * Requirement for a main_loop to have a certain feature.
 * \arg \c T The required feature class.
 * \relates loop_requirement
 */
template<class T>
loop_requirement feature() {
  return loop_requirement(T::qid, loop_requirement::feature);
}

/**
 * Requirement for a main_loop to have a certain feature.
 * \param qid The QID of the required feature.
 * \relates loop_requirement
 */
inline loop_requirement feature(QID const &qid) {
  return loop_requirement(qid, loop_requirement::feature);
}

}}

#endif
