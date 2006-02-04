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
 * The Original Code is An Event Handling Class Library.
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
 * ***** END LICENSE BLOCK ***** *//

#include "loop_requirement.hpp"
#include "main_loop_factory.hpp"
#include <boost/variant.hpp>
#include <boost/tuple/tuple.hpp>

using boost::variant;
using boost::tuple;
using gott::events::loop_requirement;

class loop_requirement::impl {
public:
  typedef tuple<loop_requirement, loop_requirement, combiner_t> combined;
  typedef tuple<QID const &, feature_flag_t> feature;
  typedef 
    variant<
      feature,
      combined
    > data_t;
  data_t data;

  impl(data_t const &d) : data(d) {}
};

loop_requirement::loop_requirement(
    loop_requirement const &lhs,
    loop_requirement const &rhs,
    combiner_t comb)
: p(new impl(impl::combined(lhs, rhs, comb))) {}

loop_requirement::loop_requirement(
    QID const &id,
    feature_flag_t indicator)
: p(new impl(impl::feature(id, indicator))) {}

bool loop_requirement::do_try(main_loop_factory &x) const {
  if (p->data.type() == typeid(impl::combined)) {
    impl::combined const &comb = boost::get<impl::combined>(p->data);
    switch (comb.get<2>()) {
    case combine_and:
      return comb.get<0>().do_try(x) && comb.get<1>().do_try(x);
    case combine_or:
      return comb.get<0>().do_try(x) || comb.get<1>().do_try(x);
    default: throw 0;
    }
  } else if (p->data.type() == typeid(impl::feature)) {
    return x.try_add_feature(boost::get<impl::feature>(p->data).get<0>());
  } else throw 0;
}
