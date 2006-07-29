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
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <utility>

using std::size_t;
using std::pair;
using std::make_pair;
using boost::get;
namespace schema = tdl::schema;
using schema::item;
using schema::rule_t;
using schema::slotcfg;

class slotcfg::impl {
public:
  mode m;

  typedef
    boost::variant<
      size_t,
      pair<std::size_t, std::size_t>,
      callback
    >
  type_t;

  type_t type;

  enum { cancelled = size_t(-1) };
  size_t count;

  impl(mode mm, type_t tt = size_t(1), size_t cc = 0)
  : m(mm), type(tt), count(cc) {}
};

slotcfg::slotcfg() 
: p(new impl(one)) {}

slotcfg::slotcfg(slotcfg const &b)
: p(new impl(b.p->m, b.p->type, b.p->count)) {}

slotcfg::slotcfg(simple_mode mm) 
: p(new impl(mm)) {}

slotcfg::slotcfg(sized_mode mm, size_t s)
: p(new impl(mm, s)) {}

slotcfg::slotcfg(range_mode mm, size_t a, size_t b)
: p(new impl(mm, make_pair(a, b))) {}

slotcfg::slotcfg(function_mode mm, callback const &cb)
: p(new impl(mm, cb)) {}

slotcfg::~slotcfg() {}

void slotcfg::operator=(slotcfg const &b) {
  p.reset(new impl(b.p->m, b.p->type, b.p->count));
}

bool slotcfg::operator==(slotcfg const &b) const {
  if (p->m != b.p->m)
    return false;
  switch (p->type.which()) {
  case 0:
    return get<size_t>(p->type) == get<size_t>(b.p->type);
  case 1:
    return get<pair<size_t, size_t> >(p->type)
      == get<pair<size_t, size_t> >(b.p->type);
  case 2:
    return get<callback>(&p->type) == get<callback>(&b.p->type);
  default:
    return false;
  }
}

void slotcfg::add() {
  ++p->count;
}

slotcfg::mode slotcfg::get_mode() const {
  return p->m;
}

item::expect slotcfg::expectation() const {
  if (p->count == impl::cancelled)
    return item::nothing;
  
  switch (p->m) {
  case one:
    if (p->count == 1)
      return item::nothing;
    else if (p->count == 0)
      return item::need;
    else
      return item::over_filled;

  case optional:
    if (p->count == 0)
      return item::maybe;
    else if (p->count == 1)
      return item::nothing;
    else
      return item::over_filled;

  case list:
    return item::maybe;

  case some:
    if (p->count > 0)
      return item::maybe;
    else
      return item::need;

  case exactly:
    if (p->count < get<size_t>(p->type))
      return item::need;
    else if (p->count == get<size_t>(p->type))
      return item::nothing;
    else
      return item::over_filled;

  case minimum:
    if (p->count >= get<size_t>(p->type))
      return item::maybe;
    else
      return item::need;

  case maximum:
    if (p->count < get<size_t>(p->type))
      return item::maybe;
    else if (p->count == get<size_t>(p->type))
      return item::nothing;
    else
      return item::over_filled;

  case range:
    if (p->count < get<pair<size_t, size_t> >(p->type).first)
      return item::need;
    if (p->count >= get<pair<size_t, size_t> >(p->type).first
        && p->count < get<pair<size_t, size_t> >(p->type).second)
      return item::maybe;
    else if (p->count == get<pair<size_t, size_t> >(p->type).second)
      return item::nothing;
    else
      return item::over_filled;

  case function:
    return get<callback>(p->type)(p->count);
  }

  throw std::bad_exception();
}

void slotcfg::cancel() {
  p->count = impl::cancelled;
}

bool slotcfg::prefix_optional() const {
  switch (p->m) {
  case optional:
  case list:
  case maximum:
    return true;

  case minimum:
    return get<size_t>(p->type) == 0;

  case range:
    return get<pair<size_t, size_t> >(p->type).first == 0;

  case function:
    return get<callback>(p->type)(0) > 0;
  }

  return false;
}

namespace {
  struct denull {
    denull(slotcfg::callback const &c) : internal(c) {}
    slotcfg::callback internal;

    item::expect operator() (size_t i) const {
      if (i == 0)
        return item::need;
      return internal(i);
    }
  };
}

slotcfg slotcfg::no_optional() const {
  switch (p->m) {
  case optional:
    return slotcfg(one);
  case list:
    return slotcfg(some);
  case maximum:
    return slotcfg(range, 1, get<size_t>(p->type));

  case minimum:
    return slotcfg(minimum, !get<size_t>(p->type) ? 1 : get<size_t>(p->type));

  case range:
    if (get<pair<size_t, size_t> >(p->type).first == 0)
      return slotcfg(range, 1, get<pair<size_t, size_t> >(p->type).second);
    return *this;

  case function:
    return slotcfg(function, callback(denull(get<callback>(p->type))));
  }

  throw std::bad_exception();
}
