// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#ifndef GOTT_TDL_RESOURCE_HPP
#define GOTT_TDL_RESOURCE_HPP

#include <gott/string/atom.hpp>
#include <gott/string/qid.hpp>
#include <boost/function.hpp>
#include <gott/plugin/plugin_base.hpp>

namespace tdl {

class resource;

namespace detail {
  typedef boost::function<void (resource *)> generic_callback;

  template<class T, class U>
  struct callback_helper {
    U impl;

    callback_helper(U callback) : impl(callback) {}

    void operator() (resource *ptr) {
      impl(static_cast<T*>(ptr));
    }
  };
}

class resource : public gott::plugin::plugin_base {
public:
  virtual gott::atom get_kind() const = 0;
  virtual gott::atom get_id() const = 0;
  virtual ~resource();

  static gott::QID const qid;

public:
  template<class T, class U>
  static void list(U callback) {
    list_impl(T::kind, detail::callback_helper<T, U>(callback));
  }

  template<class T, class U>
  static void find(gott::atom const &id, U callback) {
    find_impl(id, T::kind, detail::callback_helper<T, U>(callback));
  }

private:
  GOTT_EXPORT static void list_impl(
      gott::atom const &kind,
      detail::generic_callback const &callback);

  GOTT_EXPORT static void find_impl(
      gott::atom const &id,
      gott::atom const &kind,
      detail::generic_callback const &callback);
};

};

#endif