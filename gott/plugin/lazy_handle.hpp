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
 * The Original Code is A Plugin / Loadable Module Engine.
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

#ifndef GOTT_PLUGIN_LAZY_HANDLE_HPP
#define GOTT_PLUGIN_LAZY_HANDLE_HPP

#include "plugin_handle.hpp"
#include "descriptor.hpp"
#include "selector.hpp"
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>

namespace gott { namespace plugin {

template<class T>
class lazy_handle {
public:
  lazy_handle(plugin_descriptor const &x) : data(x) {}
  lazy_handle(selector const &x) : data(x) {}

  T *get() { return get_impl(); }
  T const *get() const { return get_impl(); }
  T *operator*() { return get_impl(); }
  T const *operator*() const { return get_impl(); }
  T *operator->() const { return get_impl(); }

private:
  T *get_impl() const {
    switch (data.which()) {
    case 1:
      data = plugin_handle<T>(boost::get<plugin_descriptor>(data));
      break;
    case 2:
      data = plugin_handle<T>(boost::get<selector>(data));
      break;
    }
    return boost::get<plugin_handle<T> >(data).get();
  }
  mutable boost::variant<plugin_handle<T>, plugin_descriptor, selector> data;
};

}}

#endif
