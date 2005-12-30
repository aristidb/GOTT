// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT main loop spawner
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

#ifndef GOTT_BASE_EVENTS_LOOP_REQUIREMENT_HPP
#define GOTT_BASE_EVENTS_LOOP_REQUIREMENT_HPP

#include <gott/visibility.hpp>
#include <gott/string/qid.hpp>
#include <boost/shared_ptr.hpp>

namespace gott {
namespace events {
class auto_loop;

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
  bool do_try(auto_loop &) const;

private:
  bool do_try(auto_loop &, loop_requirement const *) const;
  
  class IMPL;
  boost::shared_ptr<IMPL> p;

  loop_requirement();
};

/**
 * Requirement for a main_loop to fulfill two other requirements.
 */
inline loop_requirement operator&&(
    loop_requirement const &lhs, 
    loop_requirement const &rhs) {
  return loop_requirement(lhs, rhs, loop_requirement::combine_and);
}

/**
 * Requirement for a main_loop to fulfill at least one of two other 
 * requirements.
 */
inline loop_requirement operator||(
    loop_requirement const &lhs,
    loop_requirement const &rhs) {
  return loop_requirement(lhs, rhs, loop_requirement::combine_or);
}

/**
 * Requirement for a main_loop to have a certain feature.
 * \param T The required feature class.
 */
template<class T>
loop_requirement feature() {
  return loop_requirement(T::qid, loop_requirement::feature);
}

/**
 * Requirement for a main_loop to have a certain feature.
 * \param qid The QID of the required feature.
 */
inline loop_requirement feature(QID const &qid) {
  return loop_requirement(qid, loop_requirement::feature);
}

}}

#endif
