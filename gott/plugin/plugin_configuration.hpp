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
 * Portions created by the Initial Developer are Copyright (C) 2004-2005
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

#ifndef GOTT_BASE_PLUGIN_PLUGIN_CONFIGURATION_HPP
#define GOTT_BASE_PLUGIN_PLUGIN_CONFIGURATION_HPP

#include <gott/visibility.hpp>
#include <boost/scoped_ptr.hpp>

namespace gott {
class QID;
namespace xany { class Xany; }
namespace metadata { struct plugin; }

namespace plugin {
class hook;

class plugin_configuration {
public:
  plugin_configuration(metadata::plugin const &) GOTT_EXPORT;
  plugin_configuration(plugin_configuration const &) GOTT_EXPORT;
  ~plugin_configuration() GOTT_EXPORT;

  void add_param(QID const &, xany::Xany const &) GOTT_EXPORT;
  xany::Xany const &find_param(QID const &) const GOTT_EXPORT;

  void add_hook(QID const &, hook const &) GOTT_EXPORT;
  hook const &find_hook(QID const &) const GOTT_EXPORT;

private:
  class impl;
  boost::scoped_ptr<impl> p;
};

}}

#endif
