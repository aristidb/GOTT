// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef GOTT_UTIL_TDL_SCHEMA_RULE_HPP
#define GOTT_UTIL_TDL_SCHEMA_RULE_HPP

#include <boost/shared_ptr.hpp>
#include <gott/util/visibility.hpp>
#include <ntl.h>

namespace gott {
class string;

namespace tdl {
namespace schema {

class rule_attr;
class item;
class rule_t;
class match;

template<class T>
item *
construct_item(rule_attr const &att, Vector<rule_t> const&children, match &m) {
  return new T(att, children, m);
}

typedef 
item *(*item_constructor)(rule_attr const &, Vector<rule_t> const &, match &);

typedef
bool (*item_check)(rule_attr const &, Vector<rule_t> const &);

struct abstract_rule : Moveable<abstract_rule> {
  explicit abstract_rule(item_constructor c, item_check ae) 
  : constructor(c), accept_empty(ae) {}
  item_constructor constructor;
  item_check accept_empty;
};

/**
 * Rule-factory to produce item objects.
 */
class GOTT_EXPORT rule_t : Moveable<rule_t> {
public:
  rule_t();
  rule_t(abstract_rule const &, rule_attr const &, Vector<rule_t> pick_ &);
  rule_t(rule_t const &);
  explicit rule_t(rule_t const *);
  ~rule_t();

  void operator=(rule_t const &);

  item *get(match &) const;
  rule_attr const &attributes() const;
  bool accept_empty() const;

private:
  class IMPL;
  boost::shared_ptr<IMPL const> p;
};

template<class T>
rule_t rule(rule_attr const &a, Vector<rule_t> const &c = Vector<rule_t>()) {
  return rule_t(abstract_rule(&construct_item<T>, &T::accept_any), a, c);
}

rule_t rule(string const &name, rule_attr const &a, Vector<rule_t> const &c =
    Vector<rule_t>()) GOTT_EXPORT;

}}}

#endif
