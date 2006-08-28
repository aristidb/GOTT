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

#include "multi_plugin.hpp"
#include "error.hpp"
#include "descriptor.hpp"
#include "metadata_manager.hpp"
#include <boost/lambda/bind.hpp>
#include <vector>
#include <list>

using gott::plugin::multi_plugin;

void multi_plugin::update() {
  typedef std::list<plugin_descriptor> list;
  typedef std::vector<plugin_descriptor> vector;
  
  list old(current.begin(), current.end());
  current = sel.all_plugins();
  list fresh(current.begin(), current.end());

  for (list::iterator it = old.begin(); it != old.end(); ++it)
    for (list::iterator jt = fresh.begin(); jt != fresh.end(); ++jt)
      if (*it == *jt) {
        it = old.erase(it);
        if (old.begin() != it)
          --it;
        fresh.erase(jt);
        break;
      }

  for (list::iterator it = old.begin(); it != old.end(); ++it) {
    if (!remove(*it))
      current.push_back(*it);
  }

  for (list::iterator it = fresh.begin(); it != fresh.end(); ++it) {
    try {
      add(*it);
    } catch (failed_load const &e) {
      if (e.kind() != "plugin" || e.which() != it->to_string())
        throw;
      conn.block();
      metadata_manager man;
      man.remove_plugin(*it);
      man.commit();
      conn.unblock();
    }
  }
}

void multi_plugin::inscribe() {
  metadata_manager man;
  update();
  conn = man.on_update().connect(
      boost::lambda::bind(&multi_plugin::update, this));
}
