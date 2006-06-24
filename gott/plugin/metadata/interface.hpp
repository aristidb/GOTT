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
 * The Original Code is Metadata for GOTT.
 *
 * The Initial Developer of the Original Code is Metadata for GOTT
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

#ifndef GOTT_METADATA_INTERFACE_HPP
#define GOTT_METADATA_INTERFACE_HPP

#include "detail/param.hpp"
#include <gott/string/qid.hpp>
#include <boost/function.hpp>
#include <boost/parameter/parameters.hpp>
#include <boost/parameter/macros.hpp>

namespace gott { namespace metadata {

class interface {
public:
  interface(QID const &interface_id) : interface_id_(interface_id) {}

  QID interface_id() const { return interface_id_; }

private:
  QID interface_id_;
};

GOTT_EXPORT
void enumerate_interfaces_p(
    boost::function<void (interface const &)> const &callback,
    boost::optional<QID const &> const &interface_id,
    bool cancel_early,
    bool load_standard_metadata,
    bool validate);

typedef boost::parameter::parameters<
  tags::detail::callback,
  tags::detail::interface_id,
  tags::detail::cancel_early,
  tags::detail::load_standard_metadata,
  tags::detail::validate
> eim_params;

BOOST_PARAMETER_FUN(void, enumerate_interfaces, 1, 5, eim_params);

template<class ArgPack>
void enumerate_interfaces_with_named_params(ArgPack const &args) {
  enumerate_interfaces_p(args[tags::callback],
      detail::get_opt_qid(args[tags::interface_id | boost::none]),
      args[tags::cancel_early | false],
      args[tags::load_standard_metadata | true],
      args[tags::validate | true]);
}

typedef boost::parameter::parameters<
  tags::detail::interface_id,
  tags::detail::load_standard_metadata,
  tags::detail::validate
> fim_params;

BOOST_PARAMETER_FUN(inline boost::optional<interface>,
    find_interface, 0, 3, fim_params);

template<class ArgPack>
boost::optional<interface>
find_interface_with_named_params(ArgPack const &args) {
  detail::find_functor<interface> cb;
  enumerate_interfaces_p(boost::ref(cb),
      detail::get_opt_qid(args[tags::interface_id | boost::none]),
      true,
      args[tags::load_standard_metadata | true],
      args[tags::validate | true]);
  return cb.result;
}

}}

#endif
