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
#include <vector>
#include <iosfwd>

namespace gott { namespace plugin {

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
};

/**
 * Send all plugins' metadata to a callback.
 * \param function The function to send the plugins' metadata to.
 */
GOTT_EXPORT
void enumerate_plugin_metadata(
    boost::function<void (plugin_metadata const &)> const &function);

/**
 * Send all plugins' metadata with a certain interface to a callback.
 */
GOTT_EXPORT
void enumerate_plugin_metadata_with_interface(
    QID const &interface_id,
    boost::function<void (plugin_metadata const &)> const &function);

/**
 * Find metadata for a specific plugin.
 * \param plugin_id The id of the plugin to search.
 */
GOTT_EXPORT
plugin_metadata const &find_plugin_metadata(QID const &plugin_id);

/**
 * Add a plugin or rather its' metadata to the relevant (in-memory) database.
 * This function does not record the plugin persistently.
 * \param metadata The new metadata.
 */
GOTT_EXPORT
void add_plugin_metadata(plugin_metadata const &metadata);

/**
 * Add a list of some plugins' metadata read from a stream.
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
