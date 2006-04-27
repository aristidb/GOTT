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
 * The Original Code is A Plugin / Loadable Module Engine.
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

#ifndef GOTT_PLUGIN_MODULE_METADATA_HPP
#define GOTT_PLUGIN_MODULE_METADATA_HPP

#include "param_detail.hpp"
#include <gott/string/qid.hpp>
#include <gott/string/string.hpp>
#include <boost/function.hpp>
#include <boost/optional/optional.hpp>
#include <boost/parameter/parameters.hpp>
#include <boost/parameter/macros.hpp>
#include <iosfwd>

namespace gott { namespace plugin {

struct module_metadata {
  /**
   * The module's unique identifier.
   */
  QID module_id;

  /**
   * The types a module might have.
   */
  enum module_type_t { dynamic_native };

  /**
   * The type of the module.
   */
  module_type_t module_type;

  /**
   * The file where the plugin resides.
   */
  string file_path;

  module_metadata() : module_type(dynamic_native) {}
};

GOTT_EXPORT
void enumerate_module_metadata_p(
    boost::function<void (module_metadata const &)> const &callback,
    boost::optional<QID const &> const &module_id,
    bool cancel_early
    );

#ifndef IN_DOXY
typedef boost::parameter::parameters<
    tags::detail::callback,
    tags::detail::module_id,
    tags::detail::cancel_early
  > emm_params;

BOOST_PARAMETER_FUN(void, enumerate_module_metadata, 1, 3, emm_params);

template<class ArgPack>
void enumerate_module_metadata_with_named_params(ArgPack const &args) {
  enumerate_module_metadata_p(
      args[tags::callback],
      detail::get_opt_qid(args[tags::module_id | boost::none]),
      args[tags::cancel_early | false]);
}

typedef boost::parameter::parameters<
    tags::detail::module_id
  > fmm_params;

BOOST_PARAMETER_FUN(
    inline boost::optional<module_metadata const &>, find_module_metadata,
    0, 1, fmm_params);

template<class ArgPack>
boost::optional<module_metadata const &>
find_module_metadata_with_named_params(ArgPack const &args) {
  detail::find_functor<module_metadata> cb;
  enumerate_module_metadata_p(
      boost::ref(cb),
      detail::get_opt_qid(args[tags::module_id | boost::none]),
      true);
  return cb.result;
}
#else//=>IN_DOXY
void enumerate_module_metadata(
    boost::function<void (module_metadata const &)> const &tags::callback,
    QID const &tags::module_id,
    bool tags::cancel_early = false);

boost::optional<module_metadata const &>
find_module_metadata(
    QID const &tags::module_id);
#endif

GOTT_EXPORT void add_module_metadata(module_metadata const &);

GOTT_EXPORT void extract_module_metadata(std::istream &stream);

GOTT_EXPORT
std::istream &operator>>(std::istream &stream, module_metadata &out);

GOTT_EXPORT
std::ostream &operator<<(std::ostream &stream, module_metadata const &in);

}}

#endif
