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
 * The Original Code is A Human Computer Interaction Library.
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

#ifndef GOTT_HCI_CONTAINER_HPP
#define GOTT_HCI_CONTAINER_HPP

#include "object.hpp"
#include <boost/ptr_container/ptr_vector.hpp>

namespace gott { namespace hci {

/**
 * Container object. Contains an array of objects.
 */
class GOTT_EXPORT container : public object {
public:
  object *find(path_type const &path);
  object *find_named(string const &name);
  void depth_first(
    boost::function<bool (path_type const &, object *)> const &callback,
    size_type max_depth = npos,
    path_type const &prepend = path_type());

  /**
   * Add an object to the back of this container. Ownership of the object will
   * be claimed.
   * \param child The object to add.
   */
  void add(object *child);

  /**
   * Get the size of this container.
   */
  GOTT_LOCAL size_type size() const { return children.size(); }

private:
  typedef boost::ptr_vector<object> vector;
  vector children;
};

}}

#endif
