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

#ifndef GOTT_HCI_REFERENCE_HPP
#define GOTT_HCI_REFERENCE_HPP

#include "object.hpp"

namespace gott { namespace hci {

/**
 * A reference to another object.
 */
class GOTT_EXPORT reference : public object {
public:
  /**
   * Constructor.
   * \param referee The object to refer to.
   */
  GOTT_LOCAL reference(object *referee) : referee(referee) {}

  /// Destructor.
  ~reference();

  /**
   * Get the object this reference refers to.
   */
  GOTT_LOCAL object *get() { return referee; }

  /**
   * Get the object this reference refers to.
   */
  GOTT_LOCAL object const *get() const { return referee; }

  void *domain_specific(QID const &domain);

  object *find(path_type const &path, size_type offset);

  bool first_child(path_element &) const;

private:
  object *referee;
};

}}

#endif
