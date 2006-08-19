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

#ifndef GOTT_PLUGIN_MULTI_PLUGIN_HPP
#define GOTT_PLUGIN_MULTI_PLUGIN_HPP

#include <gott/plugin/selector.hpp>
#include <boost/function.hpp>
#include <sigc++/connection.h>
#include <boost/noncopyable.hpp>

namespace gott { namespace plugin {

namespace detail { 
  bool do_not_accept(plugin_descriptor const &) { return false; }
}

/**
 * Use all plugins with given criteria.
 */
class multi_plugin : boost::noncopyable {
public:
  typedef boost::function<void (plugin_descriptor const &)> add_callback_t;
  typedef boost::function<bool (plugin_descriptor const &)> remove_callback_t;

  multi_plugin(
      selector const &selector_,
      add_callback_t const &add_callback)
  : sel(selector_), add(add_callback), remove(detail::do_not_accept) {
    update();
    inscribe();
  }

  multi_plugin(
      selector const &selector_,
      add_callback_t const &add_callback,
      remove_callback_t const &remove_callback)
  : sel(selector_), add(add_callback), remove(remove_callback) {
    update();
    inscribe();
  }

  ~multi_plugin() { conn.disconnect(); }

private:
  GOTT_EXPORT void update();
  GOTT_EXPORT void inscribe();

  std::vector<plugin_descriptor> current;
  selector sel;
  add_callback_t add;
  remove_callback_t remove;
  sigc::connection conn;
};

}}

#endif
