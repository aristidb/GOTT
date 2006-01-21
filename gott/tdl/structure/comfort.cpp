// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
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

#include "comfort.hpp"
#include "structure.hpp"
#include <boost/bind.hpp>
#include <gott/range_algo.hpp>

using gott::xany::Xany;
namespace cf = gott::tdl::structure::cf;
using cf::node_inserter_t;
using gott::tdl::structure::writable_structure;
using boost::bind;
using boost::ref;

node_inserter_t::node_inserter_t(Xany const &d, nd_list const &c, 
                                 tag_list const &t)
: data(d), children(c), tags(t) {}

node_inserter_t::~node_inserter_t() {}

void node_inserter_t::write_to(writable_structure &o) const {
  o.begin();
    o.data(data);
    for (tag_list::const_iterator it=tags.begin(); it != tags.end(); ++it)
      o.add_tag(*it);
    for_each(range(children), bind(&node_inserter_t::write_to, _1, ref(o)));
  o.end();
}

node_inserter_t cf::S(xany::Xany const &data, tag_list const &tags) {
  return node_inserter_t(data, nd_list(), tags);
}

node_inserter_t cf::S(xany::Xany const &data, string const &tag) {
  return node_inserter_t(data, nd_list(), tag_list(1, tag));
}

node_inserter_t cf::M(nd_list const &children, tag_list const &tags) {
  return node_inserter_t(Xany(), children, tags);
}

node_inserter_t cf::M(nd_list const &children, string const &tag) {
  return node_inserter_t(Xany(), children, tag_list(1, tag));
}

node_inserter_t cf::MD(xany::Xany const &data, nd_list const &children, 
                       tag_list const &tags) {
  return node_inserter_t(data, children, tags);
}

node_inserter_t cf::MD(xany::Xany const &data, nd_list const &children, 
                       string const &tag) {
  return node_inserter_t(data, children, tag_list(1, tag));
}

node_inserter_t cf::C(node_inserter_t const &child, tag_list const &tags) {
  return node_inserter_t(Xany(), nd_list(1, child), tags);
}

node_inserter_t cf::C(node_inserter_t const &child, string const &tag) {
  return node_inserter_t(Xany(), nd_list(1, child), tag_list(1, tag));
}
