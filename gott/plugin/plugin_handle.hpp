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

#ifndef GOTT_BASE_PLUGIN_PLUGIN_HANDLE_HPP
#define GOTT_BASE_PLUGIN_PLUGIN_HANDLE_HPP

#include "plugin_base.hpp"
#include "module.hpp"
#include <boost/optional/optional_fwd.hpp>
#include <boost/scoped_ptr.hpp>

namespace gott {
class string;

namespace plugin {

class plugin_handle_base {
public:
  plugin_handle_base(plugin_metadata const &which) GOTT_EXPORT;
  GOTT_EXPORT
  plugin_handle_base(boost::optional<plugin_metadata const &> const &which);
  ~plugin_handle_base() GOTT_EXPORT;

  plugin_base *get_base() const GOTT_EXPORT;

private:
  class impl;
  boost::scoped_ptr<impl> p;
};    

template<class ConcretePlugin>
class plugin_handle : public plugin_handle_base {
public:
  plugin_handle(plugin_metadata const &which)
  : plugin_handle_base(which),
    p(dynamic_cast<ConcretePlugin *>(get_base())) {}
  plugin_handle(boost::optional<plugin_metadata const &> const &which)
  : plugin_handle_base(which),
    p(dynamic_cast<ConcretePlugin *>(get_base())) {}

  ~plugin_handle() {}

  ConcretePlugin &operator*() { return *p; }
  ConcretePlugin const &operator*() const { return *p; }
  ConcretePlugin *operator->() { return p; }
  ConcretePlugin const *operator->() const { return p; }
  ConcretePlugin *get() { return p; }
  ConcretePlugin const *get() const { return p; }
private:
  ConcretePlugin *p;
};

}}

#endif
