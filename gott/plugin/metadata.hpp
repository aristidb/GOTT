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

#ifndef GOTT_PLUGIN_METADATA_HPP
#define GOTT_PLUGIN_METADATA_HPP

#include <gott/string/string.hpp>
#include <gott/string/qid.hpp>
#include <boost/function.hpp>
#include <boost/optional/optional.hpp>
#include <boost/none.hpp>
#include <boost/ref.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/parameter/parameters.hpp>
#include <boost/parameter/macros.hpp>
#include <boost/parameter/binding.hpp>
#include <vector>
#include <iosfwd>

namespace gott { namespace plugin {

GOTT_EXPORT
void load_standard_metadata();

/**
 * A plugin's metadata.
 */
struct plugin_metadata {
  QID plugin_id;

  typedef std::vector<QID> interface_list_t;
  interface_list_t interfaces;

  enum module_type_t { dynamic_native };
  module_type_t module_type;

  gott::string file_path;

  plugin_metadata()
  : module_type(dynamic_native) {}
};

/**
 * Send all plugins' metadata records that fullfill some (or none) requirements
 * to a callback. Thread-safe.
 * \param callback The function to send the plugins' metadata to.
 * \param plugin_id Requires the record to have the given plugin-id if set.
 * \param interface Requires the record to indicate support for the given
 *                 interface if set.
 * \param cancel_early Cancel after the first record was found.
 * \see enumerate_plugin_metadata
 */
GOTT_EXPORT
void enumerate_plugin_metadata_p(
    boost::function<void (plugin_metadata const &)> const &callback,
    boost::optional<QID const &> const &plugin_id,
    boost::optional<QID const &> const &interface,
    bool cancel_early);

#ifndef IN_DOXY
namespace tags {
  BOOST_PARAMETER_KEYWORD(detail, callback)
  BOOST_PARAMETER_KEYWORD(detail, plugin_id)
  BOOST_PARAMETER_KEYWORD(detail, interface)
  BOOST_PARAMETER_KEYWORD(detail, cancel_early)
}

typedef boost::parameter::parameters<
  tags::detail::callback,
  tags::detail::plugin_id,
  tags::detail::interface,
  tags::detail::cancel_early
> epm_params;

BOOST_PARAMETER_FUN(void, enumerate_plugin_metadata, 1, 4, epm_params);

namespace detail {
  template<class T>
  QID get_opt_qid(T const &x) {
    return x;
  }

  inline QID const &get_opt_qid(QID const &x) {
    return x;
  }

  inline boost::none_t get_opt_qid(boost::none_t) {
    return boost::none;
  }
}

template<class ArgPack>
void enumerate_plugin_metadata_with_named_params(ArgPack const &args) {
  enumerate_plugin_metadata_p(args[tags::callback],
      detail::get_opt_qid(args[tags::plugin_id | boost::none]),
      detail::get_opt_qid(args[tags::interface | boost::none]),
      args[tags::cancel_early | false]);
}

typedef boost::parameter::parameters<
  tags::detail::plugin_id,
  tags::detail::interface
> fpm_params;

BOOST_PARAMETER_FUN(inline boost::optional<plugin_metadata const &>,
    find_plugin_metadata, 0, 2, fpm_params);

namespace detail {
  struct find_functor {
    find_functor() {}
    boost::optional<plugin_metadata const &> result;
    void operator() (plugin_metadata const &x) {
      result = x;
    }
  };
}

template<class ArgPack>
boost::optional<plugin_metadata const &>
find_plugin_metadata_with_named_params(ArgPack const &args) {
  detail::find_functor cb;
  enumerate_plugin_metadata_p(boost::ref(cb),
      detail::get_opt_qid(args[tags::plugin_id | boost::none]),
      detail::get_opt_qid(args[tags::interface | boost::none]),
      true);
  return cb.result;
}
#else
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
void enumerate_plugin_metadata(
    boost::function<void (plugin_metadata const &)> const &tags::callback,
    QID const &tags::plugin_id,
    QID const &tags::interface,
    bool tags::cancel_early = false
    );

/**
 * Find a plugin's metadata that fulfills some requirements and return it if any
 * was found.
 * \param tags::plugin_id Requires the record to have the given plugin-id
 *                        (optional).
 * \param tags::interface Requires the record to indicate support for the given
 *                        interface (optional).
 */
boost::optional<plugin_metadata const &>
find_plugin_metadata(
    QID const &tags::plugin_id,
    QID const &tags::interface
    );
#endif

/**
 * Add a plugin or rather its' metadata to the relevant (in-memory) database.
 * This function does not record the plugin persistently. Thread-safe.
 * \param metadata The new metadata.
 */
GOTT_EXPORT
void add_plugin_metadata(plugin_metadata const &metadata);

/**
 * Add a list of some plugins' metadata read from a stream. Thread-safe.
 * \param stream The stream to empty.
 */
GOTT_EXPORT
void extract_plugin_metadata(std::istream &stream);

/**
 * Read a plugin's metadata from a stream.
 * \param stream The stream to read from.
 * \param out The metadata object to fill.
 */
GOTT_EXPORT
std::istream &operator>>(std::istream &stream, plugin_metadata &out);

/**
 * Write a plugin's metadata to a stream.
 * \param stream The stream to write to.
 * \param in The metadata object to write.
 */
GOTT_EXPORT
std::ostream &operator<<(std::ostream &stream, plugin_metadata const &in);

}}

#endif
