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

#include "atom.hpp"
#include <map> // TODO: use hash map
#include <boost/functional/hash.hpp>
#include <boost/thread/mutex.hpp>

using gott::atom;
using gott::string;

namespace {
  boost::mutex mutex;
}

static string *atomize(string const &n) {
  boost::mutex::scoped_lock lock(mutex);

  static std::map<string, string *> table;
  std::map<string, string *>::iterator it = table.find(n);
  if (it == table.end()) {
    string *x = new string(n.as_utf8(), gott::utf8);
    table.insert(std::map<string, string *>::value_type(*x, x));
    return x;
  }
  return it->second;
}

atom::atom(string const &n) : rep(atomize(n)) {}

size_t gott::hash_value(atom const &a) {
  boost::hash<string *>  hash;
  return hash(a.rep);
}
