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
 * Portions created by the Initial Developer are Copyright (C) 2004-2005
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

#ifndef GOTT_BASE_PLUGIN_PLUGIN_HPP
#define GOTT_BASE_PLUGIN_PLUGIN_HPP

#include <gott/visibility.hpp>
#include <gott/xany/xany.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <vector>
#include <map>

namespace gott {
class QID;

namespace plugin {

class system_configuration;
class plugin_configuration;
class hook;

class GOTT_EXPORT plugin_base {
public:
  typedef std::vector<gott::xany::Xany> parameter_list_t;
  virtual ~plugin_base() = 0;
  virtual void add(QID const &point, hook const &extension) = 0;
  virtual QID class_id() const = 0;
  gott::xany::Xany run_method(QID const &id, 
      parameter_list_t const &parameters);
protected:
  typedef
    boost::function<gott::xany::Xany (plugin_base *, parameter_list_t const &)>
    function_entry_t;
  virtual boost::optional<function_entry_t> 
    find_method(QID const &id) const = 0;
};

#define GOTT_PLUGIN_DECLARE_METHOD_REGISTRY \
  boost::optional<function_entry_t> find_method(gott::QID const &id) const

#define GOTT_PLUGIN_METHOD_REGISTRY_BEGIN(type) \
  boost::optional<plugin_base::function_entry_t> \
  type::find_method(gott::QID const &id) const { \
    typedef std::map<gott::QID, function_entry_t> method_map_t; \
    static const method_map_t method_handlers; \
    if (method_handlers.empty()) {

#define GOTT_PLUGIN_METHOD_REGISTRY_ENTRY(id, fun) \
      method_handlers[id] = fun;

#define GOTT_PLUGIN_METHOD_REGISTRY_END() \
    } \
    method_map_t::const_iterator it = method_handlers.find(id); \
    if (it != method_handlers.end()) \
      return it->second; \
  }

template<class Interface, class Dispatcher>
class remote_plugin : public Interface {
public:
  remote_plugin(Dispatcher d = Dispatcher())
  : dispatcher(d) {}

private:
  Dispatcher dispatcher;

  boost::optional<plugin_base::function_entry_t>
  find_method(QID const &id) const {
    boost::optional<plugin_base::function_entry_t> m =
      this->Interface::find_method(id);
    if (!m) return m;
    return dispatcher.create_functor(*m);
  }
};

typedef plugin_base *(*plugin_builder)(system_configuration &, 
                                       plugin_configuration const &);

}}

#endif
