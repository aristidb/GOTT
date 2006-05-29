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

#ifndef GOTT_METADATA_PLUGIN_HPP
#define GOTT_METADATA_PLUGIN_HPP

#include "detail/param.hpp"
#include "detail/handle.hpp"
#include <gott/string/string.hpp>
#include <gott/string/qid.hpp>
#include <boost/function.hpp>
#include <boost/optional/optional.hpp>
#include <boost/ref.hpp>
#include <boost/parameter/parameters.hpp>
#include <boost/parameter/macros.hpp>
#include <boost/scoped_ptr.hpp>

namespace gott { namespace metadata {

class module;
class interface;

/**
 * A plugin's metadata.
 */
class plugin {
public:
  explicit plugin(handle_t handle) : handle(handle) {}

  /**
   * The plugin's unique identifier.
   */
  GOTT_EXPORT
  QID plugin_id() const;

  GOTT_EXPORT
  bool supports_interface(interface const &) const;

  GOTT_EXPORT
  bool supports_interface_id(QID const &) const;

  GOTT_EXPORT
  void enumerate_supported_interfaces(
      boost::function<void (interface const &)> const &callback) const;

  /**
   * The module the plugin resides in.
   */
  GOTT_EXPORT
  module enclosing_module() const;

  /**
   * The entry smbol of the plugin.
   */
  GOTT_EXPORT
  string symbol() const;

  handle_t const &get_handle() const { return handle; }

public: //internal
  bool is_valid() const;

private:
  handle_t handle;
};

/**
 * Send all plugins' metadata records that fullfill some (or none) requirements
 * to a callback. Thread-safe.
 * \param callback The function to send the plugins' metadata to.
 * \param plugin_id Requires the record to have the given plugin-id if set.
 * \param interface Requires the record to indicate support for the given
 *                 interface if set.
 * \param cancel_early Cancel after the first record was found.
 * \param load_standard_metadata Call load_standard_metadata() first.
 * \see enumerate_plugin
 */
GOTT_EXPORT
void enumerate_plugins_p(
    boost::function<void (plugin const &)> const &callback,
    boost::optional<QID const &> const &plugin_id,
    boost::optional<QID const &> const &interface,
    bool cancel_early,
    bool load_standard_metadata,
    bool validate);

#ifndef IN_DOXY
typedef boost::parameter::parameters<
  tags::detail::callback,
  tags::detail::plugin_id,
  tags::detail::interface_id,
  tags::detail::cancel_early,
  tags::detail::load_standard_metadata,
  tags::detail::validate
> epm_params;

BOOST_PARAMETER_FUN(void, enumerate_plugins, 1, 6, epm_params);

template<class ArgPack>
void enumerate_plugins_with_named_params(ArgPack const &args) {
  enumerate_plugins_p(args[tags::callback],
      detail::get_opt_qid(args[tags::plugin_id | boost::none]),
      detail::get_opt_qid(args[tags::interface_id | boost::none]),
      args[tags::cancel_early | false],
      args[tags::load_standard_metadata | true],
      args[tags::validate | true]);
}

typedef boost::parameter::parameters<
  tags::detail::plugin_id,
  tags::detail::interface_id,
  tags::detail::load_standard_metadata,
  tags::detail::validate
> fpm_params;

BOOST_PARAMETER_FUN(inline boost::optional<plugin>,
    find_plugin, 0, 4, fpm_params);

template<class ArgPack>
boost::optional<plugin>
find_plugin_with_named_params(ArgPack const &args) {
  detail::find_functor<plugin> cb;
  enumerate_plugins_p(boost::ref(cb),
      detail::get_opt_qid(args[tags::plugin_id | boost::none]),
      detail::get_opt_qid(args[tags::interface_id | boost::none]),
      true,
      args[tags::load_standard_metadata | true],
      args[tags::validate | true]);
  return cb.result;
}
#else//=>IN_DOXY
/**
 * Send all plugins' metadata records that fullfill some (or none) requirements
 * to a callback. Supports named parameters.
 * \param tags::callback The function to send the plugins' metadata to.
 * \param tags::plugin_id Requires the record to have the given plugin-id
 *                       (optional).
 * \param tags::interface Requires the record to indicate support for the given
 *                       interface (optional).
 * \param tags::cancel_early Cancel after the first record was found.
 */
void enumerate_plugins(
    boost::function<void (plugin const &)> const &tags::callback,
    QID const &tags::plugin_id,
    QID const &tags::interface,
    bool tags::cancel_early = false,
    bool tags::load_standard_metadata = true,
    bool tags::validate = true
    );

/**
 * Find a plugin's metadata that fulfills some requirements and return it if any
 * was found.
 * \param tags::plugin_id Requires the record to have the given plugin-id
 *                        (optional).
 * \param tags::interface Requires the record to indicate support for the given
 *                        interface (optional).
 */
boost::optional<plugin>
find_plugin(
    QID const &tags::plugin_id,
    QID const &tags::interface,
    bool tags::load_standard_metadata = true,
    bool tags::validate = true
    );
#endif

}}

#endif
