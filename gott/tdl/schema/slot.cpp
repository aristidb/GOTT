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
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
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

#include "slot.hpp"
#include "rule.hpp"
#include <boost/variant/get.hpp>

using std::size_t;
using std::pair;
using std::make_pair;
using boost::get;
namespace schema = tdl::schema;
using schema::item;
using schema::rule_t;
using schema::slotcfg;

item::expect slotcfg::expectation() const {
  if (count == cancelled)
    return item::nothing;
  
  switch (m) {
  case one:
    if (count == 1)
      return item::nothing;
    else if (count == 0)
      return item::need;
    else
      return item::over_filled;

  case optional:
    if (count == 0)
      return item::maybe;
    else if (count == 1)
      return item::nothing;
    else
      return item::over_filled;

  case list:
    return item::maybe;

  case some:
    if (count > 0)
      return item::maybe;
    else
      return item::need;

  case exactly:
    if (count < get<size_t>(type))
      return item::need;
    else if (count == get<size_t>(type))
      return item::nothing;
    else
      return item::over_filled;

  case minimum:
    if (count >= get<size_t>(type))
      return item::maybe;
    else
      return item::need;

  case maximum:
    if (count < get<size_t>(type))
      return item::maybe;
    else if (count == get<size_t>(type))
      return item::nothing;
    else
      return item::over_filled;

  case range:
    if (count < get<pair<size_t, size_t> >(type).first)
      return item::need;
    if (count >= get<pair<size_t, size_t> >(type).first
        && count < get<pair<size_t, size_t> >(type).second)
      return item::maybe;
    else if (count == get<pair<size_t, size_t> >(type).second)
      return item::nothing;
    else
      return item::over_filled;

  case function:
    return get<callback>(type)(count);
  }

  throw std::bad_exception();
}

bool slotcfg::prefix_optional() const {
  switch (m) {
  case optional:
  case list:
  case maximum:
    return true;

  case exactly:
  case minimum:
    return get<size_t>(type) == 0;

  case range:
    return get<pair<size_t, size_t> >(type).first == 0;

  case function:
    return get<callback>(type)(0) > 0;
  }

  return false;
}

