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

#include "container.hpp"

using gott::hci::container;
using gott::hci::object;
using gott::string;

container::container() {}
container::~container() {}

object *container::find(object::path_type const &path, size_type offset) {
  if (path.size() <= offset)
    return this;
  size_type index = path[offset];
  if (index >= children.size())
    return 0;
  return children[index].find(path, offset + 1);
}

object *container::find_named(string const &name) {
  for (vector::iterator it = children.begin(); it != children.end(); ++it) {
    object *result = it->find_named(name);
    if (result)
      return result;
  }
  return 0;
}

void container::depth_first(
    boost::function<bool (path_type const &, object *)> const &callback,
    size_type max_depth,
    path_type const &prepend) {
  if (!callback(prepend, this))
    return;

  if (max_depth != 0) {
    if (max_depth != npos)
      --max_depth;
    path_type newprepend(prepend.begin(), prepend.end());
    newprepend.push_back(npos);
    for (vector::iterator it = children.begin(); it != children.end(); ++it){
      newprepend.back() = (it - children.begin());
      it->depth_first(callback, max_depth, newprepend);
    }
  }
}
