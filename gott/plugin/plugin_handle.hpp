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
#include "selector.hpp"
#include "descriptor.hpp"
#include <boost/optional/optional.hpp>
#include <boost/shared_ptr.hpp>

namespace gott {
class string;

namespace plugin {

class selector;
class plugin_descriptor;

class plugin_handle_base {
public:
  GOTT_EXPORT
  plugin_handle_base(selector const &sel);

  GOTT_EXPORT
  plugin_handle_base(plugin_descriptor const &desc);
  
  ~plugin_handle_base() GOTT_EXPORT;

  plugin_base *get_base() const GOTT_EXPORT;

protected:
  void fail_interface(string const &which) GOTT_EXPORT;

private:
  class impl;
  boost::shared_ptr<impl const> p;
};   

/**
 * Handle class for plugins.
 * \param Interface The plugin's interface.
 */
template<class Interface>
class plugin_handle : public plugin_handle_base {
public:
  /**
   * Constructor. Find a plugin with the correct interface.
   */
  plugin_handle()
    : plugin_handle_base(with_interface<Interface>()),
      p(cast(get_base())) {
    if (!p) fail_interface(with_interface<Interface>().to_string());
  }

  /**
   * Constructor. Find a plugin that has the correct interface AND matches
   * a given selector.
   * \param sel The selector to match.
   */
  plugin_handle(selector const &sel)
    : plugin_handle_base(sel && with_interface<Interface>()),
      p(cast(get_base())) {
    if (!p) fail_interface(sel.to_string());
  }

  /**
   * Constructor. Just load a given plugin.
   */
  plugin_handle(plugin_descriptor const &desc)
    : plugin_handle_base(desc),
      p(cast(get_base())) {
    if (!p) fail_interface(desc.to_string());
  }

  /// Destructor.
  ~plugin_handle() {}

  /// Use the plugin.
  Interface &operator*() { return *p; }
  /// Use the plugin.
  Interface const &operator*() const { return *p; }
  /// Use the plugin.
  Interface *operator->() { return p; }
  /// Use the plugin.
  Interface const *operator->() const { return p; }
  /// Use the plugin.
  Interface *get() { return p; }
  /// Use the plugin.
  Interface const *get() const { return p; }

private:
  Interface *p;

  static Interface *cast(plugin_base *base) {
    return static_cast<Interface *>(base);
  }
};

}}

#endif
