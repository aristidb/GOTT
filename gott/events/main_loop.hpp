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

#ifndef GOTT_BASE_EVENTS_MAIN_LOOP_HPP
#define GOTT_BASE_EVENTS_MAIN_LOOP_HPP

#include <gott/visibility.hpp>
#include <gott/string/qid.hpp>
#include <gott/exceptions.hpp>
#include <gott/plugin/plugin_base.hpp>
#include <sigc++/signal.h>
#include <boost/scoped_ptr.hpp>

namespace gott {
namespace events {

/**
 * Basic main loop interface class. For a single thread only.
 */
class GOTT_EXPORT main_loop : public gott::plugin::plugin_base {
public:
  /// Constructor.
  main_loop();
  /// Pure virtual destructor.
  virtual ~main_loop() = 0;

  /**
   * Run the main loop until there is nothing to do. Nothing to do means, there
   * are no objects to wait for (explicitly).
   */
  virtual void run() = 0;
  /// Quit the main loop from inside.
  virtual void quit_local() = 0;

  /**
   * The idle signal.
   * The idle signal is emitted (at least) whenever the main_loop is about to
   * block. Also, this signal is guaranteed to be emitted after <b>all</b>
   * timers have been handled.
   */
  virtual sigc::signal0<void> &on_idle() = 0;

  /**
   * Add an object to the list of objects the main_loop should wait for.
   */
  virtual void add_waitable() = 0;

  /**
   * Remove an object from the list of objects the main_loop should wait for.
   */
  virtual void remove_waitable() = 0;

  /**
   * Get a concrete feature of the main loop.
   * \return
   *   - 0 if the feature is not available
   *   - a pointer to the feature
   */
  template<class T>
  GOTT_LOCAL T *feature_ptr() {
    T *outside = T::get_for(*this);
    if (outside)
      return outside;
    return i_feature_ptr<T>();
  }

  /**
   * Get a concrete integrated feature of the main loop.
   * \return
   *   - 0 if the feature is not available
   *   - a pointer to the feature
   */
  template<class T>
  GOTT_LOCAL T *i_feature_ptr() {
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

  /**
   * Get a concrete feature of the main loop.
   * \return A pointer to the feature.
   * \throw user_error If the feature is not available.
   */
  template<class T>
  GOTT_LOCAL T &feature() {
    T *p = feature_ptr<T>();
    if (!p)
      throw user_error("unsupported feature");
    return *p;
  }

  /**
   * Get a concrete feature of the main loop.
   * \return A pointer to the feature.
   * \throw user_error If the feature is not available.
   */
  template<class T>
  GOTT_LOCAL T const &feature() const {
    T const *p = feature_ptr<T>();
    if (!p)
      throw user_error("unsupported feature");
    return *p;
  }

public:
  sigc::signal0<void> &on_destroy();
  void *&feature_data(QID const &id);

protected:
  virtual void *do_feature(QID const &) = 0;

private:
  class impl;
  boost::scoped_ptr<impl> p;
};

//TODO faster please
#define GOTT_EVENTS_FEATURE(var,T) \
  if (var == T::qid) \
    return static_cast<T *>(this);

}}

#endif
