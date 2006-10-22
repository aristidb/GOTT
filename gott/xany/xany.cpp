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
 * The Original Code is A Type-agnostic Storage Class.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005
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

#define GOTT_XANY_INTERNAL
#include "xany.hpp"

using namespace gott::xany;

operations_base::~operations_base() {}

bool gott::xany::operator==(Xany const &lhs, Xany const &rhs) {
  if (lhs.empty() || rhs.empty())
    return lhs.empty() && rhs.empty();
  if (!lhs.type_equals(rhs))
    return false;
  return dynamic_cast<equality_comparable const &>(lhs.get_operations())
          .equals(lhs, rhs);
}

std::ostream &gott::xany::operator<<(std::ostream &s, Xany const &v) {
  if (!v.empty()) {
    printable const &printer = 
      dynamic_cast<printable const &>(v.get_operations());
    printer.print(s, v);
  }
  return s;
}

printable::~printable() {}

equality_comparable::~equality_comparable() {}

