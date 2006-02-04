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

#ifndef GOTT_UTIL_TDL_SCHEMA_RULE_HPP
#define GOTT_UTIL_TDL_SCHEMA_RULE_HPP

#include "rule_attr.hpp"
#include <boost/shared_ptr.hpp>
#include <gott/visibility.hpp>
#include <vector>

namespace gott {
class string;

namespace tdl {
namespace schema {

class rule_attr_t;
class item;
class rule_t;
class match;

template<class T>
item *
construct_item(rule_attr_t const &att, std::vector<rule_t> const &children, 
    match &m) {
  return new T(att, children, m);
}

typedef 
item *(*item_constructor)(rule_attr_t const &, std::vector<rule_t> const &, 
    match &);

typedef
bool (*item_check)(rule_attr_t const &, std::vector<rule_t> const &);

struct abstract_rule {
  explicit abstract_rule(item_constructor c, item_check ae) 
  : constructor(c), accept_empty(ae) {}
  item_constructor constructor;
  item_check accept_empty;
};

/**
 * Rule-factory to produce item objects.
 */
class GOTT_EXPORT rule_t {
public:
  rule_t();
  rule_t(abstract_rule const &, rule_attr_t const &, std::vector<rule_t>const&);
  rule_t(rule_t const &);
  explicit rule_t(rule_t const *);
  ~rule_t();

  void operator=(rule_t const &);

  item *get(match &) const;
  rule_attr_t const &attributes() const;
  bool accept_empty() const;

private:
  class impl;
  boost::shared_ptr<impl const> p;
};

template<class T>
rule_t rule(rule_attr_t const &a, 
    std::vector<rule_t> const &c = std::vector<rule_t>()) {
  return rule_t(abstract_rule(&construct_item<T>, &T::accept_any), a, c);
}

rule_t rule(string const &name, rule_attr_t const &a = rule_attr_t(), 
    std::vector<rule_t> const &c = std::vector<rule_t>()) GOTT_EXPORT;

inline
rule_t rule_one(string const &name, rule_attr_t const &a, rule_t const &c) {
  return rule(name, a, std::vector<rule_t>(1, c));
}

inline
rule_t rule(string const &name, std::vector<rule_t> const &c) {
  return rule(name, rule_attr_t(), c);
}

inline
rule_t rule_one(string const &name, rule_t const &c) {
  return rule(name, rule_attr_t(), std::vector<rule_t>(1, c));
}

}}}

#endif
