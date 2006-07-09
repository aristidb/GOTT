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

#ifndef GOTT_PLUGIN_METADATA_MANAGER_HPP
#define GOTT_PLUGIN_METADATA_MANAGER_HPP

#include <gott/string/string.hpp>
#include <gott/string/qid.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/function.hpp>

namespace gott { namespace plugin {

class plugin_descriptor;
class module_descriptor;

/**
 * A locking, thread-safe metadata manager for GOTT plugins. Only one instance
 * of this object must be active at a time. Objects of this class are used when
 * loading plugins and modules. If you do any changes to the metadata database,
 * you must call metadata_manager::commit() in order to apply them.
 */
class metadata_manager {
public:
  /**
   * Constructor. There can not be more than one metadata_manager at a time.
   */
  GOTT_EXPORT
  metadata_manager();

  /**
   * Destructor. Does not commit any changes to the database so be sure to do
   * this yourself.
   */
  GOTT_EXPORT
  ~metadata_manager();

  /**
   * Commits all changes to the database.
   */
  GOTT_EXPORT
  void commit();

  /**
   * Remove all data associated with the resource. The identifier should be a
   * file name. The file will not be deleted or accessed in any way.
   * \param resource The resource to remove.
   */
  GOTT_EXPORT
  void remove_resource(gott::string const &resource);

  /**
   * Update or add a resource.
   * \param stream The physical resource.
   * \param resource The name of the resource.
   */
  GOTT_EXPORT
  void update_resource(std::istream &stream, string const &resource);

public: //internal
  void load_core();
  void load_standard();

  void enum_plugins(
      boost::function<bool (plugin_descriptor const &)> const &callback,
      boost::optional<QID> const &plugin_id,
      boost::optional<QID> const &interface_id,
      boost::optional<QID> const &module_id);

  void enum_modules(
      boost::function<bool (module_descriptor const &)> const &callback,
      boost::optional<QID> const &plugin_id,
      boost::optional<QID> const &interface_id,
      boost::optional<QID> const &module_id);


private:
  class impl;
  boost::scoped_ptr<impl> p;
};

}}

#endif
