// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT main loop
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

#ifndef GOTT_BASE_EVENTS_MAIN_LOOP_HPP
#define GOTT_BASE_EVENTS_MAIN_LOOP_HPP

#include <stdexcept>
#include <gott/visibility.hpp>
#include <gott/string/qid.hpp>

namespace gott {
namespace events {

/**
 * Basic main loop interface class. For a single thread only.
 */
class GOTT_EXPORT main_loop {
public:
  /// Constructor.
  main_loop();
  /// Pure virtual destructor.
  virtual ~main_loop() = 0;
  /// Run the main loop until there is nothing to do.
  virtual void run() = 0;
  /// Quit the main loop.
  virtual void quit() = 0;

  /**
   * Get a concrete feature of the main loop.
   * \return
   *   - 0 if the feature is not available
   *   - a pointer to the feature
   */
  template<class T>
  GOTT_LOCAL T *feature_ptr() { 
    return static_cast<T *>(do_feature(T::qid));
  }

  /**
   * Get a concrete feature of the main loop.
   * \return
   *   - 0 if the feature is not available.
   *   - A pointer to the feature.
   */
  template<class T>
  GOTT_LOCAL T const *feature_ptr() const { 
    return const_cast<T const *>(
        const_cast<main_loop *>(this)->feature_ptr<T>());
  }

  /// Exception thrown if a requested feature is not available.
  class GOTT_EXPORT bad_feature : std::runtime_error {
    GOTT_LOCAL
    bad_feature() 
      : std::runtime_error("Requested main_loop feature not available") {}
    friend class main_loop;
  };

  /**
   * Get a concrete feature of the main loop.
   * \return A pointer to the feature.
   * \throw #bad_feature if the feature is not available.
   */
  template<class T>
  GOTT_LOCAL T &feature() {
    T *p = feature_ptr<T>();
    if (!p)
      throw bad_feature();
    return *p;
  }

  /**
   * Get a concrete feature of the main loop.
   * \return A pointer to the feature.
   * \throw #bad_feature if the feature is not available.
   */
  template<class T>
  GOTT_LOCAL T const &feature() const {
    T const *p = feature_ptr<T>();
    if (!p)
      throw bad_feature();
    return *p;
  }

protected:
  virtual void *do_feature(gott::QID const &) = 0;
};

//TODO faster please
#define GOTT_EVENTS_FEATURE(var,T) \
  if (var == T::qid) \
    return static_cast<T *>(this);

}}

#endif
