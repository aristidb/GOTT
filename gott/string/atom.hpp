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
 * The Original Code is A String and Text Storage Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#ifndef GOTT_UTIL_ATOM_ATOM_HPP
#define GOTT_UTIL_ATOM_ATOM_HPP

#include <gott/string/string.hpp>

namespace gott {

/**
 * Atom class.
 * Atoms are strings with fast comparison and fast hashing but slow constructor
 * and medium-slow copying.
 */
class atom {
public:
  /// Copy-Constructor (fast).
  atom(atom const &o) : rep(o.rep) {}

  /// Constructor (slow).
  GOTT_EXPORT atom(string const & = string(""));

  /// Destructor.
  ~atom() {}

  string const &get_string() const { return *rep; }

  bool operator==(atom const &o) const {
    return rep == o.rep;
  }

  /**
   * Compare two atoms (fast). Does not return lexicographical order.
   */
  bool operator<(atom const &o) const {
    return rep < o.rep;
  }

  bool operator<=(atom const &o) const {
    return rep <= o.rep;
  }

private:
  string *rep;
  friend size_t hash_value(atom const &);
};

/**
 * Compare two atoms for inequality (fast).
 * \relates atom
 */
inline bool operator!=(atom const &a, atom const &b) {
  return !(a == b);
}

inline bool operator>(atom const &a, atom const &b) {
  return !(a <= b);
}

inline bool operator>=(atom const &a, atom const &b) {
  return !(a < b);
}

inline std::ostream &operator<<(std::ostream &os, atom const &x) {
  return os << x.get_string();
}

size_t hash_value(atom const &a);

}

#endif
