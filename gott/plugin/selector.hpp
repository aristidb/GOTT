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

#ifndef GOTT_PLUGIN_SELECTOR_HPP
#define GOTT_PLUGIN_SELECTOR_HPP

#include <gott/string/qid.hpp>
#include <boost/shared_ptr.hpp>
#include <utility>

namespace gott { namespace plugin {

class plugin_descriptor;
class module_descriptor;

class selector {
public:
  plugin_descriptor get_plugin() const;
  module_descriptor get_module() const;

  GOTT_EXPORT ~selector();

  GOTT_EXPORT static selector with_plugin_id(QID const &plugin_id);
  GOTT_EXPORT static selector with_interface_id(QID const &interface_id);
  GOTT_EXPORT static selector with_module_id(QID const &module_id);
  GOTT_EXPORT static selector with_feature_id(QID const &feature_id);

  GOTT_EXPORT selector operator&&(selector const &other) const;

private:
  class impl;
  boost::shared_ptr<impl const> p;
  explicit selector(impl *);
};

inline selector with_plugin_id(QID const &plugin_id) {
  return selector::with_plugin_id(plugin_id);
}

inline selector with_interface_id(QID const &interface_id) {
  return selector::with_interface_id(interface_id);
}

template<class T>
inline selector with_interface() {
  return with_interface_id(T::qid);
}

inline selector with_module_id(QID const &module_id) {
  return selector::with_module_id(module_id);
}

inline selector with_feature_id(QID const &feature_id) {
  return selector::with_feature_id(feature_id);
}

template<class T>
inline selector with_feature() {
  return with_feature_id(T::qid);
}

}}

#endif
